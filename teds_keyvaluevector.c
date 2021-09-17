/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c. Instead of a C array of zvals, this is based on a C array of pairs of zvals for key-value entries */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_keyvaluevector_arginfo.h"
#include "teds_keyvaluevector.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"

#include <stdbool.h>

zend_object_handlers teds_handler_KeyValueVector;
zend_class_entry *teds_ce_KeyValueVector;

/** TODO: Does C guarantee that this has the same memory layout as an array of zvals? */
typedef struct _zval_pair {
	zval key;
	zval value;
} zval_pair;

/* This is a placeholder value to distinguish between empty and uninitialized KeyValueVector instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const zval_pair empty_entry_list[1];

typedef struct _teds_keyvaluevector_entries {
	size_t size;
	size_t capacity;
	zval_pair *entries;
} teds_keyvaluevector_entries;

typedef struct _teds_keyvaluevector {
	teds_keyvaluevector_entries array;
	zend_object                 std;
} teds_keyvaluevector;

/* Used by InternalIterator returned by KeyValueVector->getIterator() */
typedef struct _teds_keyvaluevector_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_keyvaluevector_it;


static teds_keyvaluevector *teds_keyvaluevector_from_obj(zend_object *obj)
{
	return (teds_keyvaluevector*)((char*)(obj) - XtOffsetOf(teds_keyvaluevector, std));
}

#define Z_KEYVALUEVECTOR_P(zv)  teds_keyvaluevector_from_obj(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then entries == NULL
 *   - if size > 0, then entries != NULL
 *   - size is not less than 0
 */
static bool teds_keyvaluevector_entries_empty_size(teds_keyvaluevector_entries *array)
{
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		ZEND_ASSERT(array->capacity >= array->size);
		return false;
	}
	// This vector may have reserved capacity.
	return true;
}

static bool teds_keyvaluevector_entries_empty_capacity(teds_keyvaluevector_entries *array)
{
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	// This vector may have reserved capacity.
	return true;
}

static bool teds_keyvaluevector_entries_uninitialized(teds_keyvaluevector_entries *array)
{
	if (array->entries == NULL) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static void teds_keyvaluevector_raise_capacity(teds_keyvaluevector *intern, const zend_long new_capacity) {
	ZEND_ASSERT(new_capacity > intern->array.capacity);
	if (intern->array.capacity == 0) {
		intern->array.entries = safe_emalloc(new_capacity, sizeof(zval_pair), 0);
	} else {
		intern->array.entries = safe_erealloc(intern->array.entries, new_capacity, sizeof(zval_pair), 0);
	}
	intern->array.capacity = new_capacity;
	ZEND_ASSERT(intern->array.entries != NULL);
}

static inline void teds_keyvaluevector_shrink_capacity(teds_keyvaluevector_entries *array, size_t size, size_t capacity, zval_pair *old_entries) {
	ZEND_ASSERT(size <= capacity);
	ZEND_ASSERT(size == array->size);
	ZEND_ASSERT(capacity > 0);
	ZEND_ASSERT(capacity < array->capacity);
	ZEND_ASSERT(old_entries == array->entries);
	zval_pair *new_entries = safe_emalloc(capacity, sizeof(zval_pair), 0);
	ZEND_ASSERT(new_entries != NULL);
	memcpy(new_entries, old_entries, size * sizeof(zval_pair));

	array->entries = new_entries;
	array->capacity = capacity;
	efree(old_entries);
}

/* Initializes the range [from, to) to null. Does not dtor existing entries. */
/* TODO: Delete if this isn't used in the final version
static void teds_keyvaluevector_entries_init_elems(teds_keyvaluevector_entries *array, zend_long from, zend_long to)
{
	ZEND_ASSERT(from <= to);
	zval *begin = &array->entries[from].key;
	zval *end = &array->entries[to].key;

	while (begin != end) {
		ZVAL_NULL(begin++);
	}
}
*/

static inline void teds_keyvaluevector_entries_set_empty_list(teds_keyvaluevector_entries *array) {
	array->size = 0;
	array->capacity = 0;
	array->entries = (zval_pair *)empty_entry_list;
}

static void teds_keyvaluevector_entries_init_from_array(teds_keyvaluevector_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		zval_pair *entries;
		int i = 0;

		array->size = 0; /* reset size in case emalloc() fails */
		array->entries = entries = safe_emalloc(size, sizeof(zval_pair), 0);
		array->size = size;
		array->capacity = size;
		ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
			ZEND_ASSERT(i < size);
			if (skey) {
				ZVAL_STR_COPY(&entries[i].key, skey);
			} else {
				ZVAL_LONG(&entries[i].key, nkey);
			}
			ZVAL_COPY_DEREF(&entries[i].value, val);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		teds_keyvaluevector_entries_set_empty_list(array);
	}
}

