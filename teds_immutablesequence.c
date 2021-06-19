/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c but uses less memory and is immutable */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_immutablesequence_arginfo.h"
#include "teds_immutablesequence.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers spl_handler_ImmutableSequence;
zend_class_entry *spl_ce_ImmutableSequence;

/* This is a placeholder value to distinguish between empty and uninitialized ImmutableSequence instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const zval empty_entry_list[1];

typedef struct _teds_cached_entries {
	size_t size;
	zval *entries;
} teds_cached_entries;

typedef struct _teds_immutablesequence_object {
	teds_cached_entries		array;
	zend_object				std;
} teds_immutablesequence_object;

/* Used by InternalIterator returned by ImmutableSequence->getIterator() */
typedef struct _teds_immutablesequence_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_immutablesequence_it;

static teds_immutablesequence_object *teds_sequence_from_obj(zend_object *obj)
{
	return (teds_immutablesequence_object*)((char*)(obj) - XtOffsetOf(teds_immutablesequence_object, std));
}

#define Z_IMMUTABLESEQUENCE_P(zv)  teds_sequence_from_obj(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then entries == NULL
 *   - if size > 0, then entries != NULL
 *   - size is not less than 0
 */
static bool teds_cached_entries_empty(teds_cached_entries *array)
{
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static bool teds_cached_entries_uninitialized(teds_cached_entries *array)
{
	if (array->entries == NULL) {
		ZEND_ASSERT(array->size == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->size == 0) || array->size > 0);
	return false;
}

/* Initializes the range [from, to) to null. Does not dtor existing entries. */
/* TODO: Delete if this isn't used in the final version
static void teds_cached_entries_init_elems(teds_cached_entries *array, zend_long from, zend_long to)
{
	ZEND_ASSERT(from <= to);
	zval *begin = &array->entries[from].key;
	zval *end = &array->entries[to].key;

	while (begin != end) {
		ZVAL_NULL(begin++);
	}
}
*/

static void teds_cached_entries_init_from_array(teds_cached_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		zval *entries;
		int i = 0;

		array->size = 0; /* reset size in case ecalloc() fails */
		array->entries = entries = safe_emalloc(size, sizeof(zval), 0);
		array->size = size;
		ZEND_HASH_FOREACH_VAL(values, val)  {
			ZEND_ASSERT(i < size);
			ZVAL_COPY(&entries[i], val);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->entries = (zval *)empty_entry_list;
	}
}

static void teds_cached_entries_init_from_traversable(teds_cached_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->entries = NULL;
	zval *entries = NULL;
	zval tmp_obj;
	ZVAL_OBJ(&tmp_obj, obj);
	iter = ce->get_iterator(ce, &tmp_obj, 0);

	if (UNEXPECTED(EG(exception))) {
		return;
	}

	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (funcs->rewind) {
		funcs->rewind(iter);
		if (UNEXPECTED(EG(exception))) {
			return;
		}
	}

	while (funcs->valid(iter) == SUCCESS) {
		if (EG(exception)) {
			break;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		if (size >= capacity) {
			/* TODO: Could use countable and get_count handler to estimate the size of the array to allocate */
			if (entries) {
				capacity *= 2;
				entries = safe_erealloc(entries, capacity, sizeof(zval), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(zval), 0);
			}
		}
		ZVAL_COPY_DEREF(&entries[size], value);
		size++;

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}
	if (capacity > size) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval));
	}

	array->size = size;
	array->entries = entries;
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [begin, end) into the immutablesequence, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_immutablesequence_copy_range(teds_cached_entries *array, size_t offset, zval *begin, zval *end)
{
	ZEND_ASSERT(array->size - offset >= end - begin);

	zval *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(to, begin);
		begin++;
		to++;
	}
}

static void teds_cached_entries_copy_ctor(teds_cached_entries *to, teds_cached_entries *from)
{
	zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->entries = (zval *)empty_entry_list;
		return;
	}

	to->size = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(size, sizeof(zval), 0);
	to->size = size;

	zval *begin = from->entries, *end = from->entries + size;
	teds_immutablesequence_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_cached_entries_dtor_range(teds_cached_entries *array, size_t from, size_t to)
{
	zval *begin = array->entries + from, *end = array->entries + to;
	while (begin != end) {
		zval_ptr_dtor(begin);
		begin++;
	}
}

