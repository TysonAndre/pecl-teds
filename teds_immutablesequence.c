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
#include "teds_util.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"

#include <stdbool.h>

zend_object_handlers teds_handler_ImmutableSequence;
zend_class_entry *teds_ce_ImmutableSequence;

/* This is a placeholder value to distinguish between empty and uninitialized ImmutableSequence instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const zval empty_entry_list[1];

/* Differs from a Vector in that there is no excess capacity field. */
typedef struct _teds_immutablesequence_entries {
	zval *entries;
	uint32_t size;
} teds_immutablesequence_entries;

typedef struct _teds_immutablesequence {
	teds_immutablesequence_entries		array;
	zend_object				std;
} teds_immutablesequence;

/* Used by InternalIterator returned by ImmutableSequence->getIterator() */
typedef struct _teds_immutablesequence_it {
	zend_object_iterator intern;
	uint32_t             current;
} teds_immutablesequence_it;

/*
 * If a size this large is encountered, assume the allocation will likely fail or
 * future changes to the capacity will overflow.
 */
static ZEND_COLD ZEND_NORETURN void teds_error_noreturn_max_immutablesequence_capacity()
{
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\ImmutableSequence capacity");
}

static zend_always_inline teds_immutablesequence *teds_immutablesequence_from_object(zend_object *obj)
{
	return (teds_immutablesequence*)((char*)(obj) - XtOffsetOf(teds_immutablesequence, std));
}

#define Z_IMMUTABLESEQUENCE_P(zv)  teds_immutablesequence_from_object(Z_OBJ_P((zv)))
#define Z_IMMUTABLESEQUENCE_ENTRIES_P(zv)  (&Z_IMMUTABLESEQUENCE_P((zv))->array)

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then entries == NULL
 *   - if size > 0, then entries != NULL
 *   - size is not less than 0
 */
static zend_always_inline bool teds_immutablesequence_entries_empty(const teds_immutablesequence_entries *array)
{
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static zend_always_inline bool teds_immutablesequence_entries_uninitialized(const teds_immutablesequence_entries *array)
{
	if (array->entries == NULL) {
		ZEND_ASSERT(array->size == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->size == 0) || array->size > 0);
	return false;
}

static zend_always_inline void teds_immutablesequence_entries_set_empty_list(teds_immutablesequence_entries *array) {
	array->size = 0;
	array->entries = (zval *)empty_entry_list;
}

static void teds_immutablesequence_entries_init_from_array(teds_immutablesequence_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zval *val;
		zval *entries;
		int i = 0;

		array->size = 0; /* reset size in case emalloc() fails */
		array->entries = entries = safe_emalloc(size, sizeof(zval), 0);
		array->size = size;
		ZEND_HASH_FOREACH_VAL(values, val)  {
			ZEND_ASSERT(i < size);
			ZVAL_COPY_DEREF(&entries[i], val);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->entries = (zval *)empty_entry_list;
	}
}

static void teds_immutablesequence_entries_init_from_traversable(teds_immutablesequence_entries *array, zend_object *obj)
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
static void teds_immutablesequence_copy_range(teds_immutablesequence_entries *array, size_t offset, zval *begin, zval *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	zval *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(to, begin);
		begin++;
		to++;
	}
}

