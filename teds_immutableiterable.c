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
#include "teds_immutableiterable_arginfo.h"
#include "teds_immutableiterable.h"
#include "teds_exceptions.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_ImmutableIterable;
zend_class_entry *teds_ce_ImmutableIterable;

typedef struct _teds_immutableiterable_entries {
	zval_pair *entries;
	uint32_t   size;
} teds_immutableiterable_entries;

typedef struct _teds_immutableiterable {
	teds_immutableiterable_entries array;
	zend_object				       std;
} teds_immutableiterable;

/* Used by InternalIterator returned by ImmutableIterable->getIterator() */
typedef struct _teds_immutableiterable_it {
	zend_object_iterator intern;
	uint32_t             current;
} teds_immutableiterable_it;

/*
 * If a size this large is encountered, assume the allocation will likely fail or
 * future changes to the capacity will overflow.
 */
static ZEND_COLD ZEND_NORETURN void teds_error_noreturn_max_immutableiterable_capacity()
{
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\ImmutableIterable capacity");
}

static zend_always_inline teds_immutableiterable *teds_immutableiterable_from_object(zend_object *obj)
{
	return (teds_immutableiterable*)((char*)(obj) - XtOffsetOf(teds_immutableiterable, std));
}

#define Z_IMMUTABLEITERABLE_P(zv)  teds_immutableiterable_from_object(Z_OBJ_P((zv)))
#define Z_IMMUTABLEITERABLE_ENTRIES_P(zv)  (&Z_IMMUTABLEITERABLE_P((zv))->array)

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then entries == NULL
 *   - if size > 0, then entries != NULL
 *   - size is not less than 0
 */
static zend_always_inline bool teds_immutableiterable_entries_empty_size(teds_immutableiterable_entries *array)
{
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->entries == empty_entry_list || array->entries == NULL);
	return true;
}

static zend_always_inline bool teds_immutableiterable_entries_uninitialized(const teds_immutableiterable_entries *array)
{
	if (array->entries == NULL) {
		ZEND_ASSERT(array->size == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->size == 0) || array->size > 0);
	return false;
}

static zend_always_inline void teds_immutableiterable_entries_set_empty_list(teds_immutableiterable_entries *array) {
	array->size = 0;
	array->entries = (zval_pair *)empty_entry_list;
}

static void teds_immutableiterable_entries_init_from_array(teds_immutableiterable_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		zval_pair *entries;
		uint32_t i = 0;

		array->size = 0; /* reset size in case emalloc() fails */
		array->entries = entries = safe_emalloc(size, sizeof(zval_pair), 0);
		array->size = size;
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
		teds_immutableiterable_entries_set_empty_list(array);
	}
}

static void teds_immutableiterable_entries_init_from_traversable(teds_immutableiterable_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->entries = NULL;
	zval_pair *entries = NULL;
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
				if (UNEXPECTED(capacity > TEDS_MAX_ZVAL_PAIR_COUNT / 2)) {
					teds_error_noreturn_max_immutableiterable_capacity();
					ZEND_UNREACHABLE();
				}
				capacity *= 2;
				entries = safe_erealloc(entries, capacity, sizeof(zval_pair), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(zval_pair), 0);
			}
		}
		/* The key's reference count was already increased */
		ZVAL_COPY_VALUE(&entries[size].key, &key);
		ZVAL_COPY_DEREF(&entries[size].value, value);
		size++;

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
	}
	if (capacity > size) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval_pair));
	}

	array->size = size;
	array->entries = entries;
cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [begin, end) into the immutableiterable, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_immutableiterable_copy_range(teds_immutableiterable_entries *array, uint32_t offset, zval_pair *begin, zval_pair *end)
{
	ZEND_ASSERT(offset <= array->size);
	ZEND_ASSERT(begin <= end);
	ZEND_ASSERT(array->size - offset >= (size_t)(end - begin));

	zval_pair *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(&to->key, &begin->key);
		ZVAL_COPY(&to->value, &begin->value);
		begin++;
		to++;
	}
}