/* Destructs and frees contents but not the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_cached_entries_dtor(teds_cached_entries *array)
{
	if (!teds_cached_entries_empty(array)) {
		teds_cached_entries_dtor_range(array, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_immutablesequence_object_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_immutablesequence_object *intern = teds_sequence_from_obj(obj);

	*table = intern->array.entries;
	*n = (int)intern->array.size;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_immutablesequence_object_get_properties(zend_object *obj)
{
	teds_immutablesequence_object *intern = teds_sequence_from_obj(obj);
	size_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	if (!len) {
		/* Nothing to add. */
		return ht;
	}
	if (zend_hash_num_elements(ht)) {
		/* Already built. */
		return ht;
	}

	zval *entries = intern->array.entries;
	/* Initialize properties array */
	for (size_t i = 0; i < len; i++) {
		Z_TRY_ADDREF_P(&entries[i]);
		zend_hash_index_update(ht, i, &entries[i]);
	}

	return ht;
}

static void teds_immutablesequence_object_free_storage(zend_object *object)
{
	teds_immutablesequence_object *intern = teds_sequence_from_obj(object);
	teds_cached_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_immutablesequence_object_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_immutablesequence_object *intern;

	intern = zend_object_alloc(sizeof(teds_immutablesequence_object), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == spl_ce_ImmutableSequence);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &spl_handler_ImmutableSequence;

	if (orig && clone_orig) {
		teds_immutablesequence_object *other = teds_sequence_from_obj(orig);
		teds_cached_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_immutablesequence_new(zend_class_entry *class_type)
{
	return teds_immutablesequence_object_new_ex(class_type, NULL, 0);
}


static zend_object *teds_immutablesequence_object_clone(zend_object *old_object)
{
	zend_object *new_object = teds_immutablesequence_object_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_immutablesequence_object_count_elements(zend_object *object, zend_long *count)
{
	teds_immutablesequence_object *intern;

	intern = teds_sequence_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this iterable */
PHP_METHOD(Teds_ImmutableSequence, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(object);
	RETURN_LONG(intern->array.size);
}

/* Create this from an iterable */
PHP_METHOD(Teds_ImmutableSequence, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(object);

	if (UNEXPECTED(!teds_cached_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\ImmutableSequence::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_cached_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_cached_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_ImmutableSequence, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_immutablesequence_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_immutablesequence_it_rewind(zend_object_iterator *iter)
{
	((teds_immutablesequence_it*)iter)->current = 0;
}

static int teds_immutablesequence_it_valid(zend_object_iterator *iter)
{
	teds_immutablesequence_it     *iterator = (teds_immutablesequence_it*)iter;
	teds_immutablesequence_object *object   = Z_IMMUTABLESEQUENCE_P(&iter->data);

	if (iterator->current >= 0 && iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval *teds_immutablesequence_object_read_offset_helper(teds_immutablesequence_object *intern, size_t offset)
{
	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (UNEXPECTED(offset >= intern->array.size)) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

static zval *teds_immutablesequence_it_get_current_data(zend_object_iterator *iter)
{
	teds_immutablesequence_it     *iterator = (teds_immutablesequence_it*)iter;
	teds_immutablesequence_object *object   = Z_IMMUTABLESEQUENCE_P(&iter->data);

	zval *data = teds_immutablesequence_object_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return data;
	}
}

static void teds_immutablesequence_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_immutablesequence_it     *iterator = (teds_immutablesequence_it*)iter;
	teds_immutablesequence_object *object   = Z_IMMUTABLESEQUENCE_P(&iter->data);

	size_t offset = iterator->current;
	if (offset >= object->array.size) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, offset);
	}
}

static void teds_immutablesequence_it_move_forward(zend_object_iterator *iter)
{
	((teds_immutablesequence_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_immutablesequence_it_funcs = {
	teds_immutablesequence_it_dtor,
	teds_immutablesequence_it_valid,
	teds_immutablesequence_it_get_current_data,
	teds_immutablesequence_it_get_current_key,
	teds_immutablesequence_it_move_forward,
	teds_immutablesequence_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_immutablesequence_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_immutablesequence_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_immutablesequence_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_immutablesequence_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_ImmutableSequence, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	const size_t num_entries = zend_hash_num_elements(raw_data);
	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	if (UNEXPECTED(!teds_cached_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	zval *const entries = safe_emalloc(num_entries, sizeof(zval), 0);
	zval *it = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			for (zval *deleteIt = entries; deleteIt < it; deleteIt++) {
				zval_ptr_dtor_nogc(deleteIt);
			}
			efree(entries);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\ImmutableSequence::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}
		ZVAL_COPY_DEREF(it++, val);
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it == entries + num_entries);

	intern->array.size = num_entries;
	intern->array.entries = entries;
}

static void teds_cached_entries_init_from_array_values(teds_cached_entries *array, zend_array *raw_data)
{
	size_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->entries = NULL;
		return;
	}
	zval *entries = safe_emalloc(num_entries, sizeof(zval), 0);
	size_t actual_size = 0;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		ZVAL_COPY_DEREF(&entries[actual_size], val);
		actual_size++;
	} ZEND_HASH_FOREACH_END();

	ZEND_ASSERT(actual_size <= num_entries);
	if (UNEXPECTED(!actual_size)) {
		efree(entries);
		entries = NULL;
	}

	array->entries = entries;
	array->size = actual_size;
}

PHP_METHOD(Teds_ImmutableSequence, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_immutablesequence_new(spl_ce_ImmutableSequence);
	teds_immutablesequence_object *intern = teds_sequence_from_obj(object);
	teds_cached_entries_init_from_array_values(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_ImmutableSequence, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);

	if (teds_cached_entries_empty(&intern->array)) {
		RETURN_EMPTY_ARRAY();
	}
	zval *entries = intern->array.entries;
	size_t len = intern->array.size;
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	ZEND_HASH_FILL_PACKED(flat_entries_array) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i];
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	/* Unlike FixedArray, there's no setSize, so there's no reason to delete indexes */

	RETURN_ARR(flat_entries_array);
}

PHP_METHOD(Teds_ImmutableSequence, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval *entries = intern->array.entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i];
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

static zend_always_inline void teds_immutablesequence_get_value_at_offset(zval *return_value, const zval *object, zend_long offset)
{
	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(object);
	size_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		zend_throw_exception(spl_ce_RuntimeException, "Index out of range", 0);
		RETURN_THROWS();
	}
	RETURN_COPY(&intern->array.entries[offset]);
}

PHP_METHOD(Teds_ImmutableSequence, valueAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutablesequence_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_ImmutableSequence, offsetGet)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	if (Z_TYPE_P(offset_zv) != IS_LONG) {
		offset = spl_offset_convert_to_long(offset_zv);
	} else {
		offset = Z_LVAL_P(offset_zv);
	}

	teds_immutablesequence_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_ImmutableSequence, indexOf)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i])) {
			RETURN_LONG(i);
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_ImmutableSequence, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i])) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_ImmutableSequence, offsetExists)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	if (Z_TYPE_P(offset_zv) != IS_LONG) {
		offset = spl_offset_convert_to_long(offset_zv);
	} else {
		offset = Z_LVAL_P(offset_zv);
	}
	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	size_t len = intern->array.size;
	RETURN_BOOL((zend_ulong) offset < len);
}

