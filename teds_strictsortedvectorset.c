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
 * - TODO: associate StrictSortedVectorSet with linked list of iterators
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
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StrictSortedVectorSet;
zend_class_entry *teds_ce_StrictSortedVectorSet;

typedef struct _teds_strictsortedvectorset_entry {
	zval key;
} teds_strictsortedvectorset_entry;

/* This is a placeholder value to distinguish between empty and uninitialized StrictSortedVectorSet instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const teds_strictsortedvectorset_entry empty_entry_list[1];

typedef struct _teds_strictsortedvectorset_entries {
	size_t size;
	size_t capacity;
	teds_strictsortedvectorset_entry *entries;
} teds_strictsortedvectorset_entries;

typedef struct _teds_strictsortedvectorset {
	teds_strictsortedvectorset_entries		array;
	zend_object				std;
} teds_strictsortedvectorset;

typedef struct _teds_strictsortedvectorset_search_result {
	teds_strictsortedvectorset_entry *entry;
	bool found;
} teds_strictsortedvectorset_search_result;

static void teds_strictsortedvectorset_entries_raise_capacity(teds_strictsortedvectorset_entries *array, size_t new_capacity);
static teds_strictsortedvectorset_search_result teds_strictsortedvectorset_entries_sorted_search_for_key(const teds_strictsortedvectorset_entries *array, zval *key);
static teds_strictsortedvectorset_search_result teds_strictsortedvectorset_entries_sorted_search_for_key_probably_largest(const teds_strictsortedvectorset_entries *array, zval *key);

static bool teds_strictsortedvectorset_entries_insert(teds_strictsortedvectorset_entries *array, zval *key, bool probably_largest) {
	teds_strictsortedvectorset_search_result result = probably_largest
		? teds_strictsortedvectorset_entries_sorted_search_for_key_probably_largest(array, key)
		: teds_strictsortedvectorset_entries_sorted_search_for_key(array, key);
	if (result.found) {
		return false;
	}
	/* Reallocate and insert (insertion sort) */
	teds_strictsortedvectorset_entry *entry = result.entry;
	if (array->size >= array->capacity) {
		const size_t new_offset = result.entry - array->entries;
		ZEND_ASSERT(array->size == array->capacity);
		const size_t new_capacity = teds_strictsortedvectorset_next_pow2_capacity(array->size + 1);
		teds_strictsortedvectorset_entries_raise_capacity(array, new_capacity);
		entry = array->entries + new_offset;
	}

	for (teds_strictsortedvectorset_entry *it = array->entries + array->size; it > entry; it--) {
		ZVAL_COPY_VALUE(&it[0].key, &it[-1].key);
	}

	ZVAL_COPY(&entry->key, key);
	array->size++;
	return true;
}

static void teds_strictsortedvectorset_entries_clear(teds_strictsortedvectorset_entries *array);

/* Used by InternalIterator returned by StrictSortedVectorSet->getIterator() */
typedef struct _teds_strictsortedvectorset_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_strictsortedvectorset_it;

static teds_strictsortedvectorset *teds_strictsortedvectorset_from_obj(zend_object *obj)
{
	return (teds_strictsortedvectorset*)((char*)(obj) - XtOffsetOf(teds_strictsortedvectorset, std));
}

#define Z_STRICTSORTEDVECTORSET_P(zv)  teds_strictsortedvectorset_from_obj(Z_OBJ_P((zv)))
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

static teds_strictsortedvectorset_entry *teds_strictsortedvectorset_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(teds_strictsortedvectorset_entry), 0);
}

