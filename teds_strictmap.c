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
 * - TODO: associate StrictMap with linked list of iterators
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_strictmap_arginfo.h"
#include "teds_strictmap.h"
#include "teds_util.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StrictMap;
zend_class_entry *teds_ce_StrictMap;

#define TEDS_HASH(zval) ((zval).u2.extra)
#define TEDS_ENTRY_HASH(entry) ((entry)->value.u2.extra)

typedef struct _teds_strictmap_entry {
	zval key;   /* TODO: Stores Z_NEXT - similar to https://github.com/php/php-src/pull/6588 */
	zval value; /* Stores TEDS_HASH with hash of the key in extra. */
} teds_strictmap_entry;

/* This is a placeholder value to distinguish between empty and uninitialized StrictMap instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const teds_strictmap_entry empty_entry_list[1];

typedef struct _teds_strictmap_entries {
	size_t size;
	size_t capacity;
	teds_strictmap_entry *entries;
} teds_strictmap_entries;

typedef struct _teds_strictmap {
	teds_strictmap_entries		array;
	zend_object				std;
} teds_strictmap;

static void teds_strictmap_insert(teds_strictmap *intern, zval *key, zval *value);
static void teds_strictmap_clear(teds_strictmap *intern);

/* Used by InternalIterator returned by StrictMap->getIterator() */
typedef struct _teds_strictmap_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_strictmap_it;

static teds_strictmap *teds_strictmap_from_obj(zend_object *obj)
{
	return (teds_strictmap*)((char*)(obj) - XtOffsetOf(teds_strictmap, std));
}

#define Z_STRICTMAP_P(zv)  teds_strictmap_from_obj(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_strictmap_entries_empty_capacity(teds_strictmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static zend_always_inline void teds_strictmap_entry_compute_and_store_hash(teds_strictmap_entry *entry) {
	TEDS_ENTRY_HASH(entry) = teds_strict_hash(&entry->key);
}

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static bool teds_strictmap_entries_empty_size(teds_strictmap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	return true;
}

static bool teds_strictmap_entries_uninitialized(teds_strictmap_entries *array)
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
static void teds_strictmap_entries_init_elems(teds_strictmap_entries *array, zend_long from, zend_long to)
{
	ZEND_ASSERT(from <= to);
	zval *begin = &array->entries[from].key;
	zval *end = &array->entries[to].key;

	while (begin != end) {
		ZVAL_NULL(begin++);
	}
}
*/

static teds_strictmap_entry *teds_strictmap_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(teds_strictmap_entry), 0);
}

static void teds_strictmap_entries_init_from_array(teds_strictmap_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		teds_strictmap_entry *entries;
		int i = 0;
		zend_long capacity = teds_strictmap_next_pow2_capacity(size);

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = entries = teds_strictmap_allocate_entries(capacity);
		array->capacity = size;
		array->size = size;
		ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
			ZEND_ASSERT(i < size);
			teds_strictmap_entry *entry = &entries[i];
			if (skey) {
				ZVAL_STR_COPY(&entry->key, skey);
			} else {
				ZVAL_LONG(&entry->key, nkey);
			}
			ZVAL_COPY_DEREF(&entry->value, val);
			teds_strictmap_entry_compute_and_store_hash(entry);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (teds_strictmap_entry *)empty_entry_list;
	}
}

static void teds_strictmap_entries_init_from_traversable(teds_strictmap_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->capacity = 0;
	array->entries = NULL;
	teds_strictmap_entry *entries = NULL;
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
				entries = safe_erealloc(entries, capacity, sizeof(teds_strictmap_entry), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(teds_strictmap_entry), 0);
			}
		}
		/* The key's reference count was already increased */
		ZVAL_COPY_VALUE(&entries[size].key, &key);
		ZVAL_COPY_DEREF(&entries[size].value, value);
		teds_strictmap_entry_compute_and_store_hash(&entries[size]);
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

static void teds_strictmap_entries_raise_capacity(teds_strictmap_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(new_capacity > array->capacity);
	if (teds_strictmap_entries_empty_capacity(array)) {
		array->entries = safe_emalloc(new_capacity, sizeof(teds_strictmap_entry), 0);
	} else {
		array->entries = safe_erealloc(array->entries, new_capacity, sizeof(teds_strictmap_entry), 0);
	}
	array->capacity = new_capacity;
}