static void teds_keyvaluevector_entries_init_from_traversable(teds_keyvaluevector_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->entries = NULL;
	zval_pair *entries = NULL;
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
		zval key;
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		if (funcs->get_current_key) {
			funcs->get_current_key(iter, &key);
		} else {
			ZVAL_NULL(&key);
		}
		if (UNEXPECTED(EG(exception))) {
			zval_ptr_dtor(&key);
			break;
		}

		if (size >= capacity) {
			/* TODO: Could use countable and get_count handler to estimate the size of the array to allocate */
			if (entries) {
				capacity *= 2;
				entries = safe_erealloc(entries, capacity, sizeof(zval_pair), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(zval_pair), 0);
			}
		}
		/* The key's reference count was already increased */
		ZVAL_COPY_VALUE(&entries[size].key, &key);
		ZVAL_COPY_DEREF(&entries[size].value, value);
		size++;

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}
	if (capacity > size) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval_pair));
	}

	array->size = size;
	array->capacity = size;
	array->entries = entries;
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [begin, end) into the keyvaluevector, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_keyvaluevector_copy_range(teds_keyvaluevector_entries *array, size_t offset, zval_pair *begin, zval_pair *end)
{
	ZEND_ASSERT(array->size - offset >= end - begin);

	zval_pair *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		ZVAL_COPY(&to->value, &begin->value);
		begin++;
		to++;
	}
}

static void teds_keyvaluevector_entries_copy_ctor(teds_keyvaluevector_entries *to, teds_keyvaluevector_entries *from)
{
	zend_long size = from->size;
	if (!size) {
		teds_keyvaluevector_entries_set_empty_list(to);
		return;
	}

	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0;
	to->entries = safe_emalloc(size, sizeof(zval_pair), 0);
	to->size = size;
	to->capacity = size;

	zval_pair *begin = from->entries, *end = from->entries + size;
	teds_keyvaluevector_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_keyvaluevector_entries_dtor_range(teds_keyvaluevector_entries *array, size_t from, size_t to)
{
	zval_pair *begin = array->entries + from, *end = array->entries + to;
	while (begin != end) {
		zval_ptr_dtor(&begin->key);
		zval_ptr_dtor(&begin->value);
		begin++;
	}
}

/* Destructs and frees contents but not the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_keyvaluevector_entries_dtor(teds_keyvaluevector_entries *array)
{
	if (!teds_keyvaluevector_entries_empty_capacity(array)) {
		teds_keyvaluevector_entries_dtor_range(array, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_keyvaluevector_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_keyvaluevector *intern = teds_keyvaluevector_from_obj(obj);

	*table = &intern->array.entries[0].key;
	// Each offset has 1 key and 1 value
	*n = (int)intern->array.size * 2;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_keyvaluevector_get_properties(zend_object *obj)
{
	/* This is mutable and the size may change. Update or delete elements as needed. */
	teds_keyvaluevector *intern = teds_keyvaluevector_from_obj(obj);
	HashTable *ht = zend_std_get_properties(obj);

	/* Re-initialize properties array */

	// Note that destructors may mutate the original array,
	// so we fetch the size and circular buffer each time to avoid invalid memory accesses.
	for (size_t i = 0; i < intern->array.size; i++) {
		zval_pair *entries = intern->array.entries;
		zval tmp;
		Z_TRY_ADDREF_P(&entries[i].key);
		Z_TRY_ADDREF_P(&entries[i].value);
		ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
		zend_hash_index_update(ht, i, &tmp);
	}

	const size_t properties_size = zend_hash_num_elements(ht);
	if (UNEXPECTED(properties_size > intern->array.size)) {
		for (size_t i = intern->array.size; i < properties_size; i++) {
			zend_hash_index_del(ht, i);
		}
	}

	return ht;
}

