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
 * - TODO: associate StrictSet with linked list of iterators
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_strictset_arginfo.h"
#include "teds_strictset.h"
#include "teds_util.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StrictSet;
zend_class_entry *teds_ce_StrictSet;

#define TEDS_HASH(zval) ((zval).u2.extra)
#define TEDS_ENTRY_HASH(entry) ((entry)->hash)

typedef struct _teds_strictset_entry {
	zval key;   /* TODO: Stores Z_NEXT - similar to https://github.com/php/php-src/pull/6588 */
	uint64_t hash;
} teds_strictset_entry;

/* This is a placeholder value to distinguish between empty and uninitialized StrictSet instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const teds_strictset_entry empty_entry_list[1];

typedef struct _teds_strictset_entries {
	size_t size;
	size_t capacity;
	teds_strictset_entry *entries;
} teds_strictset_entries;

typedef struct _teds_strictset {
	teds_strictset_entries		array;
	zend_object				std;
} teds_strictset;

static teds_strictset_entry *teds_strictset_find_key(const teds_strictset *intern, zval *key, uint64_t hash);
static void teds_strictset_entries_raise_capacity(teds_strictset_entries *array, size_t new_capacity);

static bool teds_strictset_insert(teds_strictset *intern, zval *key) {
	const uint64_t hash = teds_strict_hash(key);
	teds_strictset_entry *entry = teds_strictset_find_key(intern, key, hash);
	if (entry) {
		return false;
	}
	/* Reallocate and append */
	teds_strictset_entries *array = &intern->array;
	if (array->size >= array->capacity) {
		ZEND_ASSERT(array->size == array->capacity);
		const size_t new_capacity = teds_strictset_next_pow2_capacity(array->size + 1);
		teds_strictset_entries_raise_capacity(&intern->array, new_capacity);
	}

	entry = &array->entries[array->size];
	ZVAL_COPY(&entry->key, key);
	TEDS_ENTRY_HASH(entry) = hash;
	array->size++;
	return true;
}

static void teds_strictset_clear(teds_strictset *intern);

/* Used by InternalIterator returned by StrictSet->getIterator() */
typedef struct _teds_strictset_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_strictset_it;

static teds_strictset *teds_strictset_from_obj(zend_object *obj)
{
	return (teds_strictset*)((char*)(obj) - XtOffsetOf(teds_strictset, std));
}

#define Z_STRICTSET_P(zv)  teds_strictset_from_obj(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_strictset_entries_empty_capacity(teds_strictset_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static zend_always_inline void teds_strictset_entry_compute_and_store_hash(teds_strictset_entry *entry) {
	TEDS_ENTRY_HASH(entry) = teds_strict_hash(&entry->key);
}

static bool teds_strictset_entries_uninitialized(teds_strictset_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->entries == NULL) {
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static teds_strictset_entry *teds_strictset_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(teds_strictset_entry), 0);
}

static void teds_strictset_entries_init_from_array(teds_strictset_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zval *val;
		teds_strictset_entry *entries;
		int i = 0;
		zend_long capacity = teds_strictset_next_pow2_capacity(size);

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = entries = teds_strictset_allocate_entries(capacity);
		array->capacity = size;
		array->size = size;
		ZEND_HASH_FOREACH_VAL(values, val)  {
			ZEND_ASSERT(i < size);
			teds_strictset_entry *entry = &entries[i];
			ZVAL_COPY(&entry->key, val);
			teds_strictset_entry_compute_and_store_hash(entry);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (teds_strictset_entry *)empty_entry_list;
	}
}

static void teds_strictset_entries_init_from_traversable(teds_strictset_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->capacity = 0;
	array->entries = NULL;
	teds_strictset_entry *entries = NULL;
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
				entries = safe_erealloc(entries, capacity, sizeof(teds_strictset_entry), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(teds_strictset_entry), 0);
			}
		}
		ZVAL_COPY_DEREF(&entries[size].key, value);
		teds_strictset_entry_compute_and_store_hash(&entries[size]);
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

static void teds_strictset_entries_raise_capacity(teds_strictset_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(new_capacity > array->capacity);
	if (teds_strictset_entries_empty_capacity(array)) {
		array->entries = safe_emalloc(new_capacity, sizeof(teds_strictset_entry), 0);
	} else {
		array->entries = safe_erealloc(array->entries, new_capacity, sizeof(teds_strictset_entry), 0);
	}
	array->capacity = new_capacity;
}