static void teds_strictsortedvectorset_entries_init_from_array(teds_strictsortedvectorset_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zval *val;
		teds_strictsortedvectorset_entry *entries;
		zend_long capacity = teds_strictsortedvectorset_next_pow2_capacity(size);

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = entries = teds_strictsortedvectorset_allocate_entries(capacity);
		array->capacity = size;
		ZEND_HASH_FOREACH_VAL(values, val) {
			ZVAL_DEREF(val);
			teds_strictsortedvectorset_entries_insert(array, val, false);
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (teds_strictsortedvectorset_entry *)empty_entry_list;
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
	teds_strictsortedvectorset_entry *entries = NULL;
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
				entries = safe_erealloc(entries, capacity, sizeof(teds_strictsortedvectorset_entry), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(teds_strictsortedvectorset_entry), 0);
			}
		}
		ZVAL_COPY_DEREF(&entries[size].key, value);
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
cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static void teds_strictsortedvectorset_entries_raise_capacity(teds_strictsortedvectorset_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(new_capacity > array->capacity);
	if (teds_strictsortedvectorset_entries_empty_capacity(array)) {
		array->entries = safe_emalloc(new_capacity, sizeof(teds_strictsortedvectorset_entry), 0);
	} else {
		array->entries = safe_erealloc(array->entries, new_capacity, sizeof(teds_strictsortedvectorset_entry), 0);
	}
	array->capacity = new_capacity;
}

/* Copies the range [begin, end) into the strictsortedvectorset, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_strictsortedvectorset_copy_range(teds_strictsortedvectorset_entries *array, size_t offset, teds_strictsortedvectorset_entry *begin, teds_strictsortedvectorset_entry *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	teds_strictsortedvectorset_entry *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
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
		to->entries = (teds_strictsortedvectorset_entry *)empty_entry_list;
		return;
	}

	const size_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(teds_strictsortedvectorset_entry), 0);
	to->size = size;
	to->capacity = capacity;

	teds_strictsortedvectorset_entry *begin = from->entries, *end = from->entries + size;
	teds_strictsortedvectorset_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_strictsortedvectorset_entries_dtor_range(teds_strictsortedvectorset_entry *start, size_t from, size_t to)
{
	teds_strictsortedvectorset_entry *begin = start + from, *end = start + to;
	while (begin < end) {
		zval_ptr_dtor(&begin->key);
		begin++;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_strictsortedvectorset_entries_dtor(teds_strictsortedvectorset_entries *array)
{
	if (!teds_strictsortedvectorset_entries_empty_capacity(array)) {
		teds_strictsortedvectorset_entries_dtor_range(array->entries, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_strictsortedvectorset_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_strictsortedvectorset *intern = teds_strictsortedvectorset_from_obj(obj);

	*table = &intern->array.entries[0].key;
	*n = (int)intern->array.size;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_strictsortedvectorset_get_properties(zend_object *obj)
{
	teds_strictsortedvectorset *intern = teds_strictsortedvectorset_from_obj(obj);
	size_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	size_t old_length = zend_hash_num_elements(ht);
	teds_strictsortedvectorset_entry *entries = intern->array.entries;
	/* Initialize properties array */
	for (size_t i = 0; i < len; i++) {
		Z_TRY_ADDREF_P(&entries[i].key);
		zend_hash_index_update(ht, i, &entries[i].key);
	}
	for (size_t i = len; i < old_length; i++) {
		zend_hash_index_del(ht, i);
	}

	return ht;
}