PHP_METHOD(Teds_ImmutableSequence, offsetSet)
{
	zval                  *zindex, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		RETURN_THROWS();
	}
	zend_throw_exception(spl_ce_RuntimeException, "ImmutableSequence does not support offsetSet - it is immutable", 0);
	RETURN_THROWS();
}

PHP_METHOD(Teds_ImmutableSequence, offsetUnset)
{
	zval                  *zindex;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		RETURN_THROWS();
	}
	zend_throw_exception(spl_ce_RuntimeException, "ImmutableSequence does not support offsetUnset - it is immutable", 0);
	RETURN_THROWS();
}

static void teds_immutablesequence_return_list(zval *return_value, teds_immutablesequence_object *intern)
{
	teds_convert_zval_list_to_php_array_list(return_value, intern->array.entries, intern->array.size);
}

PHP_METHOD(Teds_ImmutableSequence, jsonSerialize)
{
	/* json_encoder.c will always encode objects as {"0":..., "1":...}, and detects recursion if an object returns its internal property array, so we have to return a new array */
	ZEND_PARSE_PARAMETERS_NONE();
	teds_immutablesequence_object *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	teds_immutablesequence_return_list(return_value, intern);
}

PHP_MINIT_FUNCTION(teds_immutablesequence)
{
	spl_ce_ImmutableSequence = register_class_Teds_ImmutableSequence(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
	spl_ce_ImmutableSequence->create_object = teds_immutablesequence_new;

	memcpy(&spl_handler_ImmutableSequence, &std_object_handlers, sizeof(zend_object_handlers));

	spl_handler_ImmutableSequence.offset          = XtOffsetOf(teds_immutablesequence_object, std);
	spl_handler_ImmutableSequence.clone_obj       = teds_immutablesequence_object_clone;
	spl_handler_ImmutableSequence.count_elements  = teds_immutablesequence_object_count_elements;
	spl_handler_ImmutableSequence.get_properties  = teds_immutablesequence_object_get_properties;
	spl_handler_ImmutableSequence.get_gc          = teds_immutablesequence_object_get_gc;
	spl_handler_ImmutableSequence.dtor_obj        = zend_objects_destroy_object;
	spl_handler_ImmutableSequence.free_obj        = teds_immutablesequence_object_free_storage;

	spl_ce_ImmutableSequence->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	spl_ce_ImmutableSequence->get_iterator = teds_immutablesequence_get_iterator;

	return SUCCESS;
}
