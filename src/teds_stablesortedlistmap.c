/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on teds_immutableiterable.c.
 * Instead of a C array of zvals, this is based on a C array of pairs of zvals for key-value entries */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_stablesortedlistmap_arginfo.h"
#include "teds_stablesortedlistmap.h"
#include "teds_util.h"
#include "teds_interfaces.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StableSortedListMap;
zend_class_entry *teds_ce_StableSortedListMap;

typedef struct _teds_stablesortedlistmap_entry {
	zval key;
	zval value;
} teds_stablesortedlistmap_entry;

/* This is a placeholder value to distinguish between empty and uninitialized StableSortedListMap instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const teds_stablesortedlistmap_entry empty_entry_list[1];

typedef struct _teds_stablesortedlistmap_entries {
	uint32_t size;
	uint32_t capacity;
	teds_stablesortedlistmap_entry *entries;
} teds_stablesortedlistmap_entries;

typedef struct _teds_stablesortedlistmap {
	teds_stablesortedlistmap_entries		array;
	zend_object				std;
} teds_stablesortedlistmap;

static zend_always_inline bool teds_stablesortedlistmap_entries_insert(teds_stablesortedlistmap_entries *array, zval *key, zval *value, bool probably_largest);
static void teds_stablesortedlistmap_clear(teds_stablesortedlistmap *intern);

/* Used by InternalIterator returned by StableSortedListMap->getIterator() */
typedef struct _teds_stablesortedlistmap_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_stablesortedlistmap_it;

static teds_stablesortedlistmap *teds_stablesortedlistmap_from_obj(zend_object *obj)
{
	return (teds_stablesortedlistmap*)((char*)(obj) - XtOffsetOf(teds_stablesortedlistmap, std));
}

#define Z_STABLESORTEDLISTMAP_P(zv)  teds_stablesortedlistmap_from_obj(Z_OBJ_P((zv)))
#define Z_STABLESORTEDLISTMAP_ENTRIES_P(zv)  &(teds_stablesortedlistmap_from_obj(Z_OBJ_P((zv)))->array)

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_stablesortedlistmap_entries_empty_capacity(teds_stablesortedlistmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list && array->entries != NULL);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_stablesortedlistmap_entries_empty_size(teds_stablesortedlistmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list && array->entries != NULL);
		return false;
	}
	return true;
}

static zend_always_inline bool teds_stablesortedlistmap_entries_uninitialized(teds_stablesortedlistmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->entries == NULL) {
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static teds_stablesortedlistmap_entry *teds_stablesortedlistmap_allocate_entries(size_t capacity) {
	if (UNEXPECTED(capacity >= TEDS_MAX_ZVAL_PAIR_COUNT)) {
		zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\StableSortedListMap capacity");
	}
	return safe_emalloc(capacity, sizeof(teds_stablesortedlistmap_entry), 0);
}

/* Helper function for qsort */
static int teds_stablesortedlistmap_entry_compare(const void *a, const void *b) {
	return teds_stable_compare(
		&((teds_stablesortedlistmap_entry *)a)->key,
		&((teds_stablesortedlistmap_entry *)b)->key
	);
}

static void teds_stablesortedlistmap_entries_init_from_array(teds_stablesortedlistmap_entries *array, zend_array *values)
{
	const zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		teds_stablesortedlistmap_entry *entries;
		int i = 0;
		zend_long capacity = teds_stablesortedlistmap_next_pow2_capacity(size);

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = entries = teds_stablesortedlistmap_allocate_entries(capacity);
		array->capacity = size;
		array->size = size;
		ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
			ZEND_ASSERT(i < size);
			teds_stablesortedlistmap_entry *entry = &entries[i];
			if (skey) {
				ZVAL_STR_COPY(&entry->key, skey);
			} else {
				ZVAL_LONG(&entry->key, nkey);
			}
			ZVAL_COPY_DEREF(&entry->value, val);
			i++;
		} ZEND_HASH_FOREACH_END();
		qsort(entries, size, sizeof(teds_stablesortedlistmap_entry), teds_stablesortedlistmap_entry_compare);
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (teds_stablesortedlistmap_entry *)empty_entry_list;
	}
}

