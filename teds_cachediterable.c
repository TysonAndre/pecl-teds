/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c. Instead of a C array of zvals, this is based on a C array of pairs of zvals for key-value entries */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds.h"
#include "teds_util.h"
#include "teds_interfaces.h"
#include "teds_cachediterable_arginfo.h"
#include "teds_cachediterable.h"
#include "teds_exceptions.h"
#include "teds_immutableiterable.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_CachedIterable;
zend_class_entry *teds_ce_CachedIterable;

typedef struct _teds_cachediterable_entries {
	zval_pair            *entries;
	uint32_t              size;
	uint32_t              capacity;
	zend_object_iterator *iter; /* Null if this is done evaluating the Traversable*/
	bool                  end_exception;
	bool                  is_calling_inner;
} teds_cachediterable_entries;

typedef struct _teds_cachediterable {
	teds_cachediterable_entries		array;
	zend_object				std;
} teds_cachediterable;

/* Used by InternalIterator returned by CachedIterable->getIterator() */
typedef struct _teds_cachediterable_it {
	zend_object_iterator intern;
	uint32_t             current;
} teds_cachediterable_it;

/*
 * If a size this large is encountered, assume the allocation will likely fail or
 * future changes to the capacity will overflow.
 */
static ZEND_COLD ZEND_NORETURN void teds_error_noreturn_max_cachediterable_capacity()
{
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\CachedIterable capacity");
}

static zend_always_inline teds_cachediterable *teds_cachediterable_from_object(zend_object *obj)
{
	return (teds_cachediterable*)((char*)(obj) - XtOffsetOf(teds_cachediterable, std));
}

#define Z_CACHEDITERABLE_P(zv)  teds_cachediterable_from_object(Z_OBJ_P((zv)))
#define Z_CACHEDITERABLE_ENTRIES_P(zv)  (&Z_CACHEDITERABLE_P((zv))->array)

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then entries == NULL
 *   - if size > 0, then entries != NULL
 *   - size is not less than 0
 */
static zend_always_inline bool teds_cachediterable_entries_empty_size(teds_cachediterable_entries *array)
{
	ZEND_ASSERT(array->capacity >= array->size);
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list && array->entries != NULL);
		return false;
	}
	return true;
}

static zend_always_inline bool teds_cachediterable_entries_uninitialized(const teds_cachediterable_entries *array)
{
	if (array->entries == NULL) {
		ZEND_ASSERT(array->size == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->size == 0) || array->size > 0);
	return false;
}

static zend_always_inline void teds_cachediterable_entries_set_empty_list(teds_cachediterable_entries *array) {
	array->size = 0;
	array->capacity = 0;
	array->entries = (zval_pair *)empty_entry_list;
}

static void teds_cachediterable_entries_init_from_array(teds_cachediterable_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		zval_pair *entries;
		uint32_t i = 0;

		array->entries = entries = safe_emalloc(size, sizeof(zval_pair), 0);
		array->size = size;
		array->capacity = size;
		ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
			ZEND_ASSERT(i < size);
			if (skey) {
				ZVAL_STR_COPY(&entries[i].key, skey);
			} else {
				ZVAL_LONG(&entries[i].key, nkey);
			}
			ZVAL_COPY_DEREF(&entries[i].value, val);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		teds_cachediterable_entries_set_empty_list(array);
	}
	ZEND_ASSERT(array->end_exception == false);
	ZEND_ASSERT(array->iter == NULL);
}

static void teds_cachediterable_entries_free_inner_traversable(teds_cachediterable_entries *array)
{
	zend_object_iterator *iter = array->iter;
	array->iter = NULL;
	ZEND_ASSERT(iter);
	zend_iterator_dtor(iter);
	if (array->capacity > array->size) {
		array->capacity = array->size;
		/* Shrink allocated value to actual required size */
		array->entries = erealloc(array->entries, array->size * sizeof(zval_pair));
	}
}

static void teds_cachediterable_entries_set_exception(teds_cachediterable_entries *array)
{
	ZEND_ASSERT(!array->end_exception);
	ZEND_ASSERT(array->is_calling_inner);
	array->end_exception = true;
	teds_cachediterable_entries_free_inner_traversable(array);
	array->is_calling_inner = false;
}