static void teds_strictsortedvectorset_free_storage(zend_object *object)
{
	teds_strictsortedvectorset *intern = teds_strictsortedvectorset_from_obj(object);
	teds_strictsortedvectorset_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
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
		teds_strictsortedvectorset *other = teds_strictsortedvectorset_from_obj(orig);
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

static int teds_strictsortedvectorset_count_elements(zend_object *object, zend_long *count)
{
	teds_strictsortedvectorset *intern;

	intern = teds_strictsortedvectorset_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this StrictSortedVectorSet */
PHP_METHOD(Teds_StrictSortedVectorSet, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this StrictSortedVectorSet is empty */
PHP_METHOD(Teds_StrictSortedVectorSet, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(object);
	RETURN_BOOL(intern->array.size == 0);
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
		intern->array.entries = (teds_strictsortedvectorset_entry *)empty_entry_list;
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

static void teds_strictsortedvectorset_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_strictsortedvectorset_it_rewind(zend_object_iterator *iter)
{
	((teds_strictsortedvectorset_it*)iter)->current = 0;
}

static int teds_strictsortedvectorset_it_valid(zend_object_iterator *iter)
{
	teds_strictsortedvectorset_it     *iterator = (teds_strictsortedvectorset_it*)iter;
	teds_strictsortedvectorset *object   = Z_STRICTSORTEDVECTORSET_P(&iter->data);

	if (iterator->current >= 0 && ((zend_ulong) iterator->current) < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static teds_strictsortedvectorset_entry *teds_strictsortedvectorset_read_offset_helper(teds_strictsortedvectorset *intern, size_t offset)
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

static zval *teds_strictsortedvectorset_it_get_current_data(zend_object_iterator *iter)
{
	teds_strictsortedvectorset_it     *iterator = (teds_strictsortedvectorset_it*)iter;
	teds_strictsortedvectorset *object   = Z_STRICTSORTEDVECTORSET_P(&iter->data);

	teds_strictsortedvectorset_entry *data = teds_strictsortedvectorset_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->key;
	}
}

static void teds_strictsortedvectorset_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_strictsortedvectorset_it     *iterator = (teds_strictsortedvectorset_it*)iter;
	teds_strictsortedvectorset *object   = Z_STRICTSORTEDVECTORSET_P(&iter->data);

	teds_strictsortedvectorset_entry *data = teds_strictsortedvectorset_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_strictsortedvectorset_it_move_forward(zend_object_iterator *iter)
{
	((teds_strictsortedvectorset_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_strictsortedvectorset_it_funcs = {
	teds_strictsortedvectorset_it_dtor,
	teds_strictsortedvectorset_it_valid,
	teds_strictsortedvectorset_it_get_current_data,
	teds_strictsortedvectorset_it_get_current_key,
	teds_strictsortedvectorset_it_move_forward,
	teds_strictsortedvectorset_it_rewind,
	NULL,
	NULL, /* get_gc */
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

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_strictsortedvectorset_it_funcs;

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
	teds_strictsortedvectorset_entries *array = Z_STRICTSORTEDVECTORSET_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!teds_strictsortedvectorset_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		array->entries = (teds_strictsortedvectorset_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(array->entries == NULL);

	const size_t capacity = teds_strictsortedvectorset_next_pow2_capacity(raw_size);
	teds_strictsortedvectorset_entry *entries = safe_emalloc(capacity, sizeof(teds_strictsortedvectorset_entry), 0);
	array->size = 0;
	array->capacity = capacity;
	array->entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictsortedvectorset_entries_clear(array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictSortedVectorSet::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

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
	teds_strictsortedvectorset *intern = teds_strictsortedvectorset_from_obj(object);
	teds_strictsortedvectorset_entries_init_from_array(&intern->array, array_ht);

	RETURN_OBJ(object);
}

static zend_array *teds_strictsortedvectorset_create_array_copy(teds_strictsortedvectorset *intern) {
	teds_strictsortedvectorset_entry *entries = intern->array.entries;
	const size_t len = intern->array.size;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].key;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	return values;
}

PHP_METHOD(Teds_StrictSortedVectorSet, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		/* NOTE: This macro sets immutable gc flags, differently from RETURN_ARR */
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_strictsortedvectorset_create_array_copy(intern));
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
		teds_strictsortedvectorset_entry *entry = &array->entries[i];
		zval *key = &entry->key;

		// Z_TRY_ADDREF_P(key);
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

#define IMPLEMENT_READ_OFFSET_PHP_METHOD(methodName, index) \
PHP_METHOD(Teds_StrictSortedVectorSet, methodName) \
{ \
	ZEND_PARSE_PARAMETERS_NONE(); \
	const teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(ZEND_THIS); \
	if (intern->array.size == 0) { \
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read " # methodName " of empty StrictSortedVectorSet", 0); \
		RETURN_THROWS(); \
	} \
	teds_strictsortedvectorset_entry *entries = intern->array.entries; \
	RETVAL_COPY(&entries[(index)].key); \
}

IMPLEMENT_READ_OFFSET_PHP_METHOD(first, 0)
IMPLEMENT_READ_OFFSET_PHP_METHOD(last, intern->array.size - 1)

PHP_METHOD(Teds_StrictSortedVectorSet, pop) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(ZEND_THIS);
	if (intern->array.size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty StrictSortedVectorSet", 0);
		RETURN_THROWS();
	}
	teds_strictsortedvectorset_entry *entry = &intern->array.entries[intern->array.size - 1];
	RETVAL_COPY_VALUE(&entry->key);
	intern->array.size--;
}

/* Shifts values. Callers should adjust size and handle zval reference counting. */
static void teds_strictsortedvectorset_entries_remove_entry(teds_strictsortedvectorset_entry *entries, size_t len, teds_strictsortedvectorset_entry *entry)
{
	teds_strictsortedvectorset_entry *end = entries + len - 1;
	ZEND_ASSERT(entry <= end);
	/* Move entries */
	for (; entry < end; ) {
		ZVAL_COPY_VALUE(&entry->key, &entry[1].key);
		entry++;
	}
}

PHP_METHOD(Teds_StrictSortedVectorSet, shift) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictsortedvectorset *intern = Z_STRICTSORTEDVECTORSET_P(ZEND_THIS);
	const size_t len = intern->array.size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot shift from empty StrictSortedVectorSet", 0);
		RETURN_THROWS();
	}
	teds_strictsortedvectorset_entry *entry = &intern->array.entries[0];
	RETVAL_COPY_VALUE(&entry->key);
	teds_strictsortedvectorset_entries_remove_entry(entry, len, entry);
	intern->array.size--;
}

static teds_strictsortedvectorset_search_result teds_strictsortedvectorset_entries_sorted_search_for_key(const teds_strictsortedvectorset_entries *array, zval *key)
{
	/* Currently, this is a binary search in an array, but later it would be a tree lookup. */
	teds_strictsortedvectorset_entry *const entries = array->entries;
	size_t start = 0;
	size_t end = array->size;
	while (start < end) {
		size_t mid = start + (end - start)/2;
		teds_strictsortedvectorset_entry *e = &entries[mid];
		int comparison = teds_stable_compare(key, &e->key);
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
	teds_strictsortedvectorset_entry *const entries = array->entries;
	size_t end = array->size;
	size_t start = 0;
	if (end > 0) {
		size_t mid = end - 1;
		/* This is written in a way that would be fastest for branch prediction if key is larger than the last value in the array. */
		while (true) {
			teds_strictsortedvectorset_entry *e = &entries[mid];
			int comparison = teds_stable_compare(key, &e->key);
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
	teds_strictsortedvectorset_entry *entry = lookup.entry;
	zval old_key;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	teds_strictsortedvectorset_entry *end = array->entries + array->size - 1;
	ZEND_ASSERT(entry <= end);
	for (; entry < end; ) {
		teds_strictsortedvectorset_entry *next = &entry[1];
		ZVAL_COPY_VALUE(&entry->key, &next->key);
		entry = next;
	}
	array->size--;

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

static void teds_strictsortedvectorset_entries_clear(teds_strictsortedvectorset_entries *array) {
	if (teds_strictsortedvectorset_entries_empty_capacity(array)) {
		return;
	}
	teds_strictsortedvectorset_entry *entries = array->entries;
	size_t size = array->size;
	array->entries = (teds_strictsortedvectorset_entry *)empty_entry_list;
	array->size = 0;
	array->capacity = 0;

	teds_strictsortedvectorset_entries_dtor_range(entries, 0, size);
	efree(entries);
	/* Could call teds_strictsortedvectorset_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StrictSortedVectorSet, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictsortedvectorset_entries_clear(Z_STRICTSORTEDVECTORSET_ENTRIES_P(ZEND_THIS));
	TEDS_RETURN_VOID();
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
	teds_handler_StrictSortedVectorSet.get_properties  = teds_strictsortedvectorset_get_properties;
	teds_handler_StrictSortedVectorSet.get_gc          = teds_strictsortedvectorset_get_gc;
	teds_handler_StrictSortedVectorSet.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictSortedVectorSet.free_obj        = teds_strictsortedvectorset_free_storage;

	teds_ce_StrictSortedVectorSet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StrictSortedVectorSet->get_iterator = teds_strictsortedvectorset_get_iterator;

	return SUCCESS;
}
