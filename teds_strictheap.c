/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on teds_stablesortedlistset.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_strictheap_arginfo.h"
#include "teds_strictheap.h"
#include "teds_util.h"
#include "teds.h"
#include "teds_interfaces.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StrictMinHeap;
zend_class_entry *teds_ce_StrictMinHeap;
zend_object_handlers teds_handler_StrictMaxHeap;
zend_class_entry *teds_ce_StrictMaxHeap;

typedef struct _teds_strictheap_entry {
	zval key;
} teds_strictheap_entry;

/* This is a placeholder value to distinguish between empty and uninitialized StrictHeap instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const teds_strictheap_entry empty_entry_list[1];

typedef struct _teds_strictheap_entries {
	uint32_t size;
	uint32_t capacity;
	teds_strictheap_entry *entries;
} teds_strictheap_entries;

typedef struct _teds_strictheap {
	teds_strictheap_entries		array;
	zend_object				std;
} teds_strictheap;

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_strictheap_entries_empty_capacity(teds_strictheap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static void teds_strictheap_entries_raise_capacity(teds_strictheap_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(new_capacity > array->capacity);
	if (teds_strictheap_entries_empty_capacity(array)) {
		array->entries = safe_emalloc(new_capacity, sizeof(teds_strictheap_entry), 0);
	} else {
		array->entries = safe_erealloc(array->entries, new_capacity, sizeof(teds_strictheap_entry), 0);
	}
	array->capacity = new_capacity;
}

static zend_always_inline bool teds_strictheap_should_be_above(zval *a, zval *b, const bool is_min_heap) {
	zend_long result = teds_stable_compare(a, b);
	return is_min_heap ? result < 0 : result > 0;
}

static zend_always_inline void teds_strictheap_entries_insert(teds_strictheap_entries *array, zval *key, const bool is_min_heap) {
	/* Reallocate if needed and sift down. */
	uint32_t offset = array->size;
	if (offset >= array->capacity) {
		/* FIXME bounds check */
		ZEND_ASSERT(offset == array->capacity);
		const size_t new_capacity = teds_strictheap_next_pow2_capacity((size_t)offset + 1);
		teds_strictheap_entries_raise_capacity(array, new_capacity);
	}
	teds_strictheap_entry *const entries = array->entries;
	while (offset > 0) {
		uint32_t new_offset = offset >> 1;
		if (teds_strictheap_should_be_above(key, &entries[new_offset].key, is_min_heap)) {
			ZVAL_COPY_VALUE(&entries[offset].key, &entries[new_offset].key);
			offset = new_offset;
		} else {
			break;
		}
	}
	ZVAL_COPY(&entries[offset].key, key);
	array->size++;
	ZEND_ASSERT(offset < array->size);
}

static void teds_strictheap_entries_clear(teds_strictheap_entries *intern);

static teds_strictheap *teds_strictheap_from_object(zend_object *obj)
{
	return (teds_strictheap*)((char*)(obj) - XtOffsetOf(teds_strictheap, std));
}

#define Z_STRICTHEAP_P(zv)  teds_strictheap_from_object(Z_OBJ_P((zv)))
#define Z_STRICTHEAP_ENTRIES_P(zv) (&teds_strictheap_from_object(Z_OBJ_P((zv)))->array)

static bool teds_strictheap_entries_uninitialized(teds_strictheap_entries *array)
{
	ZEND_ASSERT(array->size <= array->capacity);
	if (array->entries == NULL) {
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static teds_strictheap_entry *teds_strictheap_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(teds_strictheap_entry), 0);
}

