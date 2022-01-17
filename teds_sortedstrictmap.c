/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on teds_immutablekeyvaluesequence.c.
 * Instead of a C array of zvals, this is based on a C array of pairs of zvals for key-value entries */
/*
 * Design plan for refactoring:
 * - Buckets: Keys (zval, uint64_t hash) and values (zval) placed based on hash. Similar to HashTable Data Layout as described in Zend_types
 * - Hashes: hash and index, chaining.
 *
 * iteration:
 * - sequence of HashPosition
 * - TODO: associate SortedStrictMap with linked list of iterators
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_sortedstrictmap_arginfo.h"
#include "teds_sortedstrictmap.h"
#include "teds_util.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_SortedStrictMap;
zend_class_entry *teds_ce_SortedStrictMap;

typedef struct _teds_sortedstrictmap_entry {
	zval key;   /* TODO: Stores Z_NEXT - similar to https://github.com/php/php-src/pull/6588 */
	zval value;
} teds_sortedstrictmap_entry;

/* This is a placeholder value to distinguish between empty and uninitialized SortedStrictMap instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const teds_sortedstrictmap_entry empty_entry_list[1];

typedef struct _teds_sortedstrictmap_entries {
	size_t size;
	size_t capacity;
	teds_sortedstrictmap_entry *entries;
} teds_sortedstrictmap_entries;

typedef struct _teds_sortedstrictmap {
	teds_sortedstrictmap_entries		array;
	zend_object				std;
} teds_sortedstrictmap;

static void teds_sortedstrictmap_insert(teds_sortedstrictmap *intern, zval *key, zval *value);
static void teds_sortedstrictmap_clear(teds_sortedstrictmap *intern);

/* Used by InternalIterator returned by SortedStrictMap->getIterator() */
typedef struct _teds_sortedstrictmap_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_sortedstrictmap_it;

static teds_sortedstrictmap *teds_sortedstrictmap_from_obj(zend_object *obj)
{
	return (teds_sortedstrictmap*)((char*)(obj) - XtOffsetOf(teds_sortedstrictmap, std));
}

#define Z_SORTEDSTRICTMAP_P(zv)  teds_sortedstrictmap_from_obj(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_sortedstrictmap_entries_empty_capacity(teds_sortedstrictmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static bool teds_sortedstrictmap_entries_empty_size(teds_sortedstrictmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	return true;
}

static bool teds_sortedstrictmap_entries_uninitialized(teds_sortedstrictmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->entries == NULL) {
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

/* Initializes the range [from, to) to null. Does not dtor existing entries. */
/* TODO: Delete if this isn't used in the final version
static void teds_sortedstrictmap_entries_init_elems(teds_sortedstrictmap_entries *array, zend_long from, zend_long to)
{
	ZEND_ASSERT(from <= to);
	zval *begin = &array->entries[from].key;
	zval *end = &array->entries[to].key;

	while (begin != end) {
		ZVAL_NULL(begin++);
	}
}
*/

static teds_sortedstrictmap_entry *teds_sortedstrictmap_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(teds_sortedstrictmap_entry), 0);
}

static void teds_sortedstrictmap_entries_init_from_array(teds_sortedstrictmap_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		teds_sortedstrictmap_entry *entries;
		int i = 0;
		zend_long capacity = teds_sortedstrictmap_next_pow2_capacity(size);

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = entries = teds_sortedstrictmap_allocate_entries(capacity);
		array->capacity = size;
		array->size = size;
		ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
			ZEND_ASSERT(i < size);
			teds_sortedstrictmap_entry *entry = &entries[i];
			if (skey) {
				ZVAL_STR_COPY(&entry->key, skey);
			} else {
				ZVAL_LONG(&entry->key, nkey);
			}
			ZVAL_COPY_DEREF(&entry->value, val);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (teds_sortedstrictmap_entry *)empty_entry_list;
	}
}