static void teds_stablesortedlistmap_entries_init_from_traversable(teds_stablesortedlistmap_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	array->size = 0;
	array->capacity = 0;
	array->entries = NULL;
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

		teds_stablesortedlistmap_entries_insert(array, &key, value, false);
		/* existing key was updated */
		zval_ptr_dtor(&key);

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}

	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static void teds_stablesortedlistmap_entries_raise_capacity(teds_stablesortedlistmap_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(new_capacity > array->capacity);
	if (UNEXPECTED(new_capacity >= TEDS_MAX_ZVAL_PAIR_COUNT)) {
		zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\StableSortedListMap capacity");
	}
	if (teds_stablesortedlistmap_entries_empty_capacity(array)) {
		array->entries = safe_emalloc(new_capacity, sizeof(teds_stablesortedlistmap_entry), 0);
	} else {
		array->entries = safe_erealloc(array->entries, new_capacity, sizeof(teds_stablesortedlistmap_entry), 0);
	}
	array->capacity = new_capacity;
}

/* Copies the range [begin, end) into the stablesortedlistmap, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_stablesortedlistmap_copy_range(teds_stablesortedlistmap_entries *array, uint32_t offset, teds_stablesortedlistmap_entry *begin, teds_stablesortedlistmap_entry *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (uint32_t)(end - begin));

	teds_stablesortedlistmap_entry *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		ZVAL_COPY(&to->value, &begin->value);
		begin++;
		to++;
	}
}

static void teds_stablesortedlistmap_entries_copy_ctor(teds_stablesortedlistmap_entries *to, teds_stablesortedlistmap_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (teds_stablesortedlistmap_entry *)empty_entry_list;
		return;
	}

	const uint32_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(teds_stablesortedlistmap_entry), 0);
	to->size = size;
	to->capacity = capacity;

	teds_stablesortedlistmap_entry *begin = from->entries, *end = from->entries + size;
	teds_stablesortedlistmap_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_stablesortedlistmap_entries_dtor_range(teds_stablesortedlistmap_entry *start, uint32_t from, uint32_t to)
{
	teds_stablesortedlistmap_entry *begin = start + from, *end = start + to;
	while (begin < end) {
		zval_ptr_dtor(&begin->key);
		zval_ptr_dtor(&begin->value);
		begin++;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_stablesortedlistmap_entries_dtor(teds_stablesortedlistmap_entries *array)
{
	if (!teds_stablesortedlistmap_entries_empty_capacity(array)) {
		teds_stablesortedlistmap_entries_dtor_range(array->entries, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_stablesortedlistmap_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_stablesortedlistmap *intern = teds_stablesortedlistmap_from_obj(obj);

	*table = &intern->array.entries[0].key;
	*n = (int)intern->array.size * 2;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_stablesortedlistmap_get_properties(zend_object *obj)
{
	teds_stablesortedlistmap *intern = teds_stablesortedlistmap_from_obj(obj);
	uint32_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	uint32_t old_length = zend_hash_num_elements(ht);
	teds_stablesortedlistmap_entry *entries = intern->array.entries;
	/* Initialize properties array */
	for (uint32_t i = 0; i < len; i++) {
		zval tmp;
		Z_TRY_ADDREF_P(&entries[i].key);
		Z_TRY_ADDREF_P(&entries[i].value);
		ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
		zend_hash_index_update(ht, i, &tmp);
	}
	for (uint32_t i = len; i < old_length; i++) {
		zend_hash_index_del(ht, i);
	}

	return ht;
}