static void teds_strictheap_entries_init_from_array(teds_strictheap_entries *array, zend_array *values, const bool is_min_heap)
{
	uint32_t size = zend_hash_num_elements(values);
	if (size > 0) {
		zval *val;
		uint32_t capacity = size;

		array->size = 0; /* reset size in case emalloc() fails */
		array->capacity = 0;
		array->entries = teds_strictheap_allocate_entries(capacity);
		array->capacity = size;
		ZEND_HASH_FOREACH_VAL(values, val)  {
			teds_strictheap_entries_insert(array, val, is_min_heap);
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (teds_strictheap_entry *)empty_entry_list;
	}
}

static void teds_strictheap_entries_init_from_traversable(teds_strictheap_entries *array, zend_object *obj, const bool is_min_heap)
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
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		Z_TRY_ADDREF_P(value);
		teds_strictheap_entries_insert(array, value, is_min_heap);

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

/* Copies the range [begin, end) into the strictheap, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_strictheap_copy_range(teds_strictheap_entries *array, size_t offset, teds_strictheap_entry *begin, teds_strictheap_entry *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	teds_strictheap_entry *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		begin++;
		to++;
	}
}

static void teds_strictheap_entries_copy_ctor(teds_strictheap_entries *to, teds_strictheap_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (teds_strictheap_entry *)empty_entry_list;
		return;
	}

	const size_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(teds_strictheap_entry), 0);
	to->size = size;
	to->capacity = capacity;

	teds_strictheap_entry *begin = from->entries, *end = from->entries + size;
	teds_strictheap_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_strictheap_entries_dtor_range(teds_strictheap_entry *start, size_t from, size_t to)
{
	teds_strictheap_entry *begin = start + from, *end = start + to;
	while (begin < end) {
		zval_ptr_dtor(&begin->key);
		begin++;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_strictheap_entries_dtor(teds_strictheap_entries *array)
{
	if (!teds_strictheap_entries_empty_capacity(array)) {
		teds_strictheap_entries_dtor_range(array->entries, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_strictheap_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_strictheap *intern = teds_strictheap_from_object(obj);

	*table = &intern->array.entries[0].key;
	*n = (int)intern->array.size;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_strictheap_get_properties(zend_object *obj)
{
	teds_strictheap *intern = teds_strictheap_from_object(obj);
	HashTable *ht = zend_std_get_properties(obj);

	/* Re-initialize properties array */

	// Note that destructors may mutate the original array,
	// so we fetch the size and circular buffer each time to avoid invalid memory accesses.
	for (uint32_t i = 0; i < intern->array.size; i++) {
		zval *elem = &intern->array.entries[i].key;
		Z_TRY_ADDREF_P(elem);
		zend_hash_index_update(ht, i, elem);
	}
	const uint32_t properties_size = zend_hash_num_elements(ht);
	if (UNEXPECTED(properties_size > intern->array.size)) {
		for (uint32_t i = intern->array.size; i < properties_size; i++) {
			zend_hash_index_del(ht, i);
		}
	}

	return ht;
}