static void teds_sortedstrictmap_entries_init_from_traversable(teds_sortedstrictmap_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->capacity = 0;
	array->entries = NULL;
	teds_sortedstrictmap_entry *entries = NULL;
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
				entries = safe_erealloc(entries, capacity, sizeof(teds_sortedstrictmap_entry), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(teds_sortedstrictmap_entry), 0);
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

	array->size = size;
	array->capacity = capacity;
	array->entries = entries;
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static void teds_sortedstrictmap_entries_raise_capacity(teds_sortedstrictmap_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(new_capacity > array->capacity);
	if (teds_sortedstrictmap_entries_empty_capacity(array)) {
		array->entries = safe_emalloc(new_capacity, sizeof(teds_sortedstrictmap_entry), 0);
	} else {
		array->entries = safe_erealloc(array->entries, new_capacity, sizeof(teds_sortedstrictmap_entry), 0);
	}
	array->capacity = new_capacity;
}

/* Copies the range [begin, end) into the sortedstrictmap, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_sortedstrictmap_copy_range(teds_sortedstrictmap_entries *array, size_t offset, teds_sortedstrictmap_entry *begin, teds_sortedstrictmap_entry *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	teds_sortedstrictmap_entry *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		ZVAL_COPY(&to->value, &begin->value);
		begin++;
		to++;
	}
}

static void teds_sortedstrictmap_entries_copy_ctor(teds_sortedstrictmap_entries *to, teds_sortedstrictmap_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (teds_sortedstrictmap_entry *)empty_entry_list;
		return;
	}

	const size_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(teds_sortedstrictmap_entry), 0);
	to->size = size;
	to->capacity = capacity;

	teds_sortedstrictmap_entry *begin = from->entries, *end = from->entries + size;
	teds_sortedstrictmap_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_sortedstrictmap_entries_dtor_range(teds_sortedstrictmap_entry *start, size_t from, size_t to)
{
	teds_sortedstrictmap_entry *begin = start + from, *end = start + to;
	while (begin < end) {
		zval_ptr_dtor(&begin->key);
		zval_ptr_dtor(&begin->value);
		begin++;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_sortedstrictmap_entries_dtor(teds_sortedstrictmap_entries *array)
{
	if (!teds_sortedstrictmap_entries_empty_capacity(array)) {
		teds_sortedstrictmap_entries_dtor_range(array->entries, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_sortedstrictmap_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(obj);

	*table = &intern->array.entries[0].key;
	*n = (int)intern->array.size * 2;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_sortedstrictmap_get_properties(zend_object *obj)
{
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(obj);
	size_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	size_t old_length = zend_hash_num_elements(ht);
	teds_sortedstrictmap_entry *entries = intern->array.entries;
	/* Initialize properties array */
	for (size_t i = 0; i < len; i++) {
		zval tmp;
		Z_TRY_ADDREF_P(&entries[i].key);
		Z_TRY_ADDREF_P(&entries[i].value);
		ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
		zend_hash_index_update(ht, i, &tmp);
	}
	for (size_t i = len; i < old_length; i++) {
		zend_hash_index_del(ht, i);
	}

	return ht;
}