static void teds_cachediterable_entries_throw_end_exception(teds_cachediterable_entries *array)
{
	ZEND_ASSERT(array->end_exception);
	zend_throw_exception(spl_ce_RuntimeException, "Teds\\CachedIterable failed due to exception from underlying Traversable", 0);
}

static bool teds_cachediterable_entries_lazy_fetch_next(teds_cachediterable_entries *array)
{
	ZEND_ASSERT(array->iter);
	if (UNEXPECTED(array->end_exception)) {
		teds_cachediterable_entries_throw_end_exception(array);
		return false;
	}
	if (UNEXPECTED(array->is_calling_inner)) {
		zend_throw_exception(spl_ce_RuntimeException, "Teds\\CachedIterable reentry detected while already fetching next element", 0);
		return false;
	}
	array->is_calling_inner = true;

	zend_object_iterator *iter = array->iter;
	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (array->size > 0) {
		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
handle_exception:
			teds_cachediterable_entries_set_exception(array);
			array->is_calling_inner = false;
			return false;
		}
	}
	if (funcs->valid(iter) == SUCCESS) {
		if (UNEXPECTED(EG(exception))) {
			goto handle_exception;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			goto handle_exception;
		}
		zval key;
		if (funcs->get_current_key) {
			funcs->get_current_key(iter, &key);
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor(&key);
				goto handle_exception;
			}
		} else {
			ZVAL_NULL(&key);
		}

		const uint32_t size = array->size;
		if (size >= array->capacity) {
			ZEND_ASSERT(size == array->capacity);
			/* TODO: Could use countable and get_count handler to estimate the size of the array to allocate */
			if (size) {
				ZEND_ASSERT(array->entries != empty_entry_list && array->entries != NULL);
				if (UNEXPECTED(array->capacity > TEDS_MAX_ZVAL_PAIR_COUNT / 2)) {
					teds_error_noreturn_max_cachediterable_capacity();
					ZEND_UNREACHABLE();
				}
				const uint32_t capacity = array->capacity * 2;
				array->entries = safe_erealloc(array->entries, capacity, sizeof(zval_pair), 0);
				array->capacity = capacity;
			} else {
				ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
				array->capacity = 4;
				array->entries = emalloc(4 * sizeof(zval_pair));
			}
		}
		zval_pair *entries = array->entries;
		/* The key's reference count was already increased */
		ZVAL_COPY_VALUE(&entries[size].key, &key);
		ZVAL_COPY_DEREF(&entries[size].value, value);
		array->size++;
		array->is_calling_inner = false;
		return true;
	}
	array->is_calling_inner = false;
	teds_cachediterable_entries_free_inner_traversable(array);
	return false;
}

static zend_always_inline void teds_cachediterable_entries_fetch_all(teds_cachediterable_entries *array)
{
	while (array->iter) {
		teds_cachediterable_entries_lazy_fetch_next(array);
	}
}

static zend_always_inline bool teds_cachediterable_entries_ensure_offset_exists(teds_cachediterable_entries *array, size_t offset)
{
	while (array->iter) {
		if (!teds_cachediterable_entries_lazy_fetch_next(array)) {
			ZEND_ASSERT(!array->iter || EG(exception));
			return false;
		}
		if (array->size > offset) {
			return true;
		}
	}
	return false;
}

#define TEDS_CACHEDITERABLE_ENTRIES_FETCH_ALL_OR_THROW(array) do { \
	if ((array)->iter) { \
		teds_cachediterable_entries_fetch_all((array)); \
		if (UNEXPECTED(EG(exception))) { \
			RETURN_THROWS(); \
		} \
	} else if (UNEXPECTED((array)->end_exception)) {  \
		teds_cachediterable_entries_throw_end_exception((array)); \
	} \
} while(0)