static void teds_keyvaluevector_free_storage(zend_object *object)
{
	teds_keyvaluevector *intern = teds_keyvaluevector_from_obj(object);
	teds_keyvaluevector_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_keyvaluevector_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_keyvaluevector *intern;

	intern = zend_object_alloc(sizeof(teds_keyvaluevector), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_KeyValueVector);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_KeyValueVector;

	if (orig && clone_orig) {
		teds_keyvaluevector *other = teds_keyvaluevector_from_obj(orig);
		teds_keyvaluevector_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_keyvaluevector_new(zend_class_entry *class_type)
{
	return teds_keyvaluevector_new_ex(class_type, NULL, 0);
}


static zend_object *teds_keyvaluevector_clone(zend_object *old_object)
{
	zend_object *new_object = teds_keyvaluevector_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_keyvaluevector_count_elements(zend_object *object, zend_long *count)
{
	teds_keyvaluevector *intern = teds_keyvaluevector_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this keyvaluevector */
PHP_METHOD(Teds_KeyValueVector, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get capacity of this KeyValueVector */
PHP_METHOD(Teds_KeyValueVector, capacity)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(object);
	RETURN_LONG(intern->array.capacity);
}

/* Free elements and backing storage of this KeyValueVector */
PHP_METHOD(Teds_KeyValueVector, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	if (intern->array.capacity == 0) {
		/* Nothing to clear */
		return;
	}
	/* Immediately make the original storage inaccessible and set count/capacity to 0 in case destructors modify the vector */
	const size_t old_size = intern->array.size;
	zval *const old_entries = (zval *)intern->array.entries;
	teds_keyvaluevector_entries_set_empty_list(&intern->array);
	teds_zval_dtor_range(old_entries, old_size * 2);
	efree(old_entries);
}

/* Set size of this KeyValueVector */
PHP_METHOD(Teds_KeyValueVector, setSize)
{
	zend_long size;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t old_size = intern->array.size;
	if (size > old_size) {
		/* Raise the capacity as needed and fill the space with nulls */
		if (size > intern->array.capacity) {
			teds_keyvaluevector_raise_capacity(intern, size);
		}
		intern->array.size = size;
		zval_pair * const entries = intern->array.entries;
		for (size_t i = old_size; i < size; i++) {
			ZVAL_NULL(&entries[i].key);
			ZVAL_NULL(&entries[i].value);
		}
		return;
	}
	/* Reduce the size and invalidate memory. If a destructor unexpectedly changes the size then read the new size and keep removing elements. */
	const size_t entries_to_remove = old_size - size;
	if (entries_to_remove == 0) {
		return;
	}

	zval_pair *old_entries = intern->array.entries;
	zval *old_copy;
	if (size == 0) {
		old_copy = (zval *)old_entries;
		teds_keyvaluevector_entries_set_empty_list(&intern->array);
	} else {
		old_copy = teds_zval_copy_range((const zval *)&old_entries[size], entries_to_remove * 2);

		intern->array.size = size;

		if (size * 4 < intern->array.capacity) {
			const size_t size = old_size - 1;
			const size_t capacity = size > 2 ? size * 2 : 4;
			if (capacity < intern->array.capacity) {
				teds_keyvaluevector_shrink_capacity(&intern->array, size, capacity, old_entries);
			}
		}
	}

	teds_zval_dtor_range(old_copy, entries_to_remove * 2);
	efree(old_copy);
}

/* Create this from an iterable */
PHP_METHOD(Teds_KeyValueVector, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(object);

	if (UNEXPECTED(!teds_keyvaluevector_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\KeyValueVector::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}
	if (!iterable) {
		teds_keyvaluevector_entries_set_empty_list(&intern->array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_keyvaluevector_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_keyvaluevector_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_KeyValueVector, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_keyvaluevector_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_keyvaluevector_it_rewind(zend_object_iterator *iter)
{
	((teds_keyvaluevector_it*)iter)->current = 0;
}

static int teds_keyvaluevector_it_valid(zend_object_iterator *iter)
{
	teds_keyvaluevector_it     *iterator = (teds_keyvaluevector_it*)iter;
	teds_keyvaluevector *object   = Z_KEYVALUEVECTOR_P(&iter->data);

	if (iterator->current >= 0 && iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval_pair *teds_keyvaluevector_read_offset_helper(teds_keyvaluevector *intern, size_t offset)
{
	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (UNEXPECTED(offset >= intern->array.size)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

static zval *teds_keyvaluevector_it_get_current_data(zend_object_iterator *iter)
{
	teds_keyvaluevector_it     *iterator = (teds_keyvaluevector_it*)iter;
	teds_keyvaluevector *object   = Z_KEYVALUEVECTOR_P(&iter->data);

	zval_pair *data = teds_keyvaluevector_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_keyvaluevector_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_keyvaluevector_it     *iterator = (teds_keyvaluevector_it*)iter;
	teds_keyvaluevector *object   = Z_KEYVALUEVECTOR_P(&iter->data);

	zval_pair *data = teds_keyvaluevector_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_keyvaluevector_it_move_forward(zend_object_iterator *iter)
{
	((teds_keyvaluevector_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_keyvaluevector_it_funcs = {
	teds_keyvaluevector_it_dtor,
	teds_keyvaluevector_it_valid,
	teds_keyvaluevector_it_get_current_data,
	teds_keyvaluevector_it_get_current_key,
	teds_keyvaluevector_it_move_forward,
	teds_keyvaluevector_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_keyvaluevector_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_keyvaluevector_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_keyvaluevector_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_keyvaluevector_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_KeyValueVector, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	size_t raw_size = zend_hash_num_elements(raw_data);
	if (UNEXPECTED(raw_size % 2 != 0)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Odd number of elements", 0);
		RETURN_THROWS();
	}
	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	if (UNEXPECTED(!teds_keyvaluevector_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	size_t num_entries = raw_size / 2;
	zval_pair *entries = safe_emalloc(num_entries, sizeof(zval_pair), 0);
	zval *it = &entries[0].key;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			for (zval *deleteIt = &entries[0].key; deleteIt < it; deleteIt++) {
				zval_ptr_dtor_nogc(deleteIt);
			}
			efree(entries);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\KeyValueVector::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}
		ZVAL_COPY_DEREF(it++, val);
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it == &entries[0].key + raw_size);

	intern->array.size = num_entries;
	intern->array.capacity = num_entries;
	intern->array.entries = entries;
}

static bool teds_cached_entry_copy_entry_from_array_pair(zval_pair *pair, zval *raw_val)
{
	ZVAL_DEREF(raw_val);
	if (UNEXPECTED(Z_TYPE_P(raw_val) != IS_ARRAY)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find pair in array but got non-array", 0);
		return false;
	}
	HashTable *ht = Z_ARRVAL_P(raw_val);
	zval *key = zend_hash_index_find(ht, 0);
	if (UNEXPECTED(!key)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find key at index 0", 0);
		return false;
	}
	zval *value = zend_hash_index_find(ht, 1);
	if (UNEXPECTED(!value)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find value at index 1", 0);
		return false;
	}
	ZVAL_COPY_DEREF(&pair->key, key);
	ZVAL_COPY_DEREF(&pair->value, value);
	return true;
}


static void teds_keyvaluevector_entries_init_from_array_pairs(teds_keyvaluevector_entries *array, zend_array *raw_data)
{
	size_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->capacity = 0;
		array->entries = NULL;
		return;
	}
	zval_pair *entries = safe_emalloc(num_entries, sizeof(zval_pair), 0);
	size_t actual_size = 0;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		if (!teds_cached_entry_copy_entry_from_array_pair(&entries[actual_size], val)) {
			break;
		}
		actual_size++;
	} ZEND_HASH_FOREACH_END();

	ZEND_ASSERT(actual_size <= num_entries);
	if (UNEXPECTED(!actual_size)) {
		efree(entries);
		entries = NULL;
		num_entries = 0;
	}

	array->entries = entries;
	array->size = actual_size;
	array->capacity = num_entries;
}

static void teds_keyvaluevector_entries_init_from_traversable_pairs(teds_keyvaluevector_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->capacity = 0;
	array->entries = NULL;
	zval_pair *entries = NULL;
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
		zval *pair = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		if (size >= capacity) {
			/* TODO: Could use countable and get_count handler to estimate the size of the array to allocate */
			if (entries) {
				capacity *= 2;
				entries = safe_erealloc(entries, capacity, sizeof(zval_pair), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(zval_pair), 0);
			}
		}
		if (!teds_cached_entry_copy_entry_from_array_pair(&entries[size], pair)) {
			break;
		}
		size++;

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}
	if (capacity > size) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval_pair));
	}

	array->size = size;
	array->capacity = size;
	array->entries = entries;
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static zend_object* create_from_pairs(zval *iterable) {
	zend_object *object = teds_keyvaluevector_new(teds_ce_KeyValueVector);
	teds_keyvaluevector *intern = teds_keyvaluevector_from_obj(object);
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_keyvaluevector_entries_init_from_array_pairs(&intern->array, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_keyvaluevector_entries_init_from_traversable_pairs(&intern->array, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_KeyValueVector, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_KeyValueVector, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_keyvaluevector_new(teds_ce_KeyValueVector);
	teds_keyvaluevector *intern = teds_keyvaluevector_from_obj(object);
	teds_keyvaluevector_entries_init_from_array_pairs(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_KeyValueVector, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);

	if (teds_keyvaluevector_entries_empty_size(&intern->array)) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = intern->array.entries;
	size_t len = intern->array.size;
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	ZEND_HASH_FILL_PACKED(flat_entries_array) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].key;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
			tmp = &entries[i].value;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();

	RETURN_ARR(flat_entries_array);
}

PHP_METHOD(Teds_KeyValueVector, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = intern->array.entries;
	zend_array *keys = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(keys);

	/* Go through keys and add values to the return array */
	ZEND_HASH_FILL_PACKED(keys) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].key;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(keys);
}

PHP_METHOD(Teds_KeyValueVector, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = intern->array.entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].value;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_KeyValueVector, keyAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		RETURN_THROWS();
	}
	RETURN_COPY(&intern->array.entries[offset].key);
}

PHP_METHOD(Teds_KeyValueVector, valueAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		RETURN_THROWS();
	}
	RETURN_COPY(&intern->array.entries[offset].value);
}

PHP_METHOD(Teds_KeyValueVector, indexOfKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(key, &entries[i].key)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_KeyValueVector, indexOfValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i].value)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_KeyValueVector, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(key, &entries[i].key)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_KeyValueVector, containsValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i].value)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_KeyValueVector, setKeyAt)
{
	zend_long offset;
	zval *key;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		RETURN_THROWS();
	}
	zval old_key;
	zval *old_key_ptr = &intern->array.entries[offset].key;
	ZVAL_COPY_VALUE(&old_key, old_key_ptr);
	ZVAL_COPY(old_key_ptr, key);
	zval_ptr_dtor(&old_key);
}

PHP_METHOD(Teds_KeyValueVector, setValueAt)
{
	zend_long offset;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		RETURN_THROWS();
	}
	zval old_value;
	zval *old_value_ptr = &intern->array.entries[offset].value;
	ZVAL_COPY_VALUE(&old_value, old_value_ptr);
	ZVAL_COPY(old_value_ptr, value);
	zval_ptr_dtor(&old_value);
}

PHP_METHOD(Teds_KeyValueVector, push)
{
	zval *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t old_size = intern->array.size;
	const size_t old_capacity = intern->array.capacity;

	if (old_size >= old_capacity) {
		ZEND_ASSERT(old_size == old_capacity);
		teds_keyvaluevector_raise_capacity(intern, old_size ? old_size * 2 : 4);
	}
	zval_pair *dest = &intern->array.entries[old_size];
	ZVAL_COPY(&dest->key, key);
	ZVAL_COPY(&dest->value, value);
	intern->array.size++;
}

PHP_METHOD(Teds_KeyValueVector, pop)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t old_size = intern->array.size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty Teds\\KeyValueVector", 0);
		RETURN_THROWS();
	}
	const size_t old_capacity = intern->array.capacity;
	intern->array.size--;
	zval_pair *entry = &intern->array.entries[intern->array.size];
	RETVAL_ARR(zend_new_pair(&entry->key, &entry->value));
	if (old_size * 4 < old_capacity) {
		/* Shrink the storage if only a quarter of the capacity is used  */
		const size_t size = old_size - 1;
		zval_pair *old_entries = intern->array.entries;
		const size_t capacity = size > 2 ? size * 2 : 4;
		if (capacity < old_capacity) {
			teds_keyvaluevector_shrink_capacity(&intern->array, size, capacity, old_entries);
		}
	}
}