/* Copies the range [begin, end) into the strictset, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_strictset_copy_range(teds_strictset_entries *array, size_t offset, teds_strictset_entry *begin, teds_strictset_entry *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	teds_strictset_entry *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		TEDS_ENTRY_HASH(to) = TEDS_ENTRY_HASH(begin);
		begin++;
		to++;
	}
}

static void teds_strictset_entries_copy_ctor(teds_strictset_entries *to, teds_strictset_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (teds_strictset_entry *)empty_entry_list;
		return;
	}

	const size_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(teds_strictset_entry), 0);
	to->size = size;
	to->capacity = capacity;

	teds_strictset_entry *begin = from->entries, *end = from->entries + size;
	teds_strictset_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_strictset_entries_dtor_range(teds_strictset_entry *start, size_t from, size_t to)
{
	teds_strictset_entry *begin = start + from, *end = start + to;
	while (begin < end) {
		zval_ptr_dtor(&begin->key);
		begin++;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_strictset_entries_dtor(teds_strictset_entries *array)
{
	if (!teds_strictset_entries_empty_capacity(array)) {
		teds_strictset_entries_dtor_range(array->entries, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_strictset_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_strictset *intern = teds_strictset_from_obj(obj);

	*table = &intern->array.entries[0].key;
	*n = (int)intern->array.size * 2;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_strictset_get_properties(zend_object *obj)
{
	teds_strictset *intern = teds_strictset_from_obj(obj);
	size_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	size_t old_length = zend_hash_num_elements(ht);
	teds_strictset_entry *entries = intern->array.entries;
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

static void teds_strictset_free_storage(zend_object *object)
{
	teds_strictset *intern = teds_strictset_from_obj(object);
	teds_strictset_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_strictset_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_strictset *intern;

	intern = zend_object_alloc(sizeof(teds_strictset), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StrictSet);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_StrictSet;

	if (orig && clone_orig) {
		teds_strictset *other = teds_strictset_from_obj(orig);
		teds_strictset_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_strictset_new(zend_class_entry *class_type)
{
	return teds_strictset_new_ex(class_type, NULL, 0);
}


static zend_object *teds_strictset_clone(zend_object *old_object)
{
	zend_object *new_object = teds_strictset_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_strictset_count_elements(zend_object *object, zend_long *count)
{
	teds_strictset *intern;

	intern = teds_strictset_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this StrictSet */