static void teds_cachediterable_entries_init_from_traversable(teds_cachediterable_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_cachediterable_entries_set_empty_list(array);
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
			zend_iterator_dtor(iter);
			return;
		}
	}
	array->iter = iter;
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_cachediterable_entries_dtor_range(teds_cachediterable_entries *array, uint32_t from, uint32_t to)
{
	zval_pair *begin = array->entries + from, *end = array->entries + to;
	while (begin != end) {
		zval_ptr_dtor(&begin->key);
		zval_ptr_dtor(&begin->value);
		begin++;
	}
}

/* Destructs and frees contents but not the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_cachediterable_entries_dtor(teds_cachediterable_entries *array)
{
	if (array->iter) {
		teds_cachediterable_entries_free_inner_traversable(array);
	}
	if (!teds_cachediterable_entries_empty_size(array)) {
		teds_cachediterable_entries_dtor_range(array, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_cachediterable_get_gc(zend_object *obj, zval **table, int *table_count)
{
	teds_cachediterable_entries *const array = &teds_cachediterable_from_object(obj)->array;
	zval_pair *const entries = array->entries;
	const uint32_t len = array->size;

	if (array->iter) {
		zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
		for (uint32_t i = 0; i < len; i++) {
			zend_get_gc_buffer_add_zval(gc_buffer, &entries[i].key);
			zend_get_gc_buffer_add_zval(gc_buffer, &entries[i].value);
		}
		zend_object *traversable = &array->iter->std;
		zend_get_gc_buffer_add_obj(gc_buffer, traversable);
		zend_get_gc_buffer_use(gc_buffer, table, table_count);
	} else {
		*table = &array->entries[0].key;
		// Each offset has 1 key and 1 value
		*table_count = (int)(len * 2);
	}

	return obj->properties;
}

static HashTable* teds_cachediterable_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	teds_cachediterable_entries *array = &teds_cachediterable_from_object(obj)->array;

	/* Fetch all of the elements so that properties are only built once and var_export shows the full state.
	 * If this threw already, stop there without throwing. */
	teds_cachediterable_entries_fetch_all(array);

	const uint32_t len = array->size;
	if (!len) {
		/* Nothing to add or remove - this is immutable. */
		/* debug_zval_dump DEBUG purpose requires null or a refcounted array. */
		return NULL;
	}
	switch (purpose) {
		case ZEND_PROP_PURPOSE_JSON: /* jsonSerialize and get_properties() is used instead. */
			ZEND_UNREACHABLE();
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_DEBUG:
#if PHP_VERSION_ID < 80300
		/* In php 8.3, var_export and debug_zval_dump now check for infinite recursion on the object */
		{
			HashTable *ht = zend_std_get_properties(obj);
			if (zend_hash_num_elements(ht)) {
				GC_TRY_ADDREF(ht);
				/* Already built. This is immutable there is no need to rebuild it. */
				return ht;
			}
			teds_build_properties_for_immutable_zval_pairs(ht, array->entries, len);
			/* When modifying the hash table, the reference count should be 1 */
			GC_TRY_ADDREF(ht);
			return ht;
		}
#endif
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
		case ZEND_PROP_PURPOSE_SERIALIZE:
			return teds_zval_pairs_to_refcounted_pairs(array->entries, len);
		default:
			ZEND_UNREACHABLE();
			return NULL;
	}
}