static void teds_strictheap_free_storage(zend_object *object)
{
	teds_strictheap *intern = teds_strictheap_from_object(object);
	teds_strictheap_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_strictheap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_strictheap *intern;

	intern = zend_object_alloc(sizeof(teds_strictheap), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StrictMinHeap || class_type == teds_ce_StrictMaxHeap);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = class_type == teds_ce_StrictMinHeap ? &teds_handler_StrictMinHeap : &teds_handler_StrictMaxHeap;

	if (orig && clone_orig) {
		teds_strictheap *other = teds_strictheap_from_object(orig);
		teds_strictheap_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_strictheap_new(zend_class_entry *class_type)
{
	return teds_strictheap_new_ex(class_type, NULL, 0);
}


static zend_object *teds_strictheap_clone(zend_object *old_object)
{
	zend_object *new_object = teds_strictheap_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_strictheap_count_elements(zend_object *object, zend_long *count)
{
	teds_strictheap *intern;

	intern = teds_strictheap_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this StrictHeap */
PHP_METHOD(Teds_StrictMinHeap, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(Z_STRICTHEAP_ENTRIES_P(ZEND_THIS)->size);
}

/* Get whether this StrictHeap is empty */
PHP_METHOD(Teds_StrictMinHeap, isEmpty)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(Z_STRICTHEAP_ENTRIES_P(ZEND_THIS)->size == 0);
}

/* Get whether this StrictHeap is not empty */
PHP_METHOD(Teds_StrictMinHeap, valid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(Z_STRICTHEAP_ENTRIES_P(ZEND_THIS)->size != 0);
}
/* Create this from an iterable */
PHP_METHOD(Teds_StrictMinHeap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictheap *intern = Z_STRICTHEAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_strictheap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictHeap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (teds_strictheap_entry *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_strictheap_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable), true);
			return;
		case IS_OBJECT:
			teds_strictheap_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable), true);
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

/* Create this from an iterable */
PHP_METHOD(Teds_StrictMaxHeap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictheap *intern = Z_STRICTHEAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_strictheap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictHeap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.size = 0;
		intern->array.capacity = 0;
		intern->array.entries = (teds_strictheap_entry *)empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_strictheap_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable), false);
			return;
		case IS_OBJECT:
			teds_strictheap_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable), false);
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StrictMinHeap, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	size_t raw_size = zend_hash_num_elements(raw_data);
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!teds_strictheap_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		array->entries = (teds_strictheap_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(array->entries == NULL);

	const size_t capacity = teds_strictheap_next_pow2_capacity(raw_size);
	teds_strictheap_entry *entries = safe_emalloc(capacity, sizeof(teds_strictheap_entry), 0);
	array->size = 0;
	array->capacity = capacity;
	array->entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictheap_entries_clear(array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictHeap::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		teds_strictheap_entries_insert(array, val, true);
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Teds_StrictMaxHeap, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	size_t raw_size = zend_hash_num_elements(raw_data);
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!teds_strictheap_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (raw_size == 0) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		array->entries = (teds_strictheap_entry *)empty_entry_list;
		return;
	}

	ZEND_ASSERT(array->entries == NULL);

	const size_t capacity = teds_strictheap_next_pow2_capacity(raw_size);
	teds_strictheap_entry *entries = safe_emalloc(capacity, sizeof(teds_strictheap_entry), 0);
	array->size = 0;
	array->capacity = capacity;
	array->entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictheap_entries_clear(array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictHeap::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		teds_strictheap_entries_insert(array, val, false);
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Teds_StrictMinHeap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_strictheap_new(teds_ce_StrictMinHeap);
	teds_strictheap *intern = teds_strictheap_from_object(object);
	teds_strictheap_entries_init_from_array(&intern->array, array_ht, true);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_StrictMaxHeap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_strictheap_new(teds_ce_StrictMinHeap);
	teds_strictheap *intern = teds_strictheap_from_object(object);
	teds_strictheap_entries_init_from_array(&intern->array, array_ht, true);

	RETURN_OBJ(object);
}

/* Shifts values. Callers should adjust size and handle zval reference counting. */
static zend_always_inline void teds_strictheap_entries_remove_top(teds_strictheap_entries *const array, const bool is_min_heap)
{
	teds_strictheap_entry *const entries = array->entries;
	ZEND_ASSERT(array->size > 0);
	const uint32_t len = --array->size;
	teds_strictheap_entry *replacement = &entries[len];
	uint32_t offset = 0;


	/*    0
	 *  1   2
	 * 3 4 5 6 */
	while (true) {
		uint32_t new_offset = offset * 2 + 1;
		if (new_offset >= len) {
			break;
		}
		teds_strictheap_entry *new_entry = &entries[new_offset];
		if (new_offset + 1 < len) {
			if (teds_strictheap_should_be_above(&(new_entry + 1)->key, &new_entry->key, is_min_heap)) {
				new_offset++;
				new_entry++;
			}
		}
		if (!teds_strictheap_should_be_above(&new_entry->key, &replacement->key, is_min_heap)) {
			break;
		}
		//fprintf(stderr, "Copy entry %d from %d\n", offset, (int)(new_entry - entries));
		ZEND_ASSERT(Z_TYPE(new_entry->key) != IS_UNDEF);
		ZVAL_COPY_VALUE(&entries[offset].key, &new_entry->key);
		offset = new_offset;
	}
	//fprintf(stderr, "Copy replacement %d to %d\n", (int)(replacement - entries), (int)offset);
	ZEND_ASSERT(Z_TYPE(replacement->key) != IS_UNDEF);
	ZVAL_COPY_VALUE(&entries[offset].key, &replacement->key);
}

PHP_METHOD(Teds_StrictMinHeap, extract) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty StrictHeap", 0);
		RETURN_THROWS();
	}
	RETVAL_COPY_VALUE(&array->entries[0].key);
	teds_strictheap_entries_remove_top(array, true);
}

PHP_METHOD(Teds_StrictMaxHeap, extract) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty StrictHeap", 0);
		RETURN_THROWS();
	}
	RETVAL_COPY_VALUE(&array->entries[0].key);
	teds_strictheap_entries_remove_top(array, false);
}

PHP_METHOD(Teds_StrictMinHeap, next)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty StrictHeap", 0);
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_COPY_VALUE(&tmp, &array->entries[0].key);
	teds_strictheap_entries_remove_top(array, true);
	zval_ptr_dtor(&tmp);
}

