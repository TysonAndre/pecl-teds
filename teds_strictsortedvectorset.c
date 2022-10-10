/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on teds_immutableiterable.c.
 * Instead of a C array of zvals, this is based on a C array of zvals.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_strictsortedvectorset_arginfo.h"
#include "teds_strictsortedvectorset.h"
#include "teds_util.h"
#include "teds_interfaces.h"
#include "teds_exceptions.h"
#include "teds_vector.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StrictSortedVectorSet;
zend_class_entry *teds_ce_StrictSortedVectorSet;

#define teds_strictsortedvectorset_entries_raise_capacity teds_vector_entries_raise_capacity
#define teds_strictsortedvectorset_count_elements teds_vector_count_elements
#define teds_strictsortedvectorset_clear teds_vector_clear
#define teds_strictsortedvectorset_get_properties_for teds_vector_get_properties_for
#define teds_strictsortedvectorset_get_gc teds_vector_get_gc
#define teds_strictsortedvectorset_free_storage teds_vector_free_storage

typedef teds_vector_entries teds_strictsortedvectorset_entries;
typedef teds_vector teds_strictsortedvectorset;

typedef struct _teds_strictsortedvectorset_search_result {
	zval *entry;
	bool found;
} teds_strictsortedvectorset_search_result;

static teds_strictsortedvectorset_search_result teds_strictsortedvectorset_entries_sorted_search_for_key(const teds_strictsortedvectorset_entries *array, zval *key);
static teds_strictsortedvectorset_search_result teds_strictsortedvectorset_entries_sorted_search_for_key_probably_largest(const teds_strictsortedvectorset_entries *array, zval *key);

static bool teds_strictsortedvectorset_entries_insert(teds_strictsortedvectorset_entries *array, zval *key, bool probably_largest) {
	ZEND_ASSERT(Z_TYPE_P(key) >= IS_NULL && Z_TYPE_P(key) <= IS_RESOURCE);
	teds_strictsortedvectorset_search_result result = probably_largest
		? teds_strictsortedvectorset_entries_sorted_search_for_key_probably_largest(array, key)
		: teds_strictsortedvectorset_entries_sorted_search_for_key(array, key);
	if (result.found) {
		return false;
	}
	/* Reallocate and insert (insertion sort) */
	zval *entry = result.entry;
	if (array->size >= array->capacity) {
		const uint32_t new_offset = result.entry - array->entries;
		ZEND_ASSERT(array->size == array->capacity);
		const uint32_t new_capacity = teds_strictsortedvectorset_next_pow2_capacity(array->size + 1);
		teds_strictsortedvectorset_entries_raise_capacity(array, new_capacity);
		entry = array->entries + new_offset;
	}
	zval *entries = array->entries;

	if (UNEXPECTED(array->active_iterators.first)) {
		teds_vector_adjust_iterators_before_insert(array, array->active_iterators.first, entry - entries, 1);
	}

	for (zval *it = entries + array->size; it > entry; it--) {
		ZVAL_COPY_VALUE(&it[0], &it[-1]);
	}

	ZVAL_COPY(entry, key);
	array->size++;
	TEDS_SET_SHOULD_REBUILD_PROPERTIES(array, true);
	return true;
}

/* Used by InternalIterator returned by StrictSortedVectorSet->getIterator() */
typedef teds_vector_it teds_strictsortedvectorset_it;

static teds_strictsortedvectorset *teds_strictsortedvectorset_from_object(zend_object *obj)
{
	return (teds_strictsortedvectorset*)((char*)(obj) - XtOffsetOf(teds_strictsortedvectorset, std));
}

#define Z_STRICTSORTEDVECTORSET_P(zv)  teds_strictsortedvectorset_from_object(Z_OBJ_P((zv)))
#define Z_STRICTSORTEDVECTORSET_ENTRIES_P(zv)  (&Z_STRICTSORTEDVECTORSET_P((zv))->array)

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_strictsortedvectorset_entries_empty_capacity(teds_strictsortedvectorset_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static bool teds_strictsortedvectorset_entries_uninitialized(teds_strictsortedvectorset_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->entries == NULL) {
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static zend_always_inline zval *teds_strictsortedvectorset_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(zval), 0);
}

static bool teds_strictsortedvectorset_entries_is_sorted(zval *entries, const uint32_t size)
{
	for (size_t i = 1; i < size; i++) {
		if (teds_stable_compare(&entries[i - 1], &entries[i]) >= 0) {
			return false;
		}
	}
	return true;
}