static void teds_sortedstrictmap_free_storage(zend_object *object)
{
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(object);
	teds_sortedstrictmap_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_sortedstrictmap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_sortedstrictmap *intern;

	intern = zend_object_alloc(sizeof(teds_sortedstrictmap), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_SortedStrictMap);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_SortedStrictMap;

	if (orig && clone_orig) {
		teds_sortedstrictmap *other = teds_sortedstrictmap_from_obj(orig);
		teds_sortedstrictmap_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_sortedstrictmap_new(zend_class_entry *class_type)
{
	return teds_sortedstrictmap_new_ex(class_type, NULL, 0);
}


static zend_object *teds_sortedstrictmap_clone(zend_object *old_object)
{
	zend_object *new_object = teds_sortedstrictmap_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_sortedstrictmap_count_elements(zend_object *object, zend_long *count)
{
	teds_sortedstrictmap *intern;

	intern = teds_sortedstrictmap_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this SortedStrictMap */
PHP_METHOD(Teds_SortedStrictMap, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this SortedStrictMap is empty */
PHP_METHOD(Teds_SortedStrictMap, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_SortedStrictMap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_sortedstrictmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\SortedStrictMap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (teds_sortedstrictmap_entry *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_sortedstrictmap_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_sortedstrictmap_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_SortedStrictMap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_sortedstrictmap_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_sortedstrictmap_it_rewind(zend_object_iterator *iter)
{
	((teds_sortedstrictmap_it*)iter)->current = 0;
}

static int teds_sortedstrictmap_it_valid(zend_object_iterator *iter)
{
	teds_sortedstrictmap_it     *iterator = (teds_sortedstrictmap_it*)iter;
	teds_sortedstrictmap *object   = Z_SORTEDSTRICTMAP_P(&iter->data);

	if (iterator->current >= 0 && ((zend_ulong) iterator->current) < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static teds_sortedstrictmap_entry *teds_sortedstrictmap_read_offset_helper(teds_sortedstrictmap *intern, size_t offset)
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

static zval *teds_sortedstrictmap_it_get_current_data(zend_object_iterator *iter)
{
	teds_sortedstrictmap_it     *iterator = (teds_sortedstrictmap_it*)iter;
	teds_sortedstrictmap *object   = Z_SORTEDSTRICTMAP_P(&iter->data);

	teds_sortedstrictmap_entry *data = teds_sortedstrictmap_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_sortedstrictmap_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_sortedstrictmap_it     *iterator = (teds_sortedstrictmap_it*)iter;
	teds_sortedstrictmap *object   = Z_SORTEDSTRICTMAP_P(&iter->data);

	teds_sortedstrictmap_entry *data = teds_sortedstrictmap_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_sortedstrictmap_it_move_forward(zend_object_iterator *iter)
{
	((teds_sortedstrictmap_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_sortedstrictmap_it_funcs = {
	teds_sortedstrictmap_it_dtor,
	teds_sortedstrictmap_it_valid,
	teds_sortedstrictmap_it_get_current_data,
	teds_sortedstrictmap_it_get_current_key,
	teds_sortedstrictmap_it_move_forward,
	teds_sortedstrictmap_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_sortedstrictmap_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_sortedstrictmap_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_sortedstrictmap_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_sortedstrictmap_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_SortedStrictMap, __unserialize)
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
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (UNEXPECTED(!teds_sortedstrictmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(intern->array.size == 0);
		ZEND_ASSERT(intern->array.capacity == 0);
		intern->array.entries = (teds_sortedstrictmap_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	size_t i = 0;
	const size_t capacity = teds_sortedstrictmap_next_pow2_capacity(raw_size / 2);
	teds_sortedstrictmap_entry *entries = safe_emalloc(capacity, sizeof(teds_sortedstrictmap_entry), 0);
	intern->array.size = 0;
	intern->array.capacity = capacity;
	intern->array.entries = entries;

	zend_string *str;
	zval key;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_sortedstrictmap_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\SortedStrictMap::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
		if (i % 2 == 1) {
			teds_sortedstrictmap_insert(intern, &key, val);
		} else {
			ZVAL_COPY_VALUE(&key, val);
		}
		i++;
	} ZEND_HASH_FOREACH_END();

}

static bool teds_sortedstrictmap_insert_from_pair(teds_sortedstrictmap *intern, zval *raw_val)
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
	teds_sortedstrictmap_insert(intern, key, value);
	return true;
}

static void teds_sortedstrictmap_entries_init_from_array_pairs(teds_sortedstrictmap *intern, zend_array *raw_data)
{
	teds_sortedstrictmap_entries *array = &intern->array;
	size_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->entries = (teds_sortedstrictmap_entry *)empty_entry_list;
		return;
	}
	const size_t capacity = teds_sortedstrictmap_next_pow2_capacity(num_entries);
	array->entries = teds_sortedstrictmap_allocate_entries(capacity);
	array->size = 0;
	array->capacity = capacity;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		if (!teds_sortedstrictmap_insert_from_pair(intern, val)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();
}

static void teds_sortedstrictmap_entries_init_from_traversable_pairs(teds_sortedstrictmap *intern, zend_object *obj)
{
	teds_sortedstrictmap_entries *array = &intern->array;
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
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

		if (!teds_sortedstrictmap_insert_from_pair(intern, pair)) {
			break;
		}
		size++;

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
	zend_object *object = teds_sortedstrictmap_new(teds_ce_SortedStrictMap);
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(object);
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_sortedstrictmap_entries_init_from_array_pairs(intern, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_sortedstrictmap_entries_init_from_traversable_pairs(intern, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_SortedStrictMap, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_SortedStrictMap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_sortedstrictmap_new(teds_ce_SortedStrictMap);
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(object);
	teds_sortedstrictmap_entries_init_from_array_pairs(intern, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_SortedStrictMap, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);

	if (teds_sortedstrictmap_entries_empty_size(&intern->array)) {
		RETURN_EMPTY_ARRAY();
	}
	teds_sortedstrictmap_entry *entries = intern->array.entries;
	size_t len = intern->array.size;
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	for (size_t i = 0; i < intern->array.size; i++) {
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

PHP_METHOD(Teds_SortedStrictMap, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	teds_sortedstrictmap_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_SortedStrictMap, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	teds_sortedstrictmap_entry *entries = intern->array.entries;
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

typedef struct _teds_sortedstrictmap_search_result {
	teds_sortedstrictmap_entry *entry;
	bool found;
} teds_sortedstrictmap_search_result;

static teds_sortedstrictmap_search_result teds_sortedstrictmap_sorted_search_for_key(const teds_sortedstrictmap *intern, zval *key)
{
	/* Currently, this is a binary search in an array, but later it would be a tree lookup. */
	teds_sortedstrictmap_entry *const entries = intern->array.entries;
	size_t start = 0;
	size_t end = intern->array.size;
	while (start < end) {
		size_t mid = start + (end - start)/2;
		teds_sortedstrictmap_entry *e = &entries[mid];
		int comparison = teds_stable_compare(key, &e->key);
		if (comparison == 0) {
			teds_sortedstrictmap_search_result result;
			result.found = true;
			result.entry = e;
			return result;
		} else if (comparison < 0) {
			/* This key is less than the value at the midpoint. Search the left half. */
			end = mid;
		} else {
			/* This key is greater than the value at the midpoint. Search the right half. */
			start = mid + 1;
		}
	}
	/* The entry is the position in the array at which the new value should be inserted. */
	teds_sortedstrictmap_search_result result;
	result.found = false;
	result.entry = &entries[start];
	return result;
}

static teds_sortedstrictmap_entry *teds_sortedstrictmap_find_value(const teds_sortedstrictmap *intern, zval *value)
{
	const size_t len = intern->array.size;
	teds_sortedstrictmap_entry *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i].value)) {
			return &entries[i];
		}
	}
	return NULL;
}

static void teds_sortedstrictmap_remove_key(teds_sortedstrictmap *intern, zval *key)
{
	if (intern->array.size == 0) {
		return;
	}
	teds_sortedstrictmap_search_result lookup = teds_sortedstrictmap_sorted_search_for_key(intern, key);
	if (!lookup.found) {
		return;
	}
	teds_sortedstrictmap_entry *entry = lookup.entry;
	zval old_key;
	zval old_value;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	ZVAL_COPY_VALUE(&old_value, &entry->value);
	teds_sortedstrictmap_entry *end = intern->array.entries + intern->array.size - 1;
	ZEND_ASSERT(entry <= end);
	for (; entry < end; ) {
		teds_sortedstrictmap_entry *next = &entry[1];
		ZVAL_COPY_VALUE(&entry->key, &next->key);
		ZVAL_COPY_VALUE(&entry->value, &next->value);
		entry = next;
	}
	intern->array.size--;

	// TODO move entries and hashes
	zval_ptr_dtor(&old_key);
	zval_ptr_dtor(&old_value);
}
PHP_METHOD(Teds_SortedStrictMap, offsetExists)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (intern->array.size > 0) {
		teds_sortedstrictmap_search_result result = teds_sortedstrictmap_sorted_search_for_key(intern, key);
		if (result.found) {
			RETURN_BOOL(Z_TYPE(result.entry->value) != IS_NULL);
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_SortedStrictMap, offsetGet)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (intern->array.size > 0) {
		teds_sortedstrictmap_search_result result = teds_sortedstrictmap_sorted_search_for_key(intern, key);
		if (result.found) {
			RETURN_COPY_VALUE(&result.entry->value);
		}
	}
	RETURN_NULL();
}

static void teds_sortedstrictmap_insert(teds_sortedstrictmap *intern, zval *key, zval *value) {
	teds_sortedstrictmap_search_result result = teds_sortedstrictmap_sorted_search_for_key(intern, key);
	if (result.found) {
		/* Replace old value, then free old value */
		zval old;
		ZVAL_COPY_VALUE(&old, &result.entry->value);
		ZVAL_COPY(&result.entry->value, value);
		zval_ptr_dtor(&old);
		return;
	}
	/* Reallocate and insert (insertion sort) */
	teds_sortedstrictmap_entries *array = &intern->array;
	teds_sortedstrictmap_entry *entry = result.entry;
	if (array->size >= array->capacity) {
		const size_t new_offset = result.entry - array->entries;
		ZEND_ASSERT(array->size == array->capacity);
		const size_t new_capacity = teds_sortedstrictmap_next_pow2_capacity(array->size + 1);
		teds_sortedstrictmap_entries_raise_capacity(&intern->array, new_capacity);
		entry = array->entries + new_offset;
	}

	for (teds_sortedstrictmap_entry *it = array->entries + array->size; it > entry; it--) {
		ZVAL_COPY_VALUE(&it[0].key, &it[-1].key);
		ZVAL_COPY_VALUE(&it[0].value, &it[-1].value);
	}

	array->size++;
	ZVAL_COPY(&entry->key, key);
	ZVAL_COPY(&entry->value, value);
}

PHP_METHOD(Teds_SortedStrictMap, offsetSet)
{
	zval *key;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_insert(intern, key, value);
}

PHP_METHOD(Teds_SortedStrictMap, offsetUnset)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_remove_key(intern, key);
}

PHP_METHOD(Teds_SortedStrictMap, containsValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_entry *entry = teds_sortedstrictmap_find_value(intern, value);
	RETURN_BOOL(entry != NULL);
}

PHP_METHOD(Teds_SortedStrictMap, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	const size_t len = intern->array.size;
	if (len > 0) {
		teds_sortedstrictmap_search_result result = teds_sortedstrictmap_sorted_search_for_key(intern, key);
		RETURN_BOOL(result.found);
	}
	RETURN_FALSE;
}

static void teds_sortedstrictmap_return_pairs(zval *return_value, teds_sortedstrictmap *intern)
{
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	teds_sortedstrictmap_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_SortedStrictMap, jsonSerialize)
{
	/* json_encoder.c will always encode objects as {"0":..., "1":...}, and detects recursion if an object returns its internal property array, so we have to return a new array */
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_return_pairs(return_value, intern);
}

PHP_METHOD(Teds_SortedStrictMap, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_return_pairs(return_value, intern);
}

static void teds_sortedstrictmap_clear(teds_sortedstrictmap *intern) {
	teds_sortedstrictmap_entries *array = &intern->array;

	if (teds_sortedstrictmap_entries_empty_capacity(array)) {
		return;
	}
	teds_sortedstrictmap_entry *entries = intern->array.entries;
	size_t size = intern->array.size;
	intern->array.entries = (teds_sortedstrictmap_entry *)empty_entry_list;
	intern->array.size = 0;
	intern->array.capacity = 0;

	teds_sortedstrictmap_entries_dtor_range(entries, 0, size);
	efree(entries);
	/* Could call teds_sortedstrictmap_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_SortedStrictMap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_clear(intern);
}

PHP_MINIT_FUNCTION(teds_sortedstrictmap)
{
	teds_ce_SortedStrictMap = register_class_Teds_SortedStrictMap(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
	teds_ce_SortedStrictMap->create_object = teds_sortedstrictmap_new;

	memcpy(&teds_handler_SortedStrictMap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_SortedStrictMap.offset          = XtOffsetOf(teds_sortedstrictmap, std);
	teds_handler_SortedStrictMap.clone_obj       = teds_sortedstrictmap_clone;
	teds_handler_SortedStrictMap.count_elements  = teds_sortedstrictmap_count_elements;
	teds_handler_SortedStrictMap.get_properties  = teds_sortedstrictmap_get_properties;
	teds_handler_SortedStrictMap.get_gc          = teds_sortedstrictmap_get_gc;
	teds_handler_SortedStrictMap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_SortedStrictMap.free_obj        = teds_sortedstrictmap_free_storage;

	teds_ce_SortedStrictMap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_SortedStrictMap->get_iterator = teds_sortedstrictmap_get_iterator;

	return SUCCESS;
}