static void teds_stablesortedlistmap_free_storage(zend_object *object)
{
	teds_stablesortedlistmap *intern = teds_stablesortedlistmap_from_obj(object);
	teds_stablesortedlistmap_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_stablesortedlistmap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_stablesortedlistmap *intern;

	intern = zend_object_alloc(sizeof(teds_stablesortedlistmap), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StableSortedListMap);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_StableSortedListMap;

	if (orig && clone_orig) {
		teds_stablesortedlistmap *other = teds_stablesortedlistmap_from_obj(orig);
		teds_stablesortedlistmap_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_stablesortedlistmap_new(zend_class_entry *class_type)
{
	return teds_stablesortedlistmap_new_ex(class_type, NULL, 0);
}


static zend_object *teds_stablesortedlistmap_clone(zend_object *old_object)
{
	zend_object *new_object = teds_stablesortedlistmap_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_stablesortedlistmap_count_elements(zend_object *object, zend_long *count)
{
	teds_stablesortedlistmap *intern;

	intern = teds_stablesortedlistmap_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this StableSortedListMap */
PHP_METHOD(Teds_StableSortedListMap, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this StableSortedListMap is empty */
PHP_METHOD(Teds_StableSortedListMap, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_StableSortedListMap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_stablesortedlistmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StableSortedListMap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (teds_stablesortedlistmap_entry *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_stablesortedlistmap_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_stablesortedlistmap_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StableSortedListMap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_stablesortedlistmap_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_stablesortedlistmap_it_rewind(zend_object_iterator *iter)
{
	((teds_stablesortedlistmap_it*)iter)->current = 0;
}

static int teds_stablesortedlistmap_it_valid(zend_object_iterator *iter)
{
	teds_stablesortedlistmap_it     *iterator = (teds_stablesortedlistmap_it*)iter;
	teds_stablesortedlistmap *object   = Z_STABLESORTEDLISTMAP_P(&iter->data);

	if (iterator->current >= 0 && ((zend_ulong) iterator->current) < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static teds_stablesortedlistmap_entry *teds_stablesortedlistmap_it_read_offset_helper(teds_stablesortedlistmap *intern, size_t offset)
{
	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (UNEXPECTED(offset >= intern->array.size)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Iterator out of range", 0);
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

static zval *teds_stablesortedlistmap_it_get_current_data(zend_object_iterator *iter)
{
	teds_stablesortedlistmap_it     *iterator = (teds_stablesortedlistmap_it*)iter;
	teds_stablesortedlistmap *object   = Z_STABLESORTEDLISTMAP_P(&iter->data);

	teds_stablesortedlistmap_entry *data = teds_stablesortedlistmap_it_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_stablesortedlistmap_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_stablesortedlistmap_it     *iterator = (teds_stablesortedlistmap_it*)iter;
	teds_stablesortedlistmap *object   = Z_STABLESORTEDLISTMAP_P(&iter->data);

	teds_stablesortedlistmap_entry *data = teds_stablesortedlistmap_it_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_stablesortedlistmap_it_move_forward(zend_object_iterator *iter)
{
	((teds_stablesortedlistmap_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_stablesortedlistmap_it_funcs = {
	teds_stablesortedlistmap_it_dtor,
	teds_stablesortedlistmap_it_valid,
	teds_stablesortedlistmap_it_get_current_data,
	teds_stablesortedlistmap_it_get_current_key,
	teds_stablesortedlistmap_it_move_forward,
	teds_stablesortedlistmap_it_rewind,
	NULL,
	NULL, /* get_gc */
};

zend_object_iterator *teds_stablesortedlistmap_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	(void)ce;
	teds_stablesortedlistmap_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_stablesortedlistmap_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_stablesortedlistmap_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_StableSortedListMap, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	uint32_t raw_size = zend_hash_num_elements(raw_data);
	if (UNEXPECTED(raw_size % 2 != 0)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Odd number of elements", 0);
		RETURN_THROWS();
	}
	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	if (UNEXPECTED(!teds_stablesortedlistmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(intern->array.size == 0);
		ZEND_ASSERT(intern->array.capacity == 0);
		intern->array.entries = (teds_stablesortedlistmap_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	uint32_t i = 0;
	const uint32_t capacity = teds_stablesortedlistmap_next_pow2_capacity(raw_size / 2);
	teds_stablesortedlistmap_entries *array = &intern->array;
	teds_stablesortedlistmap_entry *entries = safe_emalloc(capacity, sizeof(teds_stablesortedlistmap_entry), 0);
	intern->array.size = 0;
	intern->array.capacity = capacity;
	intern->array.entries = entries;

	zend_string *str;
	zval key;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_stablesortedlistmap_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StableSortedListMap::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
		if (i % 2 == 1) {
			teds_stablesortedlistmap_entries_insert(array, &key, val, true);
		} else {
			ZVAL_COPY_VALUE(&key, val);
		}
		i++;
	} ZEND_HASH_FOREACH_END();

}

static bool teds_stablesortedlistmap_entries_insert_from_pair(teds_stablesortedlistmap_entries *array, zval *raw_val)
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
	ZVAL_DEREF(key);
	ZVAL_DEREF(value);
	teds_stablesortedlistmap_entries_insert(array, key, value, false);
	return true;
}

static void teds_stablesortedlistmap_entries_init_from_array_pairs(teds_stablesortedlistmap_entries *array, zend_array *raw_data)
{
	const uint32_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->entries = (teds_stablesortedlistmap_entry *)empty_entry_list;
		return;
	}
	const uint32_t capacity = teds_stablesortedlistmap_next_pow2_capacity(num_entries);
	array->entries = teds_stablesortedlistmap_allocate_entries(capacity);
	array->size = 0;
	array->capacity = capacity;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		if (!teds_stablesortedlistmap_entries_insert_from_pair(array, val)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();
}

static void teds_stablesortedlistmap_entries_init_from_traversable_pairs(teds_stablesortedlistmap_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	array->size = 0;
	array->capacity = 0;
	array->entries = NULL;
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

		if (!teds_stablesortedlistmap_entries_insert_from_pair(array, pair)) {
			break;
		}

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}

	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static zend_object* create_from_pairs(zval *iterable) {
	zend_object *object = teds_stablesortedlistmap_new(teds_ce_StableSortedListMap);
	teds_stablesortedlistmap *intern = teds_stablesortedlistmap_from_obj(object);
	teds_stablesortedlistmap_entries *array = &intern->array;
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_stablesortedlistmap_entries_init_from_array_pairs(array, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_stablesortedlistmap_entries_init_from_traversable_pairs(array, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_StableSortedListMap, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_StableSortedListMap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_stablesortedlistmap_new(teds_ce_StableSortedListMap);
	teds_stablesortedlistmap *intern = teds_stablesortedlistmap_from_obj(object);
	teds_stablesortedlistmap_entries_init_from_array_pairs(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_StableSortedListMap, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);

	if (teds_stablesortedlistmap_entries_empty_size(&intern->array)) {
		RETURN_EMPTY_ARRAY();
	}
	teds_stablesortedlistmap_entry *entries = intern->array.entries;
	const uint32_t len = intern->array.size;
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	for (uint32_t i = 0; i < len; i++) {
		zval *tmp = &entries[i].key;
		Z_TRY_ADDREF_P(tmp);
		zend_hash_next_index_insert(flat_entries_array, tmp);
		tmp = &entries[i].value;
		Z_TRY_ADDREF_P(tmp);
		zend_hash_next_index_insert(flat_entries_array, tmp);
	}
	/* Unlike FixedArray, there's no setSize, so there's no reason to delete indexes */

	RETURN_ARR(flat_entries_array);
}

#define IMPLEMENT_READ_ARRAY_PHP_METHOD(methodName, property)  \
PHP_METHOD(Teds_StableSortedListMap, methodName) \
{ \
	ZEND_PARSE_PARAMETERS_NONE(); \
	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS); \
	uint32_t len = intern->array.size; \
	if (!len) { \
		RETURN_EMPTY_ARRAY(); \
	} \
	teds_stablesortedlistmap_entry *entries = intern->array.entries; \
	zend_array *arr = zend_new_array(len); \
	/* Initialize return array */ \
	zend_hash_real_init_packed(arr); \
 \
	/* Go through arr and add propName to the return array */ \
	ZEND_HASH_FILL_PACKED(arr) { \
		for (uint32_t i = 0; i < len; i++) { \
			zval *tmp = &entries[i].property; \
			Z_TRY_ADDREF_P(tmp); \
			ZEND_HASH_FILL_ADD(tmp); \
		} \
	} ZEND_HASH_FILL_END(); \
	RETURN_ARR(arr); \
}

IMPLEMENT_READ_ARRAY_PHP_METHOD(keys, key)
IMPLEMENT_READ_ARRAY_PHP_METHOD(values, value)

#define IMPLEMENT_READ_OFFSET_PHP_METHOD(methodName, index, propName) \
PHP_METHOD(Teds_StableSortedListMap, methodName) \
{ \
	ZEND_PARSE_PARAMETERS_NONE(); \
	const teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS); \
	if (intern->array.size == 0) { \
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read " # methodName " of empty StableSortedListMap", 0); \
		RETURN_THROWS(); \
	} \
	teds_stablesortedlistmap_entry *entries = intern->array.entries; \
	RETVAL_COPY(&entries[(index)].propName); \
}

IMPLEMENT_READ_OFFSET_PHP_METHOD(bottom, 0, value)
IMPLEMENT_READ_OFFSET_PHP_METHOD(bottomKey, 0, key)
IMPLEMENT_READ_OFFSET_PHP_METHOD(top, intern->array.size - 1, value)
IMPLEMENT_READ_OFFSET_PHP_METHOD(topKey, intern->array.size - 1, key)

PHP_METHOD(Teds_StableSortedListMap, pop) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	if (intern->array.size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty StableSortedListMap", 0);
		RETURN_THROWS();
	}
	teds_stablesortedlistmap_entry *entry = &intern->array.entries[intern->array.size - 1];
	RETVAL_ARR(zend_new_pair(&entry->key, &entry->value));
	intern->array.size--;
}

/* Shifts values. Callers should adjust size and handle zval reference counting. */
static void teds_stablesortedlistmap_remove_entry(teds_stablesortedlistmap_entry *entries, uint32_t len, teds_stablesortedlistmap_entry *entry)
{
	teds_stablesortedlistmap_entry *end = entries + len - 1;
	ZEND_ASSERT(entry <= end);
	/* Move entries */
	for (; entry < end; ) {
		ZVAL_COPY_VALUE(&entry->key, &entry[1].key);
		ZVAL_COPY_VALUE(&entry->value, &entry[1].value);
		entry++;
	}
}

PHP_METHOD(Teds_StableSortedListMap, shift) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot shift from empty StableSortedListMap", 0);
		RETURN_THROWS();
	}
	teds_stablesortedlistmap_entry *entry = &intern->array.entries[0];
	RETVAL_ARR(zend_new_pair(&entry->key, &entry->value));
	teds_stablesortedlistmap_remove_entry(entry, len, entry);
	intern->array.size--;
}

typedef struct _teds_stablesortedlistmap_search_result {
	teds_stablesortedlistmap_entry *entry;
	bool found;
} teds_stablesortedlistmap_search_result;

static teds_stablesortedlistmap_search_result teds_stablesortedlistmap_entries_sorted_search_for_key(const teds_stablesortedlistmap_entries *array, zval *key)
{
	/* Currently, this is a binary search in an array, but later it would be a tree lookup. */
	teds_stablesortedlistmap_entry *const entries = array->entries;
	uint32_t start = 0;
	uint32_t end = array->size;
	while (start < end) {
		uint32_t mid = start + (end - start)/2;
		teds_stablesortedlistmap_entry *e = &entries[mid];
		int comparison = teds_stable_compare(key, &e->key);
		if (comparison > 0) {
			/* This key is greater than the value at the midpoint. Search the right half. */
			start = mid + 1;
		} else if (comparison < 0) {
			/* This key is less than the value at the midpoint. Search the left half. */
			end = mid;
		} else {
			teds_stablesortedlistmap_search_result result;
			result.found = true;
			result.entry = e;
			return result;
		}
	}
	/* The entry is the position in the array at which the new value should be inserted. */
	teds_stablesortedlistmap_search_result result;
	result.found = false;
	result.entry = &entries[start];
	return result;
}

static teds_stablesortedlistmap_search_result teds_stablesortedlistmap_entries_sorted_search_for_key_probably_largest(const teds_stablesortedlistmap_entries *array, zval *key)
{
	teds_stablesortedlistmap_entry *const entries = array->entries;
	uint32_t end = array->size;
	uint32_t start = 0;
	if (end > 0) {
		uint32_t mid = end - 1;
		/* This is written in a way that would be fastest for branch prediction if key is larger than the last value in the array. */
		while (true) {
			teds_stablesortedlistmap_entry *e = &entries[mid];
			int comparison = teds_stable_compare(key, &e->key);
			if (comparison > 0) {
				/* This key is greater than the value at the midpoint. Search the right half. */
				start = mid + 1;
			} else if (comparison < 0) {
				/* This key is less than the value at the midpoint. Search the left half. */
				end = mid;
			} else {
				teds_stablesortedlistmap_search_result result;
				result.found = true;
				result.entry = e;
				return result;
			}
			if (start >= end) {
				break;
			}
			mid = start + (end - mid) / 2;
		}
	}
	/* The entry is the position in the array at which the new value should be inserted. */
	teds_stablesortedlistmap_search_result result;
	result.found = false;
	result.entry = &entries[start];
	return result;
}

static teds_stablesortedlistmap_entry *teds_stablesortedlistmap_find_value(const teds_stablesortedlistmap *intern, zval *value)
{
	const uint32_t len = intern->array.size;
	teds_stablesortedlistmap_entry *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i].value)) {
			return &entries[i];
		}
	}
	return NULL;
}

static void teds_stablesortedlistmap_entries_remove_key(teds_stablesortedlistmap_entries *array, zval *key)
{
	const uint32_t len = array->size;
	if (len == 0) {
		return;
	}
	teds_stablesortedlistmap_search_result lookup = teds_stablesortedlistmap_entries_sorted_search_for_key(array, key);
	if (!lookup.found) {
		return;
	}
	teds_stablesortedlistmap_entry *entry = lookup.entry;
	zval old_key;
	zval old_value;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	ZVAL_COPY_VALUE(&old_value, &entry->value);
	teds_stablesortedlistmap_remove_entry(array->entries, len, entry);
	array->size--;

	zval_ptr_dtor(&old_key);
	zval_ptr_dtor(&old_value);
}

PHP_METHOD(Teds_StableSortedListMap, offsetExists)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	if (intern->array.size > 0) {
		teds_stablesortedlistmap_search_result result = teds_stablesortedlistmap_entries_sorted_search_for_key(&intern->array, key);
		if (result.found) {
			RETURN_BOOL(Z_TYPE(result.entry->value) != IS_NULL);
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_StableSortedListMap, offsetGet)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	if (intern->array.size > 0) {
		teds_stablesortedlistmap_search_result result = teds_stablesortedlistmap_entries_sorted_search_for_key(&intern->array, key);
		if (result.found) {
			RETURN_COPY(&result.entry->value);
		}
	}
	zend_throw_exception(spl_ce_OutOfBoundsException, "Key not found", 0);
	RETURN_THROWS();
}

PHP_METHOD(Teds_StableSortedListMap, get)
{
	zval *key;
	zval *default_zv = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_zv)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stablesortedlistmap_entries *array = Z_STABLESORTEDLISTMAP_ENTRIES_P(ZEND_THIS);
	if (array->size > 0) {
		teds_stablesortedlistmap_search_result result = teds_stablesortedlistmap_entries_sorted_search_for_key(array, key);
		if (result.found) {
			RETURN_COPY(&result.entry->value);
		}
	}
	if (default_zv != NULL) {
		RETURN_COPY(default_zv);
	}
	zend_throw_exception(spl_ce_OutOfBoundsException, "Key not found", 0);
	RETURN_THROWS();
}

static zend_always_inline bool teds_stablesortedlistmap_entries_insert(teds_stablesortedlistmap_entries *array, zval *key, zval *value, bool probably_largest) {
	teds_stablesortedlistmap_search_result result = probably_largest
		? teds_stablesortedlistmap_entries_sorted_search_for_key_probably_largest(array, key)
		: teds_stablesortedlistmap_entries_sorted_search_for_key(array, key);
	if (result.found) {
		/* Replace old value, then free old value */
		zval old;
		ZVAL_COPY_VALUE(&old, &result.entry->value);
		ZVAL_COPY(&result.entry->value, value);
		zval_ptr_dtor(&old);
		return false;
	}
	/* Reallocate and insert (insertion sort) */
	teds_stablesortedlistmap_entry *entry = result.entry;
	if (array->size >= array->capacity) {
		const uint32_t new_offset = result.entry - array->entries;
		ZEND_ASSERT(array->size == array->capacity);
		const uint32_t new_capacity = teds_stablesortedlistmap_next_pow2_capacity(array->size + 1);
		teds_stablesortedlistmap_entries_raise_capacity(array, new_capacity);
		entry = array->entries + new_offset;
	}

	for (teds_stablesortedlistmap_entry *it = array->entries + array->size; it > entry; it--) {
		ZVAL_COPY_VALUE(&it[0].key, &it[-1].key);
		ZVAL_COPY_VALUE(&it[0].value, &it[-1].value);
	}

	array->size++;
	ZVAL_COPY(&entry->key, key);
	ZVAL_COPY(&entry->value, value);
	return true;
}

PHP_METHOD(Teds_StableSortedListMap, offsetSet)
{
	zval *key;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_stablesortedlistmap_entries_insert(Z_STABLESORTEDLISTMAP_ENTRIES_P(ZEND_THIS), key, value, 0);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_StableSortedListMap, offsetUnset)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_stablesortedlistmap_entries_remove_key(Z_STABLESORTEDLISTMAP_ENTRIES_P(ZEND_THIS), key);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_StableSortedListMap, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	teds_stablesortedlistmap_entry *entry = teds_stablesortedlistmap_find_value(intern, value);
	RETURN_BOOL(entry != NULL);
}

PHP_METHOD(Teds_StableSortedListMap, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	if (len > 0) {
		teds_stablesortedlistmap_search_result result = teds_stablesortedlistmap_entries_sorted_search_for_key(&intern->array, key);
		RETURN_BOOL(result.found);
	}
	RETURN_FALSE;
}

static void teds_stablesortedlistmap_return_pairs(zval *return_value, teds_stablesortedlistmap *intern)
{
	const uint32_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	teds_stablesortedlistmap_entry *entries = intern->array.entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (uint32_t i = 0; i < len; i++) {
			zval tmp;
			Z_TRY_ADDREF_P(&entries[i].key);
			Z_TRY_ADDREF_P(&entries[i].value);
			ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
			ZEND_HASH_FILL_ADD(&tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_StableSortedListMap, toPairs)
{
	/* json_encoder.c will always encode objects as {"0":..., "1":...}, and detects recursion if an object returns its internal property array, so we have to return a new array */
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	teds_stablesortedlistmap_return_pairs(return_value, intern);
}

PHP_METHOD(Teds_StableSortedListMap, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stablesortedlistmap_entries *const array = Z_STABLESORTEDLISTMAP_ENTRIES_P(ZEND_THIS);
	if (!array->size) {
		RETURN_EMPTY_ARRAY();
	}

	zend_array *values = zend_new_array(array->size);
	for (uint32_t i = 0; i < array->size; i++) {
		teds_stablesortedlistmap_entry *entry = &array->entries[i];
		zval *key = &entry->key;
		zval *val = &entry->value;

		// Z_TRY_ADDREF_P(key);
		Z_TRY_ADDREF_P(val);
		array_set_zval_key(values, key, val);
		zval_ptr_dtor_nogc(val);
		if (UNEXPECTED(EG(exception))) {
			zend_array_destroy(values);
			RETURN_THROWS();
		}
	}
	RETURN_ARR(values);
}

static void teds_stablesortedlistmap_clear(teds_stablesortedlistmap *intern) {
	teds_stablesortedlistmap_entries *array = &intern->array;

	if (teds_stablesortedlistmap_entries_empty_capacity(array)) {
		return;
	}
	teds_stablesortedlistmap_entry *entries = intern->array.entries;
	const uint32_t size = intern->array.size;
	intern->array.entries = (teds_stablesortedlistmap_entry *)empty_entry_list;
	intern->array.size = 0;
	intern->array.capacity = 0;

	teds_stablesortedlistmap_entries_dtor_range(entries, 0, size);
	efree(entries);
	/* Could call teds_stablesortedlistmap_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StableSortedListMap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stablesortedlistmap *intern = Z_STABLESORTEDLISTMAP_P(ZEND_THIS);
	teds_stablesortedlistmap_clear(intern);
	TEDS_RETURN_VOID();
}

PHP_MINIT_FUNCTION(teds_stablesortedlistmap)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_StableSortedListMap = register_class_Teds_StableSortedListMap(zend_ce_aggregate, teds_ce_Map, php_json_serializable_ce);
	teds_ce_StableSortedListMap->create_object = teds_stablesortedlistmap_new;

	memcpy(&teds_handler_StableSortedListMap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StableSortedListMap.offset          = XtOffsetOf(teds_stablesortedlistmap, std);
	teds_handler_StableSortedListMap.clone_obj       = teds_stablesortedlistmap_clone;
	teds_handler_StableSortedListMap.count_elements  = teds_stablesortedlistmap_count_elements;
	teds_handler_StableSortedListMap.get_properties  = teds_stablesortedlistmap_get_properties;
	teds_handler_StableSortedListMap.get_gc          = teds_stablesortedlistmap_get_gc;
	teds_handler_StableSortedListMap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StableSortedListMap.free_obj        = teds_stablesortedlistmap_free_storage;

	teds_ce_StableSortedListMap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StableSortedListMap->get_iterator = teds_stablesortedlistmap_get_iterator;

	return SUCCESS;
}