/* Copies the range [begin, end) into the strictmap, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_strictmap_copy_range(teds_strictmap_entries *array, size_t offset, teds_strictmap_entry *begin, teds_strictmap_entry *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	teds_strictmap_entry *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		ZVAL_COPY(&to->value, &begin->value);
		TEDS_ENTRY_HASH(to) = TEDS_ENTRY_HASH(begin);
		begin++;
		to++;
	}
}

static void teds_strictmap_entries_copy_ctor(teds_strictmap_entries *to, teds_strictmap_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (teds_strictmap_entry *)empty_entry_list;
		return;
	}

	const size_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(teds_strictmap_entry), 0);
	to->size = size;
	to->capacity = capacity;

	teds_strictmap_entry *begin = from->entries, *end = from->entries + size;
	teds_strictmap_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_strictmap_entries_dtor_range(teds_strictmap_entry *start, size_t from, size_t to)
{
	teds_strictmap_entry *begin = start + from, *end = start + to;
	while (begin < end) {
		zval_ptr_dtor(&begin->key);
		zval_ptr_dtor(&begin->value);
		begin++;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_strictmap_entries_dtor(teds_strictmap_entries *array)
{
	if (!teds_strictmap_entries_empty_capacity(array)) {
		teds_strictmap_entries_dtor_range(array->entries, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_strictmap_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_strictmap *intern = teds_strictmap_from_obj(obj);

	*table = &intern->array.entries[0].key;
	*n = (int)intern->array.size * 2;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_strictmap_get_properties(zend_object *obj)
{
	teds_strictmap *intern = teds_strictmap_from_obj(obj);
	size_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	size_t old_length = zend_hash_num_elements(ht);
	teds_strictmap_entry *entries = intern->array.entries;
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

static void teds_strictmap_free_storage(zend_object *object)
{
	teds_strictmap *intern = teds_strictmap_from_obj(object);
	teds_strictmap_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_strictmap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_strictmap *intern;

	intern = zend_object_alloc(sizeof(teds_strictmap), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StrictMap);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_StrictMap;

	if (orig && clone_orig) {
		teds_strictmap *other = teds_strictmap_from_obj(orig);
		teds_strictmap_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_strictmap_new(zend_class_entry *class_type)
{
	return teds_strictmap_new_ex(class_type, NULL, 0);
}


static zend_object *teds_strictmap_clone(zend_object *old_object)
{
	zend_object *new_object = teds_strictmap_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_strictmap_count_elements(zend_object *object, zend_long *count)
{
	teds_strictmap *intern;

	intern = teds_strictmap_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this StrictMap */
