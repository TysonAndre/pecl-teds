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
 * - TODO: associate SortedStrictSet with linked list of iterators
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_sortedstrictset_arginfo.h"
#include "teds_sortedstrictset.h"
#include "teds_util.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_SortedStrictSet;
zend_class_entry *teds_ce_SortedStrictSet;

typedef struct _teds_sortedstrictset_entry {
	zval key;
} teds_sortedstrictset_entry;

/* This is a placeholder value to distinguish between empty and uninitialized SortedStrictSet instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const teds_sortedstrictset_entry empty_entry_list[1];

typedef struct _teds_sortedstrictset_entries {
	size_t size;
	size_t capacity;
	teds_sortedstrictset_entry *entries;
} teds_sortedstrictset_entries;

typedef struct _teds_sortedstrictset {
	teds_sortedstrictset_entries		array;
	zend_object				std;
} teds_sortedstrictset;

typedef struct _teds_sortedstrictset_search_result {
	teds_sortedstrictset_entry *entry;
	bool found;
} teds_sortedstrictset_search_result;

static void teds_sortedstrictset_entries_raise_capacity(teds_sortedstrictset_entries *array, size_t new_capacity);
static teds_sortedstrictset_search_result teds_sortedstrictset_sorted_search_for_key(const teds_sortedstrictset *intern, zval *key);

static bool teds_sortedstrictset_insert(teds_sortedstrictset *intern, zval *key) {
	teds_sortedstrictset_search_result result = teds_sortedstrictset_sorted_search_for_key(intern, key);
	if (result.found) {
		return false;
	}
	/* Reallocate and insert (insertion sort) */
	teds_sortedstrictset_entries *array = &intern->array;
	teds_sortedstrictset_entry *entry = result.entry;
	if (array->size >= array->capacity) {
		const size_t new_offset = result.entry - array->entries;
		ZEND_ASSERT(array->size == array->capacity);
		const size_t new_capacity = teds_sortedstrictset_next_pow2_capacity(array->size + 1);
		teds_sortedstrictset_entries_raise_capacity(&intern->array, new_capacity);
		entry = array->entries + new_offset;
	}

	for (teds_sortedstrictset_entry *it = array->entries + array->size; it > entry; it--) {
		ZVAL_COPY_VALUE(&it[0].key, &it[-1].key);
	}

	ZVAL_COPY(&entry->key, key);
	array->size++;
	return true;
}

static void teds_sortedstrictset_clear(teds_sortedstrictset *intern);

/* Used by InternalIterator returned by SortedStrictSet->getIterator() */
typedef struct _teds_sortedstrictset_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_sortedstrictset_it;

static teds_sortedstrictset *teds_sortedstrictset_from_obj(zend_object *obj)
{
	return (teds_sortedstrictset*)((char*)(obj) - XtOffsetOf(teds_sortedstrictset, std));
}

#define Z_SORTEDSTRICTSET_P(zv)  teds_sortedstrictset_from_obj(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_sortedstrictset_entries_empty_capacity(teds_sortedstrictset_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static bool teds_sortedstrictset_entries_uninitialized(teds_sortedstrictset_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->entries == NULL) {
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static teds_sortedstrictset_entry *teds_sortedstrictset_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(teds_sortedstrictset_entry), 0);
}

static void teds_sortedstrictset_entries_init_from_array(teds_sortedstrictset_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zval *val;
		teds_sortedstrictset_entry *entries;
		int i = 0;
		zend_long capacity = teds_sortedstrictset_next_pow2_capacity(size);

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = entries = teds_sortedstrictset_allocate_entries(capacity);
		array->capacity = size;
		array->size = size;
		ZEND_HASH_FOREACH_VAL(values, val)  {
			ZEND_ASSERT(i < size);
			teds_sortedstrictset_entry *entry = &entries[i];
			ZVAL_COPY_DEREF(&entry->key, val);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (teds_sortedstrictset_entry *)empty_entry_list;
	}
}