PHP_METHOD(Teds_KeyValueVector, shrinkToFit)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	const size_t size = intern->array.size;
	const size_t old_capacity = intern->array.capacity;
	if (size >= old_capacity) {
		ZEND_ASSERT(size == old_capacity);
		return;
	}

	if (size == 0) {
		efree(intern->array.entries);
		intern->array.entries = (zval_pair *)empty_entry_list;
	} else {
		intern->array.entries = safe_erealloc(intern->array.entries, size, sizeof(zval_pair), 0);
	}
	intern->array.capacity = size;
}


static void teds_keyvaluevector_return_pairs(zval *return_value, teds_keyvaluevector *intern)
{
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	zval_pair *entries = intern->array.entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (size_t i = 0; i < len; i++) {
			zval tmp;
			Z_TRY_ADDREF_P(&entries[i].key);
			Z_TRY_ADDREF_P(&entries[i].value);
			ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
			ZEND_HASH_FILL_ADD(&tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_KeyValueVector, jsonSerialize)
{
	/* json_encoder.c will always encode objects as {"0":..., "1":...}, and detects recursion if an object returns its internal property array, so we have to return a new array */
	ZEND_PARSE_PARAMETERS_NONE();
	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	teds_keyvaluevector_return_pairs(return_value, intern);
}

PHP_METHOD(Teds_KeyValueVector, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_keyvaluevector *intern = Z_KEYVALUEVECTOR_P(ZEND_THIS);
	teds_keyvaluevector_return_pairs(return_value, intern);
}

PHP_MINIT_FUNCTION(teds_keyvaluevector)
{
	teds_ce_KeyValueVector = register_class_Teds_KeyValueVector(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce);
	teds_ce_KeyValueVector->create_object = teds_keyvaluevector_new;

	memcpy(&teds_handler_KeyValueVector, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_KeyValueVector.offset          = XtOffsetOf(teds_keyvaluevector, std);
	teds_handler_KeyValueVector.clone_obj       = teds_keyvaluevector_clone;
	teds_handler_KeyValueVector.count_elements  = teds_keyvaluevector_count_elements;
	teds_handler_KeyValueVector.get_properties  = teds_keyvaluevector_get_properties;
	teds_handler_KeyValueVector.get_gc          = teds_keyvaluevector_get_gc;
	teds_handler_KeyValueVector.dtor_obj        = zend_objects_destroy_object;
	teds_handler_KeyValueVector.free_obj        = teds_keyvaluevector_free_storage;

	teds_ce_KeyValueVector->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_KeyValueVector->get_iterator = teds_keyvaluevector_get_iterator;

	return SUCCESS;
}