static void teds_immutableiterable_entries_copy_ctor(teds_immutableiterable_entries *to, teds_immutableiterable_entries *from)
{
	zend_long size = from->size;
	if (!size) {
		teds_immutableiterable_entries_set_empty_list(to);
		return;
	}

	to->size = 0; /* reset size in case emalloc() fails */
	to->entries = safe_emalloc(size, sizeof(zval_pair), 0);
	to->size = size;

	zval_pair *begin = from->entries, *end = from->entries + size;
	teds_immutableiterable_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_immutableiterable_entries_dtor_range(teds_immutableiterable_entries *array, uint32_t from, uint32_t to)
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
static void teds_immutableiterable_entries_dtor(teds_immutableiterable_entries *array)
{
	if (!teds_immutableiterable_entries_empty_size(array)) {
		teds_immutableiterable_entries_dtor_range(array, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_immutableiterable_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_immutableiterable *intern = teds_immutableiterable_from_object(obj);

	*table = &intern->array.entries[0].key;
	// Each offset has 1 key and 1 value
	*n = (int)intern->array.size * 2;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

#if PHP_VERSION_ID < 80300
void teds_build_properties_for_immutable_zval_pairs(HashTable *ht, zval_pair *entries, const uint32_t len)
{
	ZEND_ASSERT(len <= TEDS_MAX_ZVAL_PAIR_COUNT);

	/* Initialize properties array */
	for (uint32_t i = 0; i < len; i++) {
		zval tmp;
		Z_TRY_ADDREF_P(&entries[i].key);
		Z_TRY_ADDREF_P(&entries[i].value);
		ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
		zend_hash_index_update(ht, i, &tmp);
	}
#if PHP_VERSION_ID >= 80200
	if (HT_IS_PACKED(ht)) {
		/* Engine doesn't expect packed array */
		zend_hash_packed_to_hash(ht);
	}
#endif
}
#endif

static HashTable* teds_immutableiterable_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	teds_immutableiterable_entries *array = &teds_immutableiterable_from_object(obj)->array;

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
			/* When modifying the hash table, the reference count should be 1. Increase refcount after modifying. */
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

static void teds_immutableiterable_free_storage(zend_object *object)
{
	teds_immutableiterable *intern = teds_immutableiterable_from_object(object);
	teds_immutableiterable_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_immutableiterable_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_immutableiterable *intern;

	intern = zend_object_alloc(sizeof(teds_immutableiterable), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_ImmutableIterable);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_ImmutableIterable;

	if (orig && clone_orig) {
		teds_immutableiterable *other = teds_immutableiterable_from_object(orig);
		teds_immutableiterable_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_immutableiterable_new(zend_class_entry *class_type)
{
	return teds_immutableiterable_new_ex(class_type, NULL, 0);
}


static zend_object *teds_immutableiterable_clone(zend_object *old_object)
{
	zend_object *new_object = teds_immutableiterable_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static TEDS_COUNT_ELEMENTS_RETURN_TYPE teds_immutableiterable_count_elements(zend_object *object, zend_long *count)
{
	const teds_immutableiterable *intern = teds_immutableiterable_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this ImmutableIterable */
PHP_METHOD(Teds_ImmutableIterable, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this ImmutableIterable is empty. */
PHP_METHOD(Teds_ImmutableIterable, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_ImmutableIterable, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(object);

	if (UNEXPECTED(!teds_immutableiterable_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\ImmutableIterable::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_immutableiterable_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_immutableiterable_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

ZEND_COLD PHP_METHOD(Teds_ImmutableIterable, clear)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	TEDS_THROW_UNSUPPORTEDOPERATIONEXCEPTION("Teds\\ImmutableIterable does not support clear - it is immutable");
}


PHP_METHOD(Teds_ImmutableIterable, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_immutableiterable_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_immutableiterable_it_rewind(zend_object_iterator *iter)
{
	((teds_immutableiterable_it*)iter)->current = 0;
}

static int teds_immutableiterable_it_valid(zend_object_iterator *iter)
{
	teds_immutableiterable_it     *iterator = (teds_immutableiterable_it*)iter;
	teds_immutableiterable *object   = Z_IMMUTABLEITERABLE_P(&iter->data);

	if (iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval_pair *teds_immutableiterable_read_offset_helper(teds_immutableiterable *intern, uint32_t offset)
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

static zval *teds_immutableiterable_it_get_current_data(zend_object_iterator *iter)
{
	teds_immutableiterable_it     *iterator = (teds_immutableiterable_it*)iter;
	teds_immutableiterable *object   = Z_IMMUTABLEITERABLE_P(&iter->data);

	zval_pair *data = teds_immutableiterable_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_immutableiterable_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_immutableiterable_it     *iterator = (teds_immutableiterable_it*)iter;
	teds_immutableiterable *object   = Z_IMMUTABLEITERABLE_P(&iter->data);

	zval_pair *data = teds_immutableiterable_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_immutableiterable_it_move_forward(zend_object_iterator *iter)
{
	((teds_immutableiterable_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_immutableiterable_it_funcs = {
	teds_immutableiterable_it_dtor,
	teds_immutableiterable_it_valid,
	teds_immutableiterable_it_get_current_data,
	teds_immutableiterable_it_get_current_key,
	teds_immutableiterable_it_move_forward,
	teds_immutableiterable_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_immutableiterable_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_immutableiterable_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_immutableiterable_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_immutableiterable_it_funcs;

	(void) ce;

	return &iterator->intern;
}

PHP_METHOD(Teds_ImmutableIterable, __unserialize)
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
	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	if (UNEXPECTED(!teds_immutableiterable_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	ZEND_ASSERT(intern->array.entries == NULL);

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
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\ImmutableIterable::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}
		ZVAL_COPY_DEREF(it++, val);
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it == &entries[0].key + raw_size);

	intern->array.size = num_entries;
	intern->array.entries = entries;
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


static void teds_immutableiterable_entries_init_from_array_pairs(teds_immutableiterable_entries *array, zend_array *raw_data)
{
	uint32_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->entries = NULL;
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
}

static void teds_immutableiterable_entries_init_from_traversable_pairs(teds_immutableiterable_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->entries = NULL;
	zval_pair *entries = NULL;
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

		if (size >= capacity) {
			/* TODO: Could use countable and get_count handler to estimate the size of the array to allocate */
			if (entries) {
				capacity *= 2;
				entries = safe_erealloc(entries, capacity, sizeof(zval_pair), 0);
			} else {
				capacity = 4;
				entries = safe_emalloc(capacity, sizeof(zval_pair), 0);
			}
		}
		if (!teds_cached_entry_copy_entry_from_array_pair(&entries[size], pair)) {
			break;
		}
		size++;

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}
	if (capacity > size) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval_pair));
	}

	array->size = size;
	array->entries = entries;
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static zend_object* create_from_pairs(zval *iterable) {
	zend_object *object = teds_immutableiterable_new(teds_ce_ImmutableIterable);
	teds_immutableiterable *intern = teds_immutableiterable_from_object(object);
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_immutableiterable_entries_init_from_array_pairs(&intern->array, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_immutableiterable_entries_init_from_traversable_pairs(&intern->array, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_ImmutableIterable, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_ImmutableIterable, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_immutableiterable_new(teds_ce_ImmutableIterable);
	teds_immutableiterable *intern = teds_immutableiterable_from_object(object);
	teds_immutableiterable_entries_init_from_array_pairs(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_ImmutableIterable, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);

	if (teds_immutableiterable_entries_empty_size(&intern->array)) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = intern->array.entries;
	uint32_t len = intern->array.size;
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

PHP_METHOD(Teds_ImmutableIterable, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = intern->array.entries;
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

PHP_METHOD(Teds_ImmutableIterable, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval_pair *entries = intern->array.entries;
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

PHP_METHOD(Teds_ImmutableIterable, keyAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		teds_throw_invalid_collection_offset_exception();
		RETURN_THROWS();
	}
	RETURN_COPY(&intern->array.entries[offset].key);
}

PHP_METHOD(Teds_ImmutableIterable, valueAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		teds_throw_invalid_collection_offset_exception();
		RETURN_THROWS();
	}
	RETURN_COPY(&intern->array.entries[offset].value);
}

PHP_METHOD(Teds_ImmutableIterable, indexOfKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(key, &entries[i].key)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_ImmutableIterable, indexOfValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(value, &entries[i].value)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_ImmutableIterable, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(key, &entries[i].key)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_ImmutableIterable, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(value, &entries[i].value)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

zend_array *teds_zval_pairs_to_refcounted_pairs(zval_pair *entries, uint32_t len)
{
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
	return values;
}


static void teds_immutableiterable_return_pairs(zval *return_value, teds_immutableiterable *intern)
{
	const uint32_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	zval_pair *entries = intern->array.entries;
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

PHP_METHOD(Teds_ImmutableIterable, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_immutableiterable *intern = Z_IMMUTABLEITERABLE_P(ZEND_THIS);
	teds_immutableiterable_return_pairs(return_value, intern);
}

PHP_METHOD(Teds_ImmutableIterable, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_immutableiterable_entries *array = Z_IMMUTABLEITERABLE_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	zval_pair *entries = array->entries;
	zend_array *values = zend_new_array(len);

	/* Go through values and add values to the return array */
	for (uint32_t i = 0; i < len; i++) {
		/* This is immutable and should not need the reference counting check - the ImmutableIterable keeps at least one reference. */
		array_set_zval_key(values, &entries[i].key, &entries[i].value);
		if (UNEXPECTED(EG(exception))) {
			zend_array_destroy(values);
			RETURN_THROWS();
		}
	}
	RETURN_ARR(values);
}

PHP_MINIT_FUNCTION(teds_immutableiterable)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_ImmutableIterable = register_class_Teds_ImmutableIterable(zend_ce_aggregate, teds_ce_Collection, php_json_serializable_ce);
	teds_ce_ImmutableIterable->create_object = teds_immutableiterable_new;

	memcpy(&teds_handler_ImmutableIterable, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_ImmutableIterable.offset          = XtOffsetOf(teds_immutableiterable, std);
	teds_handler_ImmutableIterable.clone_obj       = teds_immutableiterable_clone;
	teds_handler_ImmutableIterable.count_elements  = teds_immutableiterable_count_elements;
	teds_handler_ImmutableIterable.get_properties_for = teds_immutableiterable_get_properties_for;
	teds_handler_ImmutableIterable.get_gc          = teds_immutableiterable_get_gc;
	teds_handler_ImmutableIterable.dtor_obj        = zend_objects_destroy_object;
	teds_handler_ImmutableIterable.free_obj        = teds_immutableiterable_free_storage;

	teds_ce_ImmutableIterable->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_ImmutableIterable->get_iterator = teds_immutableiterable_get_iterator;

	return SUCCESS;
}