static void teds_strictsortedvectorset_entries_sort_and_deduplicate(teds_strictsortedvectorset_entries *array)
{
	const uint32_t size = array->size;
	if (size < 2) {
		return;
	}
	zval *entries = array->entries;
	if (teds_strictsortedvectorset_entries_is_sorted(entries, array->size)) {
		return;
	}
	qsort(entries, size, sizeof(zval), teds_stable_compare_wrap);
	for (uint32_t i = 1; i < size; i++) {
		int result = teds_stable_compare(&entries[i - 1], &entries[i]);
		if (result >= 0) {
			/* Remove duplicates */
			ZEND_ASSERT(result == 0);
			zval_ptr_dtor(&entries[i]);
			uint32_t new_size = i;
			i++;
			for (; i < size; i++) {
				result = teds_stable_compare(&entries[new_size - 1], &entries[i]);
				ZEND_ASSERT(result <= 0);
				if (result < 0) {
					ZVAL_COPY_VALUE(&entries[new_size], &entries[i]);
					new_size++;
				} else {
					zval_ptr_dtor(&entries[i]);
				}
			}
			array->size = new_size;
			array->capacity = new_size;
			array->entries = safe_erealloc(entries, new_size, sizeof(zval), 0);
			return;
		}
	}
	/* There were no duplicates */
}

static void teds_strictsortedvectorset_entries_init_from_array(teds_strictsortedvectorset_entries *array, zend_array *values)
{
	const uint32_t size = zend_hash_num_elements(values);
	if (size > 0) {
		zval *val;
		zval *entries;

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = entries = teds_strictsortedvectorset_allocate_entries(size);
		uint32_t i = 0;
		array->capacity = size;
		ZEND_HASH_FOREACH_VAL(values, val) {
			ZVAL_COPY_DEREF(&entries[i], val);
			i++;
		} ZEND_HASH_FOREACH_END();

		ZEND_ASSERT(i == size);
		array->size = size;
		array->capacity = size;
		TEDS_SET_SHOULD_REBUILD_PROPERTIES(array, true);

		if (size > 1) {
			teds_strictsortedvectorset_entries_sort_and_deduplicate(array);
		}
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (zval *)empty_entry_list;
	}
}

static void teds_strictsortedvectorset_entries_init_from_traversable(teds_strictsortedvectorset_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->capacity = 0;
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
			goto cleanup_iter;
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

	array->size = size;
	array->capacity = capacity;
	array->entries = entries;
	TEDS_SET_SHOULD_REBUILD_PROPERTIES(array, size > 0);
	if (size > 1) {
		teds_strictsortedvectorset_entries_sort_and_deduplicate(array);
	}
cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [begin, end) into the strictsortedvectorset, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_strictsortedvectorset_copy_range(teds_strictsortedvectorset_entries *array, size_t offset, zval *begin, zval *end)
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

static void teds_strictsortedvectorset_entries_copy_ctor(teds_strictsortedvectorset_entries *to, teds_strictsortedvectorset_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (zval *)empty_entry_list;
		return;
	}

	const uint32_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(zval), 0);
	to->size = size;
	to->capacity = capacity;
	TEDS_SET_SHOULD_REBUILD_PROPERTIES(to, true);

	zval *begin = from->entries, *end = from->entries + size;
	teds_strictsortedvectorset_copy_range(to, 0, begin, end);
}