static void teds_immutablesequence_entries_copy_ctor(teds_immutablesequence_entries *to, teds_immutablesequence_entries *from)
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
static void teds_immutablesequence_entries_dtor_range(teds_immutablesequence_entries *array, size_t from, size_t to)
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
static void teds_immutablesequence_entries_dtor(teds_immutablesequence_entries *array)
{
	if (!teds_immutablesequence_entries_empty(array)) {
		teds_immutablesequence_entries_dtor_range(array, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_immutablesequence_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_immutablesequence *intern = teds_immutablesequence_from_object(obj);

	*table = intern->array.entries;
	*n = (int)intern->array.size;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_immutablesequence_get_properties(zend_object *obj)
{
	teds_immutablesequence *intern = teds_immutablesequence_from_object(obj);
	const uint32_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	if (!len) {
		/* Nothing to add or remove - this is immutable. */
		return ht;
	}
	if (zend_hash_num_elements(ht)) {
		/* Already built. Because this is immutable there is no need to rebuild it. */
		return ht;
	}

	zval *entries = intern->array.entries;
	/* Initialize the immutable properties array */
	for (uint32_t i = 0; i < len; i++) {
		Z_TRY_ADDREF_P(&entries[i]);
		zend_hash_index_update(ht, i, &entries[i]);
	}

	return ht;
}

static void teds_immutablesequence_free_storage(zend_object *object)
{
	teds_immutablesequence *intern = teds_immutablesequence_from_object(object);
	teds_immutablesequence_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_immutablesequence_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_immutablesequence *intern;

	intern = zend_object_alloc(sizeof(teds_immutablesequence), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_ImmutableSequence);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_ImmutableSequence;

	if (orig && clone_orig) {
		teds_immutablesequence *other = teds_immutablesequence_from_object(orig);
		teds_immutablesequence_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_immutablesequence_new(zend_class_entry *class_type)
{
	return teds_immutablesequence_new_ex(class_type, NULL, 0);
}

static zend_object *teds_immutablesequence_clone(zend_object *old_object)
{
	zend_object *new_object = teds_immutablesequence_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_immutablesequence_count_elements(zend_object *object, zend_long *count)
{
	const teds_immutablesequence *intern = teds_immutablesequence_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this ImmutableSequence */
PHP_METHOD(Teds_ImmutableSequence, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this ImmutableSequence is empty */
PHP_METHOD(Teds_ImmutableSequence, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(object);
	RETURN_BOOL(intern->array.size == 0);
}


/* Create this from an iterable. TODO: ImmutableSequence::emptySequence()? */
PHP_METHOD(Teds_ImmutableSequence, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(object);

	if (UNEXPECTED(!teds_immutablesequence_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\ImmutableSequence::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_immutablesequence_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_immutablesequence_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
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
	teds_immutablesequence *object   = Z_IMMUTABLESEQUENCE_P(&iter->data);

	if (iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval *teds_immutablesequence_read_offset_helper(teds_immutablesequence *intern, size_t offset)
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

static zval *teds_immutablesequence_it_get_current_data(zend_object_iterator *iter)
{
	teds_immutablesequence_it     *iterator = (teds_immutablesequence_it*)iter;
	teds_immutablesequence *object   = Z_IMMUTABLESEQUENCE_P(&iter->data);

	zval *data = teds_immutablesequence_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return data;
	}
}

static void teds_immutablesequence_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_immutablesequence_it     *iterator = (teds_immutablesequence_it*)iter;
	teds_immutablesequence *object   = Z_IMMUTABLESEQUENCE_P(&iter->data);

	uint32_t offset = iterator->current;
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
	// This is final
	ZEND_ASSERT(ce == teds_ce_ImmutableSequence);
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

	const uint32_t num_entries = zend_hash_num_elements(raw_data);
	teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	if (UNEXPECTED(!teds_immutablesequence_entries_uninitialized(&intern->array))) {
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

static void teds_immutablesequence_entries_init_from_array_values(teds_immutablesequence_entries *array, zend_array *raw_data)
{
	uint32_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		teds_immutablesequence_entries_set_empty_list(array);
		return;
	}
	zval *entries = safe_emalloc(num_entries, sizeof(zval), 0);
	uint32_t actual_size = 0;
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
	zend_object *object = teds_immutablesequence_new(teds_ce_ImmutableSequence);
	teds_immutablesequence *intern = teds_immutablesequence_from_object(object);
	teds_immutablesequence_entries_init_from_array_values(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_ImmutableSequence, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval *entries = intern->array.entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (uint32_t i = 0; i < len; i++) {
			zval *tmp = &entries[i];
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

static zend_always_inline void teds_immutablesequence_get_value_at_offset(zval *return_value, const zval *zval_this, zend_long offset)
{
	teds_immutablesequence_entries *array = Z_IMMUTABLESEQUENCE_ENTRIES_P(zval_this);
	if (UNEXPECTED((zend_ulong) offset >= array->size)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		RETURN_THROWS();
	}
	RETURN_COPY(&array->entries[offset]);
}

PHP_METHOD(Teds_ImmutableSequence, get)
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
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	teds_immutablesequence_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_ImmutableSequence, offsetExists)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	const teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	RETURN_BOOL((zend_ulong) offset < intern->array.size);
}

PHP_METHOD(Teds_ImmutableSequence, indexOf)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i])) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_ImmutableSequence, filter)
{
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	uint32_t size = 0;
	size_t capacity = 0;
	zval *entries = NULL;
	zval operand;
	if (intern->array.size == 0) {
		/* do nothing */
	} else if (ZEND_FCI_INITIALIZED(fci)) {
		zval retval;

		fci.params = &operand;
		fci.param_count = 1;
		fci.retval = &retval;

		const uint32_t src_size = intern->array.size;
		for (uint32_t i = 0; i < src_size; i++) {
			ZVAL_COPY(&operand, &intern->array.entries[i]);
			const int result = zend_call_function(&fci, &fci_cache);
			if (UNEXPECTED(result != SUCCESS || EG(exception))) {
				zval_ptr_dtor(&operand);
cleanup:
				if (entries) {
					for (; size > 0; size--) {
						zval_ptr_dtor(&entries[size]);
					}
					efree(entries);
				}
				return;
			}
			const bool is_true = zend_is_true(&retval);
			zval_ptr_dtor(&retval);
			if (UNEXPECTED(EG(exception))) {
				goto cleanup;
			}
			if (!is_true) {
				zval_ptr_dtor(&operand);
				if (UNEXPECTED(EG(exception))) {
					goto cleanup;
				}
				continue;
			}

			if (size >= capacity) {
				if (entries) {
					capacity = size + intern->array.size - i;
					if (UNEXPECTED(capacity >= TEDS_MAX_ZVAL_COLLECTION_SIZE)) {
						teds_error_noreturn_max_immutablesequence_capacity();
						ZEND_UNREACHABLE();
					}
					entries = safe_erealloc(entries, capacity, sizeof(zval), 0);
				} else {
					ZEND_ASSERT(size == 0);
					ZEND_ASSERT(capacity == 0);
					capacity = intern->array.size > i ? intern->array.size - i : 1;
					entries = safe_emalloc(capacity, sizeof(zval), 0);
				}
			}
			ZEND_ASSERT(size < capacity);
			ZVAL_COPY_VALUE(&entries[size], &operand);
			size++;
		}
	} else {
		const uint32_t src_size = intern->array.size;
		for (uint32_t i = 0; i < src_size; i++) {
			ZVAL_COPY(&operand, &intern->array.entries[i]);
			const bool is_true = zend_is_true(&operand);
			if (!is_true) {
				zval_ptr_dtor(&operand);
				if (UNEXPECTED(EG(exception))) {
					goto cleanup;
				}
				continue;
			}

			if (size >= capacity) {
				if (entries) {
					capacity = size + intern->array.size - i;
					entries = safe_erealloc(entries, capacity, sizeof(zval), 0);
				} else {
					ZEND_ASSERT(size == 0);
					ZEND_ASSERT(capacity == 0);
					capacity = intern->array.size > i ? intern->array.size - i : 1;
					entries = safe_emalloc(capacity, sizeof(zval), 0);
				}
				ZEND_ASSERT(capacity <= TEDS_MAX_ZVAL_COLLECTION_SIZE);
			}
			ZEND_ASSERT(size < capacity);
			ZVAL_COPY_VALUE(&entries[size], &operand);
			size++;
		}
	}

	zend_object *new_object = teds_immutablesequence_new_ex(teds_ce_ImmutableSequence, NULL, 0);
	teds_immutablesequence *new_intern = teds_immutablesequence_from_object(new_object);
	if (size == 0) {
		ZEND_ASSERT(!entries);
		teds_immutablesequence_entries_set_empty_list(&new_intern->array);
		RETURN_OBJ(new_object);
	}
	if (capacity > size) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval));
	}

	new_intern->array.entries = entries;
	new_intern->array.size = size;
	RETURN_OBJ(new_object);
}

PHP_METHOD(Teds_ImmutableSequence, map)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	uint32_t new_capacity = intern->array.size;

	if (new_capacity == 0) {
		zend_object *new_object = teds_immutablesequence_new_ex(teds_ce_ImmutableSequence, NULL, 0);
		teds_immutablesequence *new_intern = teds_immutablesequence_from_object(new_object);
		teds_immutablesequence_entries_set_empty_list(&new_intern->array);
		RETURN_OBJ(new_object);
	}

	zval *entries = emalloc(new_capacity * sizeof(zval));
	uint32_t size = 0;

	zval operand;
	fci.params = &operand;
	fci.param_count = 1;

	do {
		if (UNEXPECTED(size >= new_capacity)) {
			ZEND_ASSERT(size < TEDS_MAX_ZVAL_COLLECTION_SIZE);
			new_capacity = size + 1;
			entries = safe_erealloc(entries, new_capacity, sizeof(zval), 0);
		}
		fci.retval = &entries[size];
		ZVAL_COPY(&operand, &intern->array.entries[size]);
		const int result = zend_call_function(&fci, &fci_cache);
		fci.retval = &entries[size];
		zval_ptr_dtor(&operand);
		if (UNEXPECTED(result != SUCCESS || EG(exception))) {
			if (entries) {
				for (; size > 0; size--) {
					zval_ptr_dtor(&entries[size]);
				}
				efree(entries);
			}
			return;
		}
		size++;
	} while (size < intern->array.size);

	zend_object *new_object = teds_immutablesequence_new_ex(teds_ce_ImmutableSequence, NULL, 0);
	teds_immutablesequence *new_intern = teds_immutablesequence_from_object(new_object);
	if (size == 0) {
		ZEND_ASSERT(!entries);
		teds_immutablesequence_entries_set_empty_list(&new_intern->array);
		RETURN_OBJ(new_object);
	}
	if (UNEXPECTED(new_capacity > size)) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval));
	}

	new_intern->array.entries = entries;
	new_intern->array.size = size;
	RETURN_OBJ(new_object);
}

PHP_METHOD(Teds_ImmutableSequence, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesequence *intern = Z_IMMUTABLESEQUENCE_P(ZEND_THIS);
	zval *it = intern->array.entries;
	const zval *const end = it + intern->array.size;
	for (; it < end; it++) {
		if (zend_is_identical(value, it)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

ZEND_COLD PHP_METHOD(Teds_ImmutableSequence, offsetSet)
{
	zval                  *zindex, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		RETURN_THROWS();
	}
	zend_throw_exception(spl_ce_RuntimeException, "Teds\\ImmutableSequence does not support offsetSet - it is immutable", 0);
	RETURN_THROWS();
}

ZEND_COLD PHP_METHOD(Teds_ImmutableSequence, offsetUnset)
{
	zval                  *offset_zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &offset_zv) == FAILURE) {
		RETURN_THROWS();
	}
	zend_throw_exception(spl_ce_RuntimeException, "Teds\\ImmutableSequence does not support offsetUnset - it is immutable", 0);
	RETURN_THROWS();
}

static zval *teds_immutablesequence_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (UNEXPECTED(!offset_zv || Z_ISUNDEF_P(offset_zv))) {
		return &EG(uninitialized_zval);
	}
	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW_RETURN_NULLPTR(offset, offset_zv);

	if (UNEXPECTED(type != BP_VAR_IS || type != BP_VAR_R)) {
		char *message = type == BP_VAR_W
			? "Teds\\ImmutableSequence does not support offsetSet - it is immutable"
			: "Teds\\ImmutableSequence does not support modification - it is immutable";

		zend_throw_exception(spl_ce_RuntimeException, message, 0);
		return NULL;
	}

	const teds_immutablesequence *intern = teds_immutablesequence_from_object(object);

	(void)rv;

	if (UNEXPECTED(offset < 0 || (zend_ulong) offset >= intern->array.size)) {
		if (type != BP_VAR_IS) {
			zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		}
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

static int teds_immutablesequence_has_dimension(zend_object *object, zval *offset_zv, int check_empty)
{
	zend_long offset;
	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) {
		offset = teds_get_offset(offset_zv);
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
	} else {
		offset = Z_LVAL_P(offset_zv);
	}

	const teds_immutablesequence *intern = teds_immutablesequence_from_object(object);

	if (UNEXPECTED(((zend_ulong) offset) >= intern->array.size || offset < 0)) {
		return 0;
	}

	zval *val = &intern->array.entries[offset];
	if (check_empty) {
		return zend_is_true(val);
	}
	return Z_TYPE_P(val) != IS_NULL;
}

PHP_MINIT_FUNCTION(teds_immutablesequence)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_ImmutableSequence = register_class_Teds_ImmutableSequence(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
	teds_ce_ImmutableSequence->create_object = teds_immutablesequence_new;

	memcpy(&teds_handler_ImmutableSequence, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_ImmutableSequence.offset          = XtOffsetOf(teds_immutablesequence, std);
	teds_handler_ImmutableSequence.clone_obj       = teds_immutablesequence_clone;
	teds_handler_ImmutableSequence.count_elements  = teds_immutablesequence_count_elements;
	teds_handler_ImmutableSequence.get_properties  = teds_immutablesequence_get_properties;
	teds_handler_ImmutableSequence.get_gc          = teds_immutablesequence_get_gc;
	teds_handler_ImmutableSequence.free_obj        = teds_immutablesequence_free_storage;

	teds_handler_ImmutableSequence.read_dimension  = teds_immutablesequence_read_dimension;
	teds_handler_ImmutableSequence.has_dimension   = teds_immutablesequence_has_dimension;

	teds_ce_ImmutableSequence->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_ImmutableSequence->get_iterator = teds_immutablesequence_get_iterator;

	return SUCCESS;
}