static void teds_sortedstrictset_entries_init_from_traversable(teds_sortedstrictset_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->capacity = 0;
	array->entries = NULL;
	teds_sortedstrictset_entry *entries = NULL;
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
				entries = safe_erealloc(entries, capacity, sizeof(teds_sortedstrictset_entry), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(teds_sortedstrictset_entry), 0);
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
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static void teds_sortedstrictset_entries_raise_capacity(teds_sortedstrictset_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(new_capacity > array->capacity);
	if (teds_sortedstrictset_entries_empty_capacity(array)) {
		array->entries = safe_emalloc(new_capacity, sizeof(teds_sortedstrictset_entry), 0);
	} else {
		array->entries = safe_erealloc(array->entries, new_capacity, sizeof(teds_sortedstrictset_entry), 0);
	}
	array->capacity = new_capacity;
}

/* Copies the range [begin, end) into the sortedstrictset, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_sortedstrictset_copy_range(teds_sortedstrictset_entries *array, size_t offset, teds_sortedstrictset_entry *begin, teds_sortedstrictset_entry *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	teds_sortedstrictset_entry *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		begin++;
		to++;
	}
}

static void teds_sortedstrictset_entries_copy_ctor(teds_sortedstrictset_entries *to, teds_sortedstrictset_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (teds_sortedstrictset_entry *)empty_entry_list;
		return;
	}

	const size_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(teds_sortedstrictset_entry), 0);
	to->size = size;
	to->capacity = capacity;

	teds_sortedstrictset_entry *begin = from->entries, *end = from->entries + size;
	teds_sortedstrictset_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_sortedstrictset_entries_dtor_range(teds_sortedstrictset_entry *start, size_t from, size_t to)
{
	teds_sortedstrictset_entry *begin = start + from, *end = start + to;
	while (begin < end) {
		zval_ptr_dtor(&begin->key);
		begin++;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_sortedstrictset_entries_dtor(teds_sortedstrictset_entries *array)
{
	if (!teds_sortedstrictset_entries_empty_capacity(array)) {
		teds_sortedstrictset_entries_dtor_range(array->entries, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_sortedstrictset_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(obj);

	*table = &intern->array.entries[0].key;
	*n = (int)intern->array.size * 2;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_sortedstrictset_get_properties(zend_object *obj)
{
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(obj);
	size_t len = intern->array.size;
	HashTable *ht = zend_std_get_properties(obj);
	size_t old_length = zend_hash_num_elements(ht);
	teds_sortedstrictset_entry *entries = intern->array.entries;
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

static void teds_sortedstrictset_free_storage(zend_object *object)
{
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(object);
	teds_sortedstrictset_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_sortedstrictset_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_sortedstrictset *intern;

	intern = zend_object_alloc(sizeof(teds_sortedstrictset), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_SortedStrictSet);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_SortedStrictSet;

	if (orig && clone_orig) {
		teds_sortedstrictset *other = teds_sortedstrictset_from_obj(orig);
		teds_sortedstrictset_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_sortedstrictset_new(zend_class_entry *class_type)
{
	return teds_sortedstrictset_new_ex(class_type, NULL, 0);
}


static zend_object *teds_sortedstrictset_clone(zend_object *old_object)
{
	zend_object *new_object = teds_sortedstrictset_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_sortedstrictset_count_elements(zend_object *object, zend_long *count)
{
	teds_sortedstrictset *intern;

	intern = teds_sortedstrictset_from_obj(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this SortedStrictSet */
PHP_METHOD(Teds_SortedStrictSet, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this SortedStrictSet is empty */
PHP_METHOD(Teds_SortedStrictSet, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_SortedStrictSet, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);

	if (UNEXPECTED(!teds_sortedstrictset_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\SortedStrictSet::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (teds_sortedstrictset_entry *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_sortedstrictset_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_sortedstrictset_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_SortedStrictSet, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_sortedstrictset_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_sortedstrictset_it_rewind(zend_object_iterator *iter)
{
	((teds_sortedstrictset_it*)iter)->current = 0;
}

static int teds_sortedstrictset_it_valid(zend_object_iterator *iter)
{
	teds_sortedstrictset_it     *iterator = (teds_sortedstrictset_it*)iter;
	teds_sortedstrictset *object   = Z_SORTEDSTRICTSET_P(&iter->data);

	if (iterator->current >= 0 && ((zend_ulong) iterator->current) < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static teds_sortedstrictset_entry *teds_sortedstrictset_read_offset_helper(teds_sortedstrictset *intern, size_t offset)
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

static zval *teds_sortedstrictset_it_get_current_data(zend_object_iterator *iter)
{
	teds_sortedstrictset_it     *iterator = (teds_sortedstrictset_it*)iter;
	teds_sortedstrictset *object   = Z_SORTEDSTRICTSET_P(&iter->data);

	teds_sortedstrictset_entry *data = teds_sortedstrictset_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->key;
	}
}

static void teds_sortedstrictset_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_sortedstrictset_it     *iterator = (teds_sortedstrictset_it*)iter;
	teds_sortedstrictset *object   = Z_SORTEDSTRICTSET_P(&iter->data);

	teds_sortedstrictset_entry *data = teds_sortedstrictset_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_sortedstrictset_it_move_forward(zend_object_iterator *iter)
{
	((teds_sortedstrictset_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_sortedstrictset_it_funcs = {
	teds_sortedstrictset_it_dtor,
	teds_sortedstrictset_it_valid,
	teds_sortedstrictset_it_get_current_data,
	teds_sortedstrictset_it_get_current_key,
	teds_sortedstrictset_it_move_forward,
	teds_sortedstrictset_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_sortedstrictset_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_sortedstrictset_it *iterator;
	(void)ce;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_sortedstrictset_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_sortedstrictset_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_SortedStrictSet, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	size_t raw_size = zend_hash_num_elements(raw_data);
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	if (UNEXPECTED(!teds_sortedstrictset_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(intern->array.size == 0);
		ZEND_ASSERT(intern->array.capacity == 0);
		intern->array.entries = (teds_sortedstrictset_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	const size_t capacity = teds_sortedstrictset_next_pow2_capacity(raw_size / 2);
	teds_sortedstrictset_entry *entries = safe_emalloc(capacity, sizeof(teds_sortedstrictset_entry), 0);
	intern->array.size = 0;
	intern->array.capacity = capacity;
	intern->array.entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_sortedstrictset_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\SortedStrictSet::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		teds_sortedstrictset_insert(intern, val);
	} ZEND_HASH_FOREACH_END();

}

PHP_METHOD(Teds_SortedStrictSet, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_sortedstrictset_new(teds_ce_SortedStrictSet);
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(object);
	teds_sortedstrictset_entries_init_from_array(&intern->array, array_ht);

	RETURN_OBJ(object);
}

static zend_array *teds_sortedstrictset_create_array_copy(teds_sortedstrictset *intern) {
	teds_sortedstrictset_entry *entries = intern->array.entries;
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

PHP_METHOD(Teds_SortedStrictSet, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		/* NOTE: This macro sets immutable gc flags, differently from RETURN_ARR */
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_sortedstrictset_create_array_copy(intern));
}

#define IMPLEMENT_READ_OFFSET_PHP_METHOD(methodName, index) \
PHP_METHOD(Teds_SortedStrictSet, methodName) \
{ \
	ZEND_PARSE_PARAMETERS_NONE(); \
	const teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS); \
	if (intern->array.size == 0) { \
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read " # methodName " of empty SortedStrictSet", 0); \
		RETURN_THROWS(); \
	} \
	teds_sortedstrictset_entry *entries = intern->array.entries; \
	RETVAL_COPY(&entries[(index)].key); \
}

IMPLEMENT_READ_OFFSET_PHP_METHOD(bottom, 0)
IMPLEMENT_READ_OFFSET_PHP_METHOD(top, intern->array.size - 1)

PHP_METHOD(Teds_SortedStrictSet, pop) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	if (intern->array.size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty SortedStrictSet", 0);
		RETURN_THROWS();
	}
	teds_sortedstrictset_entry *entry = &intern->array.entries[intern->array.size - 1];
	RETVAL_COPY_VALUE(&entry->key);
	intern->array.size--;
}

/* Shifts values. Callers should adjust size and handle zval reference counting. */
static void teds_sortedstrictset_remove_entry(teds_sortedstrictset_entry *entries, size_t len, teds_sortedstrictset_entry *entry)
{
	teds_sortedstrictset_entry *end = entries + len - 1;
	ZEND_ASSERT(entry <= end);
	/* Move entries */
	for (; entry < end; ) {
		ZVAL_COPY_VALUE(&entry->key, &entry[1].key);
		entry++;
	}
}

PHP_METHOD(Teds_SortedStrictSet, shift) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	const size_t len = intern->array.size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot shift from empty SortedStrictSet", 0);
		RETURN_THROWS();
	}
	teds_sortedstrictset_entry *entry = &intern->array.entries[0];
	RETVAL_COPY_VALUE(&entry->key);
	teds_sortedstrictset_remove_entry(entry, len, entry);
	intern->array.size--;
}

static teds_sortedstrictset_search_result teds_sortedstrictset_sorted_search_for_key(const teds_sortedstrictset *intern, zval *key)
{
	/* Currently, this is a binary search in an array, but later it would be a tree lookup. */
	teds_sortedstrictset_entry *const entries = intern->array.entries;
	size_t start = 0;
	size_t end = intern->array.size;
	while (start < end) {
		size_t mid = start + (end - start)/2;
		teds_sortedstrictset_entry *e = &entries[mid];
		int comparison = teds_stable_compare(key, &e->key);
		if (comparison == 0) {
			teds_sortedstrictset_search_result result;
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
	teds_sortedstrictset_search_result result;
	result.found = false;
	result.entry = &entries[start];
	return result;
}

static bool teds_sortedstrictset_remove_key(teds_sortedstrictset *intern, zval *key)
{
	if (intern->array.size == 0) {
		return false;
	}
	teds_sortedstrictset_search_result lookup = teds_sortedstrictset_sorted_search_for_key(intern, key);
	if (!lookup.found) {
		return false;
	}
	teds_sortedstrictset_entry *entry = lookup.entry;
	zval old_key;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	teds_sortedstrictset_entry *end = intern->array.entries + intern->array.size - 1;
	ZEND_ASSERT(entry <= end);
	for (; entry < end; ) {
		teds_sortedstrictset_entry *next = &entry[1];
		ZVAL_COPY_VALUE(&entry->key, &next->key);
		entry = next;
	}
	intern->array.size--;

	zval_ptr_dtor(&old_key);
	return true;
}

PHP_METHOD(Teds_SortedStrictSet, remove)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	RETURN_BOOL(teds_sortedstrictset_remove_key(intern, value));
}

PHP_METHOD(Teds_SortedStrictSet, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	RETURN_BOOL(teds_sortedstrictset_insert(intern, value));
}

PHP_METHOD(Teds_SortedStrictSet, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	teds_sortedstrictset_search_result result = teds_sortedstrictset_sorted_search_for_key(intern, value);
	RETURN_BOOL(result.found);
}

static void teds_sortedstrictset_clear(teds_sortedstrictset *intern) {
	teds_sortedstrictset_entries *array = &intern->array;

	if (teds_sortedstrictset_entries_empty_capacity(array)) {
		return;
	}
	teds_sortedstrictset_entry *entries = intern->array.entries;
	size_t size = intern->array.size;
	intern->array.entries = (teds_sortedstrictset_entry *)empty_entry_list;
	intern->array.size = 0;
	intern->array.capacity = 0;

	teds_sortedstrictset_entries_dtor_range(entries, 0, size);
	efree(entries);
	/* Could call teds_sortedstrictset_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_SortedStrictSet, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	teds_sortedstrictset_clear(intern);
}

PHP_MINIT_FUNCTION(teds_sortedstrictset)
{
	teds_ce_SortedStrictSet = register_class_Teds_SortedStrictSet(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce);
	teds_ce_SortedStrictSet->create_object = teds_sortedstrictset_new;

	memcpy(&teds_handler_SortedStrictSet, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_SortedStrictSet.offset          = XtOffsetOf(teds_sortedstrictset, std);
	teds_handler_SortedStrictSet.clone_obj       = teds_sortedstrictset_clone;
	teds_handler_SortedStrictSet.count_elements  = teds_sortedstrictset_count_elements;
	teds_handler_SortedStrictSet.get_properties  = teds_sortedstrictset_get_properties;
	teds_handler_SortedStrictSet.get_gc          = teds_sortedstrictset_get_gc;
	teds_handler_SortedStrictSet.dtor_obj        = zend_objects_destroy_object;
	teds_handler_SortedStrictSet.free_obj        = teds_sortedstrictset_free_storage;

	teds_ce_SortedStrictSet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_SortedStrictSet->get_iterator = teds_sortedstrictset_get_iterator;

	return SUCCESS;
}