static void teds_cachediterable_free_storage(zend_object *object)
{
	teds_cachediterable *intern = teds_cachediterable_from_object(object);
	teds_cachediterable_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_cachediterable_new(zend_class_entry *class_type)
{
	teds_cachediterable *intern;

	intern = zend_object_alloc(sizeof(teds_cachediterable), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_CachedIterable);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_CachedIterable;

	intern->array.entries = NULL;

	return &intern->std;
}


TEDS_COUNT_ELEMENTS_RETURN_TYPE teds_size_t_count_elements(zend_object *object, zend_long *count)
{
	const teds_cachediterable *intern = teds_cachediterable_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this CachedIterable */
PHP_METHOD(Teds_CachedIterable, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(object);
	TEDS_CACHEDITERABLE_ENTRIES_FETCH_ALL_OR_THROW(array);
	RETURN_LONG(array->size);
}

/* Get whether this CachedIterable is empty. */
PHP_METHOD(Teds_CachedIterable, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(object);
	if (array->size > 0) {
		RETURN_FALSE;
	}
	if (UNEXPECTED(array->end_exception)) {
		teds_cachediterable_entries_throw_end_exception(array);
		RETURN_THROWS();
	}
	if (!array->iter) {
		RETURN_TRUE;
	}
	const bool fetched = teds_cachediterable_entries_lazy_fetch_next(array);
	RETURN_BOOL(!fetched);
}

/* Create this from an iterable */
PHP_METHOD(Teds_CachedIterable, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_cachediterable *intern = Z_CACHEDITERABLE_P(object);

	if (UNEXPECTED(!teds_cachediterable_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\CachedIterable::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_cachediterable_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_cachediterable_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

ZEND_COLD PHP_METHOD(Teds_CachedIterable, clear)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	TEDS_THROW_UNSUPPORTEDOPERATIONEXCEPTION("Teds\\CachedIterable does not support clear - it is immutable");
}


PHP_METHOD(Teds_CachedIterable, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_cachediterable_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_cachediterable_it_rewind(zend_object_iterator *iter)
{
	((teds_cachediterable_it*)iter)->current = 0;
}

static int teds_cachediterable_it_valid(zend_object_iterator *iter)
{
	teds_cachediterable_it     *iterator = (teds_cachediterable_it*)iter;
	teds_cachediterable_entries *array   = Z_CACHEDITERABLE_ENTRIES_P(&iter->data);

	if (iterator->current < array->size) {
		return SUCCESS;
	}
	if (!array->iter) {
		return FAILURE;
	}
	if (!teds_cachediterable_entries_ensure_offset_exists(array, iterator->current)) {
		return FAILURE;
	}
	if (iterator->current < array->size) {
		return SUCCESS;
	}

	return SUCCESS;
}

static zval_pair *teds_cachediterable_read_offset_helper(teds_cachediterable *intern, uint32_t offset)
{
	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (UNEXPECTED(offset >= intern->array.size)) {
		teds_throw_invalid_collection_offset_exception();
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

static zval *teds_cachediterable_it_get_current_data(zend_object_iterator *iter)
{
	teds_cachediterable_it     *iterator = (teds_cachediterable_it*)iter;
	teds_cachediterable *object   = Z_CACHEDITERABLE_P(&iter->data);

	zval_pair *data = teds_cachediterable_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_cachediterable_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_cachediterable_it     *iterator = (teds_cachediterable_it*)iter;
	teds_cachediterable *object   = Z_CACHEDITERABLE_P(&iter->data);

	zval_pair *data = teds_cachediterable_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_cachediterable_it_move_forward(zend_object_iterator *iter)
{
	((teds_cachediterable_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_cachediterable_it_funcs = {
	teds_cachediterable_it_dtor,
	teds_cachediterable_it_valid,
	teds_cachediterable_it_get_current_data,
	teds_cachediterable_it_get_current_key,
	teds_cachediterable_it_move_forward,
	teds_cachediterable_it_rewind,
	NULL,
	teds_internaliterator_get_gc, /* get_gc */
};


zend_object_iterator *teds_cachediterable_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_cachediterable_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_cachediterable_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_cachediterable_it_funcs;

	(void) ce;

	return &iterator->intern;
}

PHP_METHOD(Teds_CachedIterable, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	const uint32_t raw_size = zend_hash_num_elements(raw_data);
	if (UNEXPECTED(raw_size % 2 != 0)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Odd number of elements", 0);
		RETURN_THROWS();
	}
	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!teds_cachediterable_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	ZEND_ASSERT(array->entries == NULL);

	const uint32_t num_entries = raw_size / 2;
	zval_pair *entries = safe_emalloc(num_entries, sizeof(zval_pair), 0);
	zval *it = &entries[0].key;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			for (zval *deleteIt = &entries[0].key; deleteIt < it; deleteIt++) {
				zval_ptr_dtor_nogc(deleteIt);
			}
			efree(entries);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\CachedIterable::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}
		ZVAL_COPY_DEREF(it++, val);
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it == &entries[0].key + raw_size);

	array->size = num_entries;
	array->capacity = num_entries;
	array->entries = entries;
}

static bool teds_cached_entry_copy_entry_from_array_pair(zval_pair *pair, zval *raw_val)
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
	ZVAL_COPY_DEREF(&pair->key, key);
	ZVAL_COPY_DEREF(&pair->value, value);
	return true;
}


static void teds_cachediterable_entries_init_from_array_pairs(teds_cachediterable_entries *array, zend_array *raw_data)
{
	uint32_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		teds_cachediterable_entries_set_empty_list(array);
		return;
	}
	zval_pair *entries = safe_emalloc(num_entries, sizeof(zval_pair), 0);
	uint32_t actual_size = 0;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		if (!teds_cached_entry_copy_entry_from_array_pair(&entries[actual_size], val)) {
			break;
		}
		actual_size++;
	} ZEND_HASH_FOREACH_END();

	ZEND_ASSERT(actual_size <= num_entries);
	if (UNEXPECTED(!actual_size)) {
		efree(entries);
		entries = NULL;
	}

	array->entries = entries;
	array->size = actual_size;
	array->capacity = num_entries;
}

PHP_METHOD(Teds_CachedIterable, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_cachediterable_new(teds_ce_CachedIterable);
	teds_cachediterable *intern = teds_cachediterable_from_object(object);
	teds_cachediterable_entries_init_from_array_pairs(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_CachedIterable, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);

	TEDS_CACHEDITERABLE_ENTRIES_FETCH_ALL_OR_THROW(array);

	if (teds_cachediterable_entries_empty_size(array)) {
		RETURN_EMPTY_ARRAY();
	}

	zval_pair *entries = array->entries;
	uint32_t len = array->size;
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	ZEND_HASH_FILL_PACKED(flat_entries_array) {
		for (uint32_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].key;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
			tmp = &entries[i].value;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	/* Unlike FixedArray, there's no setSize, so there's no reason to delete indexes */

	RETURN_ARR(flat_entries_array);
}

PHP_METHOD(Teds_CachedIterable, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	TEDS_CACHEDITERABLE_ENTRIES_FETCH_ALL_OR_THROW(array);
	const uint32_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = array->entries;
	zend_array *keys = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(keys);

	/* Go through keys and add values to the return array */
	ZEND_HASH_FILL_PACKED(keys) {
		for (uint32_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].key;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(keys);
}

PHP_METHOD(Teds_CachedIterable, values)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);

	TEDS_CACHEDITERABLE_ENTRIES_FETCH_ALL_OR_THROW(array);

	uint32_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = array->entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (uint32_t i = 0; i < len; i++) {
			zval *tmp = &entries[i].value;
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_CachedIterable, keyAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if ((zend_ulong) offset >= len) {
		if (!teds_cachediterable_entries_ensure_offset_exists(array, (size_t)offset)) {
			if (!EG(exception)) {
				teds_throw_invalid_collection_offset_exception();
			}
			RETURN_THROWS();
		}
	}
	RETURN_COPY(&array->entries[offset].key);
}

PHP_METHOD(Teds_CachedIterable, valueAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if ((zend_ulong) offset >= len) {
		if (!teds_cachediterable_entries_ensure_offset_exists(array, (size_t)offset)) {
			if (!EG(exception)) {
				teds_throw_invalid_collection_offset_exception();
			}
			RETURN_THROWS();
		}
	}
	RETURN_COPY(&array->entries[offset].value);
}

PHP_METHOD(Teds_CachedIterable, indexOfKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	uint32_t i = 0;
	{
		const uint32_t len = array->size;
		zval_pair *entries = array->entries;
		for (; i < len; i++) {
			if (teds_is_identical_inline(key, &entries[i].key)) {
				RETURN_LONG(i);
			}
		}
	}
	while (array->iter) {
		if (!teds_cachediterable_entries_lazy_fetch_next(array)) {
			break;
		}
		ZEND_ASSERT(i + 1 == array->size);
		if (teds_is_identical_inline(key, &array->entries[i].key)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_CachedIterable, indexOfValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	uint32_t i = 0;
	{
		const uint32_t len = array->size;
		zval_pair *entries = array->entries;
		for (; i < len; i++) {
			if (teds_is_identical_inline(value, &entries[i].value)) {
				RETURN_LONG(i);
			}
		}
	}
	while (array->iter) {
		if (!teds_cachediterable_entries_lazy_fetch_next(array)) {
			break;
		}
		ZEND_ASSERT(i + 1 == array->size);
		if (teds_is_identical_inline(value, &array->entries[i].value)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_CachedIterable, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	uint32_t i = 0;
	{
		const uint32_t len = array->size;
		zval_pair *entries = array->entries;
		for (; i < len; i++) {
			if (teds_is_identical_inline(key, &entries[i].key)) {
				RETURN_TRUE;
			}
		}
	}
	while (array->iter) {
		if (!teds_cachediterable_entries_lazy_fetch_next(array)) {
			break;
		}
		ZEND_ASSERT(i + 1 == array->size);
		if (teds_is_identical_inline(key, &array->entries[i].key)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_CachedIterable, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);
	uint32_t i = 0;
	{
		const uint32_t len = array->size;
		zval_pair *entries = array->entries;
		for (; i < len; i++) {
			if (teds_is_identical_inline(value, &entries[i].value)) {
				RETURN_TRUE;
			}
		}
	}
	while (array->iter) {
		if (!teds_cachediterable_entries_lazy_fetch_next(array)) {
			break;
		}
		ZEND_ASSERT(i + 1 == array->size);
		if (teds_is_identical_inline(value, &array->entries[i].value)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

static void teds_cachediterable_entries_return_pairs(zval *return_value, teds_cachediterable_entries *array)
{
	const uint32_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	TEDS_CACHEDITERABLE_ENTRIES_FETCH_ALL_OR_THROW(array);

	zval_pair *entries = array->entries;
	RETURN_ARR(teds_zval_pairs_to_refcounted_pairs(entries, len));
}

PHP_METHOD(Teds_CachedIterable, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_cachediterable_entries_return_pairs(return_value, Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS));
}

PHP_METHOD(Teds_CachedIterable, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_cachediterable_entries *array = Z_CACHEDITERABLE_ENTRIES_P(ZEND_THIS);

	TEDS_CACHEDITERABLE_ENTRIES_FETCH_ALL_OR_THROW(array);

	const uint32_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	zval_pair *entries = array->entries;
	zend_array *values = zend_new_array(len);

	/* Go through values and add values to the return array */
	for (uint32_t i = 0; i < len; i++) {
		/* This is immutable and should not need the reference counting check - the CachedIterable keeps at least one reference. */
		array_set_zval_key(values, &entries[i].key, &entries[i].value);
		if (UNEXPECTED(EG(exception))) {
			zend_array_destroy(values);
			RETURN_THROWS();
		}
	}
	RETURN_ARR(values);
}

PHP_MINIT_FUNCTION(teds_cachediterable)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_CachedIterable = register_class_Teds_CachedIterable(zend_ce_aggregate, teds_ce_Collection, php_json_serializable_ce);
	teds_ce_CachedIterable->create_object = teds_cachediterable_new;

	memcpy(&teds_handler_CachedIterable, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_CachedIterable.offset          = XtOffsetOf(teds_cachediterable, std);
	teds_handler_CachedIterable.clone_obj       = NULL;
	teds_handler_CachedIterable.count_elements  = teds_size_t_count_elements;
	teds_handler_CachedIterable.get_properties_for = teds_cachediterable_get_properties_for;
	teds_handler_CachedIterable.get_gc          = teds_cachediterable_get_gc;
	teds_handler_CachedIterable.dtor_obj        = zend_objects_destroy_object;
	teds_handler_CachedIterable.free_obj        = teds_cachediterable_free_storage;

	teds_ce_CachedIterable->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_CachedIterable->get_iterator = teds_cachediterable_get_iterator;

	return SUCCESS;
}
