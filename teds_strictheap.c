/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on teds_strictsortedvectorset.c */
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
#include "teds_vector.h"
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

#define teds_strictheap_entries_raise_capacity teds_vector_entries_raise_capacity
#define teds_strictheap_count_elements teds_vector_count_elements
#define teds_strictheap_clear teds_vector_clear
#define teds_strictheap_get_properties_for teds_vector_get_properties_for
#define teds_strictheap_get_gc teds_vector_get_gc
#define teds_strictheap_free_storage teds_vector_free_storage

typedef teds_vector_entries teds_strictheap_entries;
typedef teds_vector teds_strictheap;

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

static zend_always_inline bool teds_strictheap_should_be_above(zval *a, zval *b, const bool is_min_heap) {
	zend_long result = teds_stable_compare(a, b);
	return is_min_heap ? result < 0 : result > 0;
}

static zend_always_inline void teds_strictheap_entries_insert(teds_strictheap_entries *array, zval *key, const bool is_min_heap) {
	/* Reallocate if needed and sift down. */
	ZEND_ASSERT(Z_TYPE_P(key) >= IS_NULL && Z_TYPE_P(key) <= IS_RESOURCE);
	uint32_t offset = array->size;
	if (offset >= array->capacity) {
		/* FIXME bounds check */
		ZEND_ASSERT(offset == array->capacity);
		const size_t new_capacity = teds_strictheap_next_pow2_capacity((size_t)offset + 1);
		teds_strictheap_entries_raise_capacity(array, new_capacity);
	}
	zval *const entries = array->entries;
	while (offset > 0) {
		uint32_t new_offset = offset >> 1;
		if (teds_strictheap_should_be_above(key, &entries[new_offset], is_min_heap)) {
			ZVAL_COPY_VALUE(&entries[offset], &entries[new_offset]);
			offset = new_offset;
		} else {
			break;
		}
	}
	ZVAL_COPY(&entries[offset], key);
	array->size++;
	array->should_rebuild_properties = true;
	ZEND_ASSERT(offset < array->size);
}

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

static zval *teds_strictheap_allocate_entries(size_t capacity) {
	return safe_emalloc(capacity, sizeof(zval), 0);
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
			ZVAL_DEREF(val);
			teds_strictheap_entries_insert(array, val, is_min_heap);
		} ZEND_HASH_FOREACH_END();
	} else {
		array->size = 0;
		array->capacity = 0;
		array->entries = (zval *)empty_entry_list;
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
			goto cleanup_iter;
		}
	}

	while (funcs->valid(iter) == SUCCESS) {
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		ZVAL_DEREF(value);

		teds_strictheap_entries_insert(array, value, is_min_heap);

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
	}

cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [begin, end) into the strictheap, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_strictheap_copy_range(teds_strictheap_entries *array, size_t offset, zval *begin, zval *end)
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

static void teds_strictheap_entries_copy_ctor(teds_strictheap_entries *to, teds_strictheap_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		to->size = 0;
		to->capacity = 0;
		to->entries = (zval *)empty_entry_list;
		return;
	}

	const size_t capacity = from->capacity;
	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(capacity, sizeof(zval), 0);
	to->size = size;
	to->capacity = capacity;

	zval *begin = from->entries, *end = from->entries + size;
	teds_strictheap_copy_range(to, 0, begin, end);
}