PHP_METHOD(Teds_StrictMap, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictmap *intern = Z_STRICTMAP_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this StrictMap is empty */
PHP_METHOD(Teds_StrictMap, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictmap *intern = Z_STRICTMAP_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_StrictMap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_strictmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictMap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (teds_strictmap_entry *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_strictmap_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_strictmap_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StrictMap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_strictmap_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_strictmap_it_rewind(zend_object_iterator *iter)
{
	((teds_strictmap_it*)iter)->current = 0;
}

static int teds_strictmap_it_valid(zend_object_iterator *iter)
{
	teds_strictmap_it     *iterator = (teds_strictmap_it*)iter;
	teds_strictmap *object   = Z_STRICTMAP_P(&iter->data);

	if (iterator->current >= 0 && ((zend_ulong) iterator->current) < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static teds_strictmap_entry *teds_strictmap_read_offset_helper(teds_strictmap *intern, size_t offset)
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

static zval *teds_strictmap_it_get_current_data(zend_object_iterator *iter)
{
	teds_strictmap_it     *iterator = (teds_strictmap_it*)iter;
	teds_strictmap *object   = Z_STRICTMAP_P(&iter->data);

	teds_strictmap_entry *data = teds_strictmap_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_strictmap_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_strictmap_it     *iterator = (teds_strictmap_it*)iter;
	teds_strictmap *object   = Z_STRICTMAP_P(&iter->data);

	teds_strictmap_entry *data = teds_strictmap_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_strictmap_it_move_forward(zend_object_iterator *iter)
{
	((teds_strictmap_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_strictmap_it_funcs = {
	teds_strictmap_it_dtor,
	teds_strictmap_it_valid,
	teds_strictmap_it_get_current_data,
	teds_strictmap_it_get_current_key,
	teds_strictmap_it_move_forward,
	teds_strictmap_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_strictmap_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_strictmap_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_strictmap_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_strictmap_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_StrictMap, __unserialize)
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
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	if (UNEXPECTED(!teds_strictmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(intern->array.size == 0);
		ZEND_ASSERT(intern->array.capacity == 0);
		intern->array.entries = (teds_strictmap_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	size_t i = 0;
	const size_t capacity = teds_strictmap_next_pow2_capacity(raw_size / 2);
	teds_strictmap_entry *entries = safe_emalloc(capacity, sizeof(teds_strictmap_entry), 0);
	intern->array.size = 0;
	intern->array.capacity = capacity;
	intern->array.entries = entries;

	zend_string *str;
	zval key;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictmap_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictMap::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
		if (i % 2 == 1) {
			teds_strictmap_insert(intern, &key, val);
		} else {
			ZVAL_COPY_VALUE(&key, val);
		}
		i++;
	} ZEND_HASH_FOREACH_END();

}

static bool teds_strictmap_insert_from_pair(teds_strictmap *intern, zval *raw_val)
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
	teds_strictmap_insert(intern, key, value);
	return true;
}

static void teds_strictmap_entries_init_from_array_pairs(teds_strictmap *intern, zend_array *raw_data)
{
	teds_strictmap_entries *array = &intern->array;
	size_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->entries = (teds_strictmap_entry *)empty_entry_list;
		return;
	}
	const size_t capacity = teds_strictmap_next_pow2_capacity(num_entries);
	teds_strictmap_entry *entries = teds_strictmap_allocate_entries(capacity);
	array->entries = entries;
	array->size = 0;
	array->capacity = capacity;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		if (!teds_strictmap_insert_from_pair(intern, val)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();
}

static void teds_strictmap_entries_init_from_traversable_pairs(teds_strictmap *intern, zend_object *obj)
{
	teds_strictmap_entries *array = &intern->array;
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

		if (!teds_strictmap_insert_from_pair(intern, pair)) {
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
	zend_object *object = teds_strictmap_new(teds_ce_StrictMap);
	teds_strictmap *intern = teds_strictmap_from_obj(object);
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_strictmap_entries_init_from_array_pairs(intern, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_strictmap_entries_init_from_traversable_pairs(intern, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_StrictMap, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_StrictMap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_strictmap_new(teds_ce_StrictMap);
	teds_strictmap *intern = teds_strictmap_from_obj(object);
	teds_strictmap_entries_init_from_array_pairs(intern, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_StrictMap, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);

	if (teds_strictmap_entries_empty_size(&intern->array)) {
		RETURN_EMPTY_ARRAY();
	}
	teds_strictmap_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_StrictMap, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	teds_strictmap_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_StrictMap, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	teds_strictmap_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_StrictMap, indexOfKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	const size_t len = intern->array.size;
	if (len == 0) {
		return;
	}
	teds_strictmap_entry *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(key, &entries[i].key)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_StrictMap, indexOfValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	const size_t len = intern->array.size;
	if (len == 0) {
		return;
	}
	teds_strictmap_entry *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i].value)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

static teds_strictmap_entry *teds_strictmap_find_key(const teds_strictmap *intern, zval *key, uint32_t hash)
{
	const size_t len = intern->array.size;
	teds_strictmap_entry *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (hash == TEDS_ENTRY_HASH(&entries[i]) && EXPECTED(zend_is_identical(key, &entries[i].key))) {
			return &entries[i];
		}
	}
	return NULL;
}

static teds_strictmap_entry *teds_strictmap_find_key_computing_hash(const teds_strictmap *intern, zval *key)
{
	return teds_strictmap_find_key(intern, key, teds_strict_hash(key));
}

static teds_strictmap_entry *teds_strictmap_find_value(const teds_strictmap *intern, zval *value)
{
	const size_t len = intern->array.size;
	teds_strictmap_entry *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i].value)) {
			return &entries[i];
		}
	}
	return NULL;
}

static void teds_strictmap_remove_key(teds_strictmap *intern, zval *key)
{
	if (intern->array.size == 0) {
		return;
	}
	teds_strictmap_entry *entry = teds_strictmap_find_key_computing_hash(intern, key);
	if (!entry) {
		return;
	}
	zval old_key;
	zval old_value;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	ZVAL_COPY_VALUE(&old_value, &entry->value);
	teds_strictmap_entry *end = intern->array.entries + intern->array.size - 1;
	ZEND_ASSERT(entry <= end);
	for (; entry < end; ) {
		teds_strictmap_entry *next = &entry[1];
		ZVAL_COPY_VALUE(&entry->key, &next->key);
		ZVAL_COPY_VALUE(&entry->value, &next->value);
		TEDS_ENTRY_HASH(entry) = TEDS_ENTRY_HASH(next);
		entry = next;
	}
	intern->array.size--;

	// TODO move entries and hashes
	zval_ptr_dtor(&old_key);
	zval_ptr_dtor(&old_value);
}
PHP_METHOD(Teds_StrictMap, offsetExists)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	if (intern->array.size > 0) {
		teds_strictmap_entry *entry = teds_strictmap_find_key_computing_hash(intern, key);
		if (entry) {
			RETURN_BOOL(Z_TYPE(entry->value) != IS_NULL);
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_StrictMap, offsetGet)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	if (intern->array.size > 0) {
		teds_strictmap_entry *entry = teds_strictmap_find_key_computing_hash(intern, key);
		if (entry) {
			RETURN_COPY_VALUE(&entry->value);
		}
	}
	RETURN_NULL();
}

static void teds_strictmap_insert(teds_strictmap *intern, zval *key, zval *value) {
	const uint32_t hash = teds_strict_hash(key);
	teds_strictmap_entry *entry = teds_strictmap_find_key(intern, key, hash);
	if (entry) {
		/* Replace old value, then free old value */
		zval old;
		ZVAL_COPY_VALUE(&old, &entry->value);
		ZVAL_COPY(&entry->value, value);
		ZEND_ASSERT(TEDS_ENTRY_HASH(entry) == hash); /* Not clobbered by ZVAL_COPY macro */
		zval_ptr_dtor(&old);
		return;
	}
	/* Reallocate and append */
	teds_strictmap_entries *array = &intern->array;
	if (array->size >= array->capacity) {
		ZEND_ASSERT(array->size == array->capacity);
		const size_t new_capacity = teds_strictmap_next_pow2_capacity(array->size + 1);
		teds_strictmap_entries_raise_capacity(&intern->array, new_capacity);
	}

	entry = &array->entries[array->size];
	ZVAL_COPY(&entry->key, key);
	ZVAL_COPY(&entry->value, value);
	TEDS_ENTRY_HASH(entry) = hash;
	array->size++;
}

PHP_METHOD(Teds_StrictMap, offsetSet)
{
	zval *key;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	teds_strictmap_insert(intern, key, value);
}

PHP_METHOD(Teds_StrictMap, offsetUnset)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	teds_strictmap_remove_key(intern, key);
}

PHP_METHOD(Teds_StrictMap, containsValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	teds_strictmap_entry *entry = teds_strictmap_find_value(intern, value);
	if (entry != NULL) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_StrictMap, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	const size_t len = intern->array.size;
	if (len > 0) {
		teds_strictmap_entry *entry = teds_strictmap_find_key_computing_hash(intern, key);
		RETURN_BOOL(entry != NULL);
	}
	RETURN_FALSE;
}

static void teds_strictmap_return_pairs(zval *return_value, teds_strictmap *intern)
{
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	teds_strictmap_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_StrictMap, jsonSerialize)
{
	/* json_encoder.c will always encode objects as {"0":..., "1":...}, and detects recursion if an object returns its internal property array, so we have to return a new array */
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	teds_strictmap_return_pairs(return_value, intern);
}

PHP_METHOD(Teds_StrictMap, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	teds_strictmap_return_pairs(return_value, intern);
}

static void teds_strictmap_clear(teds_strictmap *intern) {
	teds_strictmap_entries *array = &intern->array;

	if (teds_strictmap_entries_empty_capacity(array)) {
		return;
	}
	teds_strictmap_entry *entries = intern->array.entries;
	size_t size = intern->array.size;
	intern->array.entries = (teds_strictmap_entry *)empty_entry_list;
	intern->array.size = 0;
	intern->array.capacity = 0;

	teds_strictmap_entries_dtor_range(entries, 0, size);
	efree(entries);
	/* Could call teds_strictmap_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StrictMap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	teds_strictmap_clear(intern);
}

PHP_MINIT_FUNCTION(teds_strictmap)
{
	teds_ce_StrictMap = register_class_Teds_StrictMap(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
	teds_ce_StrictMap->create_object = teds_strictmap_new;

	memcpy(&teds_handler_StrictMap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictMap.offset          = XtOffsetOf(teds_strictmap, std);
	teds_handler_StrictMap.clone_obj       = teds_strictmap_clone;
	teds_handler_StrictMap.count_elements  = teds_strictmap_count_elements;
	teds_handler_StrictMap.get_properties  = teds_strictmap_get_properties;
	teds_handler_StrictMap.get_gc          = teds_strictmap_get_gc;
	teds_handler_StrictMap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictMap.free_obj        = teds_strictmap_free_storage;

	teds_ce_StrictMap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StrictMap->get_iterator = teds_strictmap_get_iterator;

	return SUCCESS;
}