PHP_METHOD(Teds_StrictMaxHeap, next)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty StrictHeap", 0);
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_COPY_VALUE(&tmp, &array->entries[0].key);
	teds_strictheap_entries_remove_top(array, false);
	zval_ptr_dtor(&tmp);
}

/* top */
PHP_METHOD(Teds_StrictMinHeap, top)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap *intern = Z_STRICTHEAP_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty StrictHeap", 0);
		RETURN_THROWS();
	}

	ZEND_ASSERT(Z_TYPE(intern->array.entries[0].key) != IS_UNDEF);
	RETURN_COPY(&intern->array.entries[0].key);
}

PHP_METHOD(Teds_StrictMinHeap, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictheap_entries_insert(Z_STRICTHEAP_ENTRIES_P(ZEND_THIS), value, true);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_StrictMaxHeap, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictheap_entries_insert(Z_STRICTHEAP_ENTRIES_P(ZEND_THIS), value, false);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_StrictMinHeap, rewind)
{
	ZEND_PARSE_PARAMETERS_NONE();
	TEDS_RETURN_VOID();
}

static void teds_strictheap_entries_clear(teds_strictheap_entries *array) {
	if (teds_strictheap_entries_empty_capacity(array)) {
		return;
	}
	teds_strictheap_entry *entries = array->entries;
	uint32_t size = array->size;
	array->entries = (teds_strictheap_entry *)empty_entry_list;
	array->size = 0;
	array->capacity = 0;

	teds_strictheap_entries_dtor_range(entries, 0, size);
	efree(entries);
	/* Could call teds_strictheap_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StrictMinHeap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries_clear(Z_STRICTHEAP_ENTRIES_P(ZEND_THIS));
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_StrictMinHeap, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	uint32_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	/* sizeof(teds_strictheap_entry) === sizeof(zval) */
	teds_strictheap_entry *entries = array->entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (uint32_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].key;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_StrictMinHeap, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	if (!array->size) {
		RETURN_EMPTY_ARRAY();
	}
	/* sizeof(teds_strictheap_entry) === sizeof(zval) */
	zend_array *values = zend_new_array(array->size);

	/* Go through values and add values to the return array */
	for (uint32_t i = 0; i < array->size; i++) {
		zval *key = &array->entries[i].key;
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

PHP_METHOD(Teds_StrictMinHeap, contains)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);

	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_BOOL(teds_zval_range_contains(value, (zval*) array->entries, array->size));
}

PHP_MINIT_FUNCTION(teds_strictheap)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_StrictMinHeap = register_class_Teds_StrictMinHeap(zend_ce_iterator, teds_ce_Collection);
	teds_ce_StrictMinHeap->create_object = teds_strictheap_new;

	memcpy(&teds_handler_StrictMinHeap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictMinHeap.offset          = XtOffsetOf(teds_strictheap, std);
	teds_handler_StrictMinHeap.clone_obj       = teds_strictheap_clone;
	teds_handler_StrictMinHeap.count_elements  = teds_strictheap_count_elements;
	teds_handler_StrictMinHeap.get_properties  = teds_strictheap_get_properties;
	teds_handler_StrictMinHeap.get_gc          = teds_strictheap_get_gc;
	teds_handler_StrictMinHeap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictMinHeap.free_obj        = teds_strictheap_free_storage;

	teds_ce_StrictMinHeap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	teds_ce_StrictMaxHeap = register_class_Teds_StrictMaxHeap(zend_ce_iterator, teds_ce_Collection);
	teds_ce_StrictMaxHeap->create_object = teds_strictheap_new;

	memcpy(&teds_handler_StrictMaxHeap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictMaxHeap.offset          = XtOffsetOf(teds_strictheap, std);
	teds_handler_StrictMaxHeap.clone_obj       = teds_strictheap_clone;
	teds_handler_StrictMaxHeap.count_elements  = teds_strictheap_count_elements;
	//teds_handler_StrictMaxHeap.get_properties  = teds_strictheap_get_properties;
	teds_handler_StrictMaxHeap.get_gc          = teds_strictheap_get_gc;
	teds_handler_StrictMaxHeap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictMaxHeap.free_obj        = teds_strictheap_free_storage;

	teds_ce_StrictMaxHeap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return SUCCESS;
}