static zend_object *teds_strictsortedvectorset_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_strictsortedvectorset *intern;

	intern = zend_object_alloc(sizeof(teds_strictsortedvectorset), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StrictSortedVectorSet);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_StrictSortedVectorSet;

	if (orig && clone_orig) {
		teds_strictsortedvectorset *other = teds_strictsortedvectorset_from_object(orig);
		teds_strictsortedvectorset_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_strictsortedvectorset_new(zend_class_entry *class_type)
{
	return teds_strictsortedvectorset_new_ex(class_type, NULL, 0);
}


static zend_object *teds_strictsortedvectorset_clone(zend_object *old_object)
{
	zend_object *new_object = teds_strictsortedvectorset_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

/* Create this from an iterable */
PHP_METHOD(Teds_StrictSortedVectorSet, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(ZEND_THIS);

	if (UNEXPECTED(!teds_strictsortedvectorset_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictSortedVectorSet::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (zval *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_strictsortedvectorset_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_strictsortedvectorset_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StrictSortedVectorSet, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static zval *teds_strictsortedvectorset_read_offset_helper(teds_strictsortedvectorset *intern, size_t offset)
{
	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (UNEXPECTED(offset >= intern->array.size)) {
		teds_throw_invalid_sequence_index_exception();
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

static void teds_strictsortedvectorset_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_strictsortedvectorset_it     *iterator = (teds_strictsortedvectorset_it*)iter;
	teds_strictsortedvectorset *object   = Z_STRICTSORTEDVECTORSET_P(&iter->data);

	zval *data = teds_strictsortedvectorset_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, data);
	}
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_strictsortedvectorset_it_funcs = {
	teds_vector_it_dtor,
	teds_vector_it_valid,
	teds_vector_it_get_current_data,
	teds_strictsortedvectorset_it_get_current_key, /* Yields the set value as a key as well ($value => $value) */
	teds_vector_it_move_forward,
	teds_vector_it_rewind,
	NULL,
	teds_internaliterator_get_gc,
};


zend_object_iterator *teds_strictsortedvectorset_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_strictsortedvectorset_it *iterator;
	(void)ce;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_strictsortedvectorset_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	zend_object *obj = Z_OBJ_P(object);
	ZVAL_OBJ_COPY(&iterator->intern.data, obj);
	iterator->intern.funcs = &teds_strictsortedvectorset_it_funcs;

	teds_intrusive_dllist_prepend(&teds_strictsortedvectorset_from_object(obj)->array.active_iterators, &iterator->dllist_node);

	return &iterator->intern;
}

PHP_METHOD(Teds_StrictSortedVectorSet, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	uint32_t raw_size = zend_hash_num_elements(raw_data);
	teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(ZEND_THIS);
	teds_strictsortedvectorset_entries *array = &intern->array;
	if (UNEXPECTED(!teds_strictsortedvectorset_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		array->entries = (zval *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(array->entries == NULL);

	const size_t capacity = teds_strictsortedvectorset_next_pow2_capacity(raw_size);
	zval *entries = safe_emalloc(capacity, sizeof(zval), 0);
	array->size = 0;
	array->capacity = capacity;
	array->entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictsortedvectorset_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictSortedVectorSet::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
		teds_strictsortedvectorset_entries_insert(array, val, true);
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Teds_StrictSortedVectorSet, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_strictsortedvectorset_new(teds_ce_StrictSortedVectorSet);
	teds_strictsortedvectorset *intern = teds_strictsortedvectorset_from_object(object);
	teds_strictsortedvectorset_entries_init_from_array(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_StrictSortedVectorSet, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictsortedvectorset_entries *const array = Z_STRICTSORTEDVECTORSET_ENTRIES_P(ZEND_THIS);
	if (!array->size) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *const values = zend_new_array(array->size);
	/* Initialize return array */

	/* Go through values and add values to the return array */
	for (uint32_t i = 0; i < array->size; i++) {
		zval *key = &array->entries[i];

		Z_TRY_ADDREF_P(key);
		array_set_zval_key(values, key, key);
		zval_ptr_dtor_nogc(key);
		if (UNEXPECTED(EG(exception))) {
			zend_array_destroy(values);
			RETURN_THROWS();
		}
	}
	RETURN_ARR(values);
}

static teds_strictsortedvectorset_search_result teds_strictsortedvectorset_entries_sorted_search_for_key(const teds_strictsortedvectorset_entries *array, zval *key)
{
	/* Currently, this is a binary search in an array, but later it would be a tree lookup. */
	zval *const entries = array->entries;
	uint32_t start = 0;
	uint32_t end = array->size;
	while (start < end) {
		uint32_t mid = start + (end - start)/2;
		zval *e = &entries[mid];
		int comparison = teds_stable_compare(key, e);
		if (comparison > 0) {
			/* This key is greater than the value at the midpoint. Search the right half. */
			start = mid + 1;
		} else if (comparison < 0) {
			/* This key is less than the value at the midpoint. Search the left half. */
			end = mid;
		} else {
			teds_strictsortedvectorset_search_result result;
			result.found = true;
			result.entry = e;
			return result;
		}
	}
	/* The entry is the position in the array at which the new value should be inserted. */
	teds_strictsortedvectorset_search_result result;
	result.found = false;
	result.entry = &entries[start];
	return result;
}

static teds_strictsortedvectorset_search_result teds_strictsortedvectorset_entries_sorted_search_for_key_probably_largest(const teds_strictsortedvectorset_entries *array, zval *key)
{
	/* Currently, this is a binary search in an array, but later it would be a tree lookup. */
	zval *const entries = array->entries;
	uint32_t end = array->size;
	uint32_t start = 0;
	if (end > 0) {
		uint32_t mid = end - 1;
		/* This is written in a way that would be fastest for branch prediction if key is larger than the last value in the array. mid is initially end - 1. */
		while (true) {
			zval *e = &entries[mid];
			int comparison = teds_stable_compare(key, e);
			if (comparison > 0) {
				/* This key is greater than the value at the midpoint. Search the right half. */
				start = mid + 1;
			} else if (comparison < 0) {
				/* This key is less than the value at the midpoint. Search the left half. */
				end = mid;
			} else {
				teds_strictsortedvectorset_search_result result;
				result.found = true;
				result.entry = e;
				return result;
			}
			if (start >= end) {
				break;
			}
			mid = start + (end - start)/2;
		}
	}
	/* The entry is the position in the array at which the new value should be inserted. */
	teds_strictsortedvectorset_search_result result;
	result.found = false;
	result.entry = &entries[start];
	return result;
}

static bool teds_strictsortedvectorset_entries_remove_key(teds_strictsortedvectorset_entries *array, zval *key)
{
	if (array->size == 0) {
		return false;
	}
	teds_strictsortedvectorset_search_result lookup = teds_strictsortedvectorset_entries_sorted_search_for_key(array, key);
	if (!lookup.found) {
		return false;
	}
	zval *const entries = array->entries;
	teds_vector_maybe_adjust_iterators_before_remove(array, lookup.entry - entries);

	zval *entry = lookup.entry;
	zval old_key;
	ZVAL_COPY_VALUE(&old_key, entry);
	zval *end = entries + array->size - 1;
	ZEND_ASSERT(entry <= end);
	memmove(entry, entry + 1, (end - entry) * sizeof(zval));
	array->size--;
	TEDS_SET_SHOULD_REBUILD_PROPERTIES(array, true);

	zval_ptr_dtor(&old_key);
	return true;
}

PHP_METHOD(Teds_StrictSortedVectorSet, remove)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(teds_strictsortedvectorset_entries_remove_key(Z_STRICTSORTEDVECTORSET_ENTRIES_P(ZEND_THIS), value));
}

PHP_METHOD(Teds_StrictSortedVectorSet, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(teds_strictsortedvectorset_entries_insert(Z_STRICTSORTEDVECTORSET_ENTRIES_P(ZEND_THIS), value, false));
}

PHP_METHOD(Teds_StrictSortedVectorSet, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictsortedvectorset_search_result result = teds_strictsortedvectorset_entries_sorted_search_for_key(Z_STRICTSORTEDVECTORSET_ENTRIES_P(ZEND_THIS), value);
	RETURN_BOOL(result.found);
}

PHP_MINIT_FUNCTION(teds_strictsortedvectorset)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_StrictSortedVectorSet = register_class_Teds_StrictSortedVectorSet(zend_ce_aggregate, teds_ce_Set, php_json_serializable_ce);
	teds_ce_StrictSortedVectorSet->create_object = teds_strictsortedvectorset_new;

	memcpy(&teds_handler_StrictSortedVectorSet, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictSortedVectorSet.offset          = XtOffsetOf(teds_strictsortedvectorset, std);
	teds_handler_StrictSortedVectorSet.clone_obj       = teds_strictsortedvectorset_clone;
	teds_handler_StrictSortedVectorSet.count_elements  = teds_strictsortedvectorset_count_elements;
	teds_handler_StrictSortedVectorSet.get_properties_for  = teds_strictsortedvectorset_get_properties_for;
	teds_handler_StrictSortedVectorSet.get_gc          = teds_strictsortedvectorset_get_gc;
	teds_handler_StrictSortedVectorSet.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictSortedVectorSet.free_obj        = teds_strictsortedvectorset_free_storage;

	teds_ce_StrictSortedVectorSet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StrictSortedVectorSet->get_iterator = teds_strictsortedvectorset_get_iterator;

	return SUCCESS;
}