static zend_object *teds_strictheap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_strictheap *intern = zend_object_alloc(sizeof(teds_strictheap), class_type);
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
		intern->array.entries = (zval *)empty_entry_list;
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
		intern->array.entries = (zval *)empty_entry_list;
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

	const size_t raw_size = zend_hash_num_elements(raw_data);
	teds_strictheap *const intern = Z_STRICTHEAP_P(ZEND_THIS);
	teds_strictheap_entries *const array = &intern->array;
	if (UNEXPECTED(!teds_strictheap_entries_uninitialized(array))) {
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

	const size_t capacity = teds_strictheap_next_pow2_capacity(raw_size);
	zval *entries = safe_emalloc(capacity, sizeof(zval), 0);
	array->size = 0;
	array->capacity = capacity;
	array->entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictheap_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictMinHeap::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
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
	teds_strictheap *const intern = Z_STRICTHEAP_P(ZEND_THIS);
	teds_strictheap_entries *array = &intern->array;
	if (UNEXPECTED(!teds_strictheap_entries_uninitialized(array))) {
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

	const size_t capacity = teds_strictheap_next_pow2_capacity(raw_size);
	zval *entries = safe_emalloc(capacity, sizeof(zval), 0);
	array->size = 0;
	array->capacity = capacity;
	array->entries = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictheap_clear(intern);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictMaxHeap::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
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
	zval *const entries = array->entries;
	ZEND_ASSERT(array->size > 0);
	const uint32_t len = --array->size;
	array->should_rebuild_properties = true;
	zval *replacement = &entries[len];
	uint32_t offset = 0;


	/*    0
	 *  1   2
	 * 3 4 5 6 */
	while (true) {
		uint32_t new_offset = offset * 2 + 1;
		if (new_offset >= len) {
			break;
		}
		zval *new_entry = &entries[new_offset];
		if (new_offset + 1 < len) {
			if (teds_strictheap_should_be_above(new_entry + 1, new_entry, is_min_heap)) {
				new_offset++;
				new_entry++;
			}
		}
		if (!teds_strictheap_should_be_above(new_entry, replacement, is_min_heap)) {
			break;
		}
		//fprintf(stderr, "Copy entry %d from %d\n", offset, (int)(new_entry - entries));
		ZEND_ASSERT(Z_TYPE_P(new_entry) != IS_UNDEF);
		ZVAL_COPY_VALUE(&entries[offset], new_entry);
		offset = new_offset;
	}
	//fprintf(stderr, "Copy replacement %d to %d\n", (int)(replacement - entries), (int)offset);
	ZEND_ASSERT(Z_TYPE_P(replacement) != IS_UNDEF);
	ZVAL_COPY_VALUE(&entries[offset], replacement);
}

PHP_METHOD(Teds_StrictMinHeap, extract) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty Teds\\StrictMinHeap", 0);
		RETURN_THROWS();
	}
	RETVAL_COPY_VALUE(&array->entries[0]);
	teds_strictheap_entries_remove_top(array, true);
}

PHP_METHOD(Teds_StrictMaxHeap, extract) {
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty Teds\\StrictMaxHeap", 0);
		RETURN_THROWS();
	}
	RETVAL_COPY_VALUE(&array->entries[0]);
	teds_strictheap_entries_remove_top(array, false);
}

PHP_METHOD(Teds_StrictMinHeap, next)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot remove from empty Teds\\StrictMinHeap", 0);
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_COPY_VALUE(&tmp, &array->entries[0]);
	teds_strictheap_entries_remove_top(array, true);
	zval_ptr_dtor(&tmp);
}

PHP_METHOD(Teds_StrictMaxHeap, next)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictheap_entries *array = Z_STRICTHEAP_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot extract from empty Teds\\StrictMaxHeap", 0);
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_COPY_VALUE(&tmp, &array->entries[0]);
	teds_strictheap_entries_remove_top(array, false);
	zval_ptr_dtor(&tmp);
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

PHP_MINIT_FUNCTION(teds_strictheap)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_StrictMinHeap = register_class_Teds_StrictMinHeap(zend_ce_iterator, teds_ce_Collection);
	teds_ce_StrictMinHeap->create_object = teds_strictheap_new;

	memcpy(&teds_handler_StrictMinHeap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictMinHeap.offset          = XtOffsetOf(teds_strictheap, std);
	teds_handler_StrictMinHeap.clone_obj       = teds_strictheap_clone;
	teds_handler_StrictMinHeap.count_elements  = teds_strictheap_count_elements;
	teds_handler_StrictMinHeap.get_properties_for  = teds_strictheap_get_properties_for;
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
	teds_handler_StrictMaxHeap.get_properties_for  = teds_strictheap_get_properties_for;
	teds_handler_StrictMaxHeap.get_gc          = teds_strictheap_get_gc;
	teds_handler_StrictMaxHeap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictMaxHeap.free_obj        = teds_strictheap_free_storage;

	teds_ce_StrictMaxHeap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return SUCCESS;
}