PHP_METHOD(Teds_StrictSet, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictset *intern = Z_STRICTSET_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this StrictSet is empty */
PHP_METHOD(Teds_StrictSet, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictset *intern = Z_STRICTSET_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_StrictSet, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictset *intern = Z_STRICTSET_P(ZEND_THIS);

	if (UNEXPECTED(!teds_strictset_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictSet::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (teds_strictset_entry *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_strictset_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_strictset_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StrictSet, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_strictset_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_strictset_it_rewind(zend_object_iterator *iter)
{
	((teds_strictset_it*)iter)->current = 0;
}

static int teds_strictset_it_valid(zend_object_iterator *iter)
{
	teds_strictset_it     *iterator = (teds_strictset_it*)iter;
	teds_strictset *object   = Z_STRICTSET_P(&iter->data);

	if (iterator->current >= 0 && ((zend_ulong) iterator->current) < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static teds_strictset_entry *teds_strictset_read_offset_helper(teds_strictset *intern, size_t offset)
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

static zval *teds_strictset_it_get_current_data(zend_object_iterator *iter)
{
	teds_strictset_it     *iterator = (teds_strictset_it*)iter;
	teds_strictset *object   = Z_STRICTSET_P(&iter->data);

	teds_strictset_entry *data = teds_strictset_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->key;
	}
}

static void teds_strictset_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_strictset_it     *iterator = (teds_strictset_it*)iter;
	teds_strictset *object   = Z_STRICTSET_P(&iter->data);

	teds_strictset_entry *data = teds_strictset_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_strictset_it_move_forward(zend_object_iterator *iter)
{
	((teds_strictset_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_strictset_it_funcs = {
	teds_strictset_it_dtor,
	teds_strictset_it_valid,
	teds_strictset_it_get_current_data,
	teds_strictset_it_get_current_key,
	teds_strictset_it_move_forward,
	teds_strictset_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_strictset_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_strictset_it *iterator;
	(void) ce;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_strictset_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_strictset_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_StrictSet, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	size_t raw_size = zend_hash_num_elements(raw_data);
	teds_strictset *intern = Z_STRICTSET_P(ZEND_THIS);
	if (UNEXPECTED(!teds_strictset_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(intern->array.size == 0);
		ZEND_ASSERT(intern->array.capacity == 0);
		intern->array.entries = (teds_strictset_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	const size_t capacity = teds_strictset_next_pow2_capacity(raw_size / 2);
	teds_strictset_entry *entries = safe_emalloc(capacity, sizeof(teds_strictset_entry), 0);
	intern->array.size = 0;
	intern->array.capacity = capacity;
	intern->array.entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictset_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictSet::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		teds_strictset_insert(intern, val);
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Teds_StrictSet, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_strictset_new(teds_ce_StrictSet);
	teds_strictset *intern = teds_strictset_from_obj(object);
	teds_strictset_entries_init_from_array(&intern->array, array_ht);

	RETURN_OBJ(object);
}

static zend_array *teds_strictset_create_array_copy(teds_strictset *intern) {
	teds_strictset_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_StrictSet, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictset *intern = Z_STRICTSET_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		/* NOTE: This macro sets immutable gc flags, differently from RETURN_ARR */
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_strictset_create_array_copy(intern));
}

static teds_strictset_entry *teds_strictset_find_key(const teds_strictset *intern, zval *key, uint64_t hash)
{
	const size_t len = intern->array.size;
	teds_strictset_entry *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (hash == TEDS_ENTRY_HASH(&entries[i]) && EXPECTED(zend_is_identical(key, &entries[i].key))) {
			return &entries[i];
		}
	}
	return NULL;
}

static teds_strictset_entry *teds_strictset_find_key_computing_hash(const teds_strictset *intern, zval *key)
{
	return teds_strictset_find_key(intern, key, teds_strict_hash(key));
}

static bool teds_strictset_remove_key(teds_strictset *intern, zval *key)
{
	if (intern->array.size == 0) {
		return false;
	}
	teds_strictset_entry *entry = teds_strictset_find_key_computing_hash(intern, key);
	if (!entry) {
		return false;
	}
	zval old_key;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	teds_strictset_entry *end = intern->array.entries + intern->array.size - 1;
	ZEND_ASSERT(entry <= end);
	for (; entry < end; ) {
		teds_strictset_entry *next = &entry[1];
		ZVAL_COPY_VALUE(&entry->key, &next->key);
		TEDS_ENTRY_HASH(entry) = TEDS_ENTRY_HASH(next);
		entry = next;
	}
	intern->array.size--;

	zval_ptr_dtor(&old_key);
	return true;
}

PHP_METHOD(Teds_StrictSet, remove)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictset *intern = Z_STRICTSET_P(ZEND_THIS);
	RETURN_BOOL(teds_strictset_remove_key(intern, value));
}

PHP_METHOD(Teds_StrictSet, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictset *intern = Z_STRICTSET_P(ZEND_THIS);
	RETURN_BOOL(teds_strictset_insert(intern, value));
}

PHP_METHOD(Teds_StrictSet, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictset *intern = Z_STRICTSET_P(ZEND_THIS);
	teds_strictset_entry *entry = teds_strictset_find_key_computing_hash(intern, value);
	RETURN_BOOL(entry != NULL);
}

static void teds_strictset_clear(teds_strictset *intern) {
	teds_strictset_entries *array = &intern->array;

	if (teds_strictset_entries_empty_capacity(array)) {
		return;
	}
	teds_strictset_entry *entries = intern->array.entries;
	size_t size = intern->array.size;
	intern->array.entries = (teds_strictset_entry *)empty_entry_list;
	intern->array.size = 0;
	intern->array.capacity = 0;

	teds_strictset_entries_dtor_range(entries, 0, size);
	efree(entries);
	/* Could call teds_strictset_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StrictSet, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictset *intern = Z_STRICTSET_P(ZEND_THIS);
	teds_strictset_clear(intern);
}

PHP_MINIT_FUNCTION(teds_strictset)
{
	teds_ce_StrictSet = register_class_Teds_StrictSet(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce);
	teds_ce_StrictSet->create_object = teds_strictset_new;

	memcpy(&teds_handler_StrictSet, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictSet.offset          = XtOffsetOf(teds_strictset, std);
	teds_handler_StrictSet.clone_obj       = teds_strictset_clone;
	teds_handler_StrictSet.count_elements  = teds_strictset_count_elements;
	teds_handler_StrictSet.get_properties  = teds_strictset_get_properties;
	teds_handler_StrictSet.get_gc          = teds_strictset_get_gc;
	teds_handler_StrictSet.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictSet.free_obj        = teds_strictset_free_storage;

	teds_ce_StrictSet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StrictSet->get_iterator = teds_strictset_get_iterator;

	return SUCCESS;
}
