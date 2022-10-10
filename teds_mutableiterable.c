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
#include "teds_mutableiterable_arginfo.h"
#include "teds_mutableiterable.h"
#include "teds_immutableiterable.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"
#include "teds_interfaces.h"
#include "teds_exceptions.h"

#include <stdbool.h>

zend_object_handlers teds_handler_MutableIterable;
zend_class_entry *teds_ce_MutableIterable;

/*
 * If a size this large is encountered, assume the allocation will likely fail or
 * future changes to the capacity will overflow.
 */
static ZEND_COLD ZEND_NORETURN void teds_error_noreturn_max_mutableiterable_capacity()
{
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\MutableIterable capacity");
}

static zend_always_inline teds_mutableiterable *teds_mutableiterable_from_object(zend_object *obj)
{
	return (teds_mutableiterable*)((char*)(obj) - XtOffsetOf(teds_mutableiterable, std));
}

static zend_always_inline zend_object *teds_mutableiterable_to_object(teds_mutableiterable_entries *array)
{
	return (zend_object*)((char*)(array) + XtOffsetOf(teds_mutableiterable, std));
}

static zend_always_inline teds_mutableiterable_it *teds_mutableiterable_it_from_node(teds_intrusive_dllist_node *node)
{
	return (teds_mutableiterable_it*)((char*)(node) - XtOffsetOf(teds_mutableiterable_it, dllist_node));
}

#define Z_MUTABLEITERABLE_P(zv)  teds_mutableiterable_from_object(Z_OBJ_P((zv)))
#define Z_MUTABLEITERABLE_ENTRIES_P(zv)  (&Z_MUTABLEITERABLE_P((zv))->array)

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then entries == NULL
 *   - if size > 0, then entries != NULL
 *   - size is not less than 0
 */
static zend_always_inline bool teds_mutableiterable_entries_empty_size(teds_mutableiterable_entries *array)
{
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		ZEND_ASSERT(array->capacity >= array->size);
		return false;
	}
	return true;
}

static zend_always_inline bool teds_mutableiterable_entries_empty_capacity(teds_mutableiterable_entries *array)
{
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	return true;
}

static zend_always_inline bool teds_mutableiterable_entries_uninitialized(const teds_mutableiterable_entries *array)
{
	if (array->entries == NULL) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static void teds_mutableiterable_raise_capacity(teds_mutableiterable *intern, const uint32_t new_capacity) {
	if (UNEXPECTED(new_capacity > TEDS_MAX_ZVAL_PAIR_COUNT)) {
		teds_error_noreturn_max_mutableiterable_capacity();
		ZEND_UNREACHABLE();
	}
	ZEND_ASSERT(new_capacity > intern->array.capacity);
	if (intern->array.capacity == 0) {
		intern->array.entries = safe_emalloc(new_capacity, sizeof(zval_pair), 0);
	} else {
		intern->array.entries = safe_erealloc(intern->array.entries, new_capacity, sizeof(zval_pair), 0);
	}
	intern->array.capacity = new_capacity;
	ZEND_ASSERT(intern->array.entries != NULL);
}

static void teds_mutableiterable_shrink_capacity(teds_mutableiterable_entries *array, uint32_t size, uint32_t capacity, zval_pair *old_entries) {
	ZEND_ASSERT(size <= capacity);
	ZEND_ASSERT(size == array->size);
	ZEND_ASSERT(capacity > 0);
	ZEND_ASSERT(capacity < array->capacity);
	ZEND_ASSERT(old_entries == array->entries);
	array->capacity = capacity;
	array->entries = erealloc2(old_entries, capacity * sizeof(zval_pair), size * sizeof(zval_pair));
	ZEND_ASSERT(array->entries != NULL);
}

static zend_always_inline void teds_mutableiterable_entries_set_empty_list(teds_mutableiterable_entries *array) {
	array->size = 0;
	array->capacity = 0;
	array->entries = (zval_pair *)empty_entry_list;
}

static void teds_mutableiterable_entries_init_from_array(teds_mutableiterable_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		zval_pair *entries;
		int i = 0;

		array->size = 0; /* reset size in case emalloc() fails */
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
		teds_mutableiterable_entries_set_empty_list(array);
	}
}

static void teds_mutableiterable_entries_init_from_traversable(teds_mutableiterable_entries *array, zend_object *obj)
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
	array->capacity = size;
	array->entries = entries;
cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [begin, end) into the mutableiterable, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_mutableiterable_copy_range(teds_mutableiterable_entries *array, uint32_t offset, zval_pair *begin, zval_pair *end)
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

static void teds_mutableiterable_entries_copy_ctor(teds_mutableiterable_entries *to, teds_mutableiterable_entries *from)
{
	zend_long size = from->size;
	if (!size) {
		teds_mutableiterable_entries_set_empty_list(to);
		return;
	}

	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0;
	to->entries = safe_emalloc(size, sizeof(zval_pair), 0);
	to->size = size;
	to->capacity = size;

	zval_pair *begin = from->entries, *end = from->entries + size;
	teds_mutableiterable_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_mutableiterable_entries_dtor_range(teds_mutableiterable_entries *array, uint32_t from, uint32_t to)
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
static void teds_mutableiterable_entries_dtor(teds_mutableiterable_entries *array)
{
	if (!teds_mutableiterable_entries_empty_capacity(array)) {
		teds_mutableiterable_entries_dtor_range(array, 0, array->size);
		efree(array->entries);
	}
}

HashTable* teds_mutableiterable_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_mutableiterable *intern = teds_mutableiterable_from_object(obj);

	*table = &intern->array.entries[0].key;
	/* Each offset has 1 key and 1 value */
	*n = (int)intern->array.size * 2;

	/* This is mutable. In php 8.2 and older this might have properties from var_export that were removed later. */
	return obj->properties;
}

#if PHP_VERSION_ID < 80300
void teds_build_properties_for_mutable_zval_pairs(HashTable *ht, zval_pair *entries, const uint32_t len)
{
	ZEND_ASSERT(len <= TEDS_MAX_ZVAL_PAIR_COUNT);

	/* Initialize properties array */
	uint32_t i;
	for (i = 0; i < len; i++) {
		zval tmp;
		Z_TRY_ADDREF_P(&entries[i].key);
		Z_TRY_ADDREF_P(&entries[i].value);
		ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
		zend_hash_index_update(ht, i, &tmp);
	}
	const uint32_t properties_size = zend_hash_num_elements(ht);
	if (UNEXPECTED(properties_size > i)) {
		for (; i < properties_size; i++) {
			zend_hash_index_del(ht, i);
		}
	}

#if PHP_VERSION_ID >= 80200
	if (HT_IS_PACKED(ht)) {
		/* Engine doesn't expect packed array */
		zend_hash_packed_to_hash(ht);
	}
#endif
}
#endif

HashTable* teds_mutableiterable_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	teds_mutableiterable_entries *array = &teds_mutableiterable_from_object(obj)->array;

	const uint32_t len = array->size;
	if (!len) {
		/* Nothing to add or remove - this is mutable. */
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
			teds_build_properties_for_mutable_zval_pairs(ht, array->entries, len);
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

void teds_mutableiterable_free_storage(zend_object *object)
{
	teds_mutableiterable *intern = teds_mutableiterable_from_object(object);
	teds_mutableiterable_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_mutableiterable_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_mutableiterable *intern;

	intern = zend_object_alloc(sizeof(teds_mutableiterable), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_MutableIterable);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_MutableIterable;

	if (orig && clone_orig) {
		teds_mutableiterable *other = teds_mutableiterable_from_object(orig);
		teds_mutableiterable_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_mutableiterable_new(zend_class_entry *class_type)
{
	return teds_mutableiterable_new_ex(class_type, NULL, 0);
}


static zend_object *teds_mutableiterable_clone(zend_object *old_object)
{
	zend_object *new_object = teds_mutableiterable_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

TEDS_COUNT_ELEMENTS_RETURN_TYPE teds_mutableiterable_count_elements(zend_object *object, zend_long *count)
{
	const teds_mutableiterable *intern = teds_mutableiterable_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this MutableIterable */
PHP_METHOD(Teds_MutableIterable, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get whether this MutableIterable is empty. */
PHP_METHOD(Teds_MutableIterable, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Get capacity of this MutableIterable */
PHP_METHOD(Teds_MutableIterable, capacity)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(object);
	RETURN_LONG(intern->array.capacity);
}

void teds_mutableiterable_clear(teds_mutableiterable *intern)
{
	if (intern->array.capacity == 0) {
		/* Nothing to clear */
		return;
	}
	/* Immediately make the original storage inaccessible and set count/capacity to 0 in case destructors modify the vector */
	const uint32_t entries_to_remove = intern->array.size;
	zval *const old_entries = (zval *)intern->array.entries;
	teds_mutableiterable_entries_set_empty_list(&intern->array);
	if (intern->std.properties) {
		zend_hash_clean(intern->std.properties);
	}
	teds_zval_dtor_range(old_entries, entries_to_remove * 2);
	efree(old_entries);
}
/* Free elements and backing storage of this MutableIterable */
PHP_METHOD(Teds_MutableIterable, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_mutableiterable_clear(Z_MUTABLEITERABLE_P(ZEND_THIS));
	TEDS_RETURN_VOID();
}

/* Set size of this MutableIterable */
PHP_METHOD(Teds_MutableIterable, setSize)
{
	zend_long size;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED((zend_ulong)size >= TEDS_MAX_ZVAL_PAIR_COUNT)) {
		if (size < 0) {
			zend_argument_value_error(1, "must be greater than or equal to 0");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "exceeded max valid offset");
		}
		RETURN_THROWS();
	}

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t old_size = intern->array.size;
	if ((zend_ulong) size > old_size) {
		/* Raise the capacity as needed and fill the space with nulls */
		if ((zend_ulong) size > intern->array.capacity) {
			teds_mutableiterable_raise_capacity(intern, size);
		}
		intern->array.size = size;
		zval_pair * const entries = intern->array.entries;
		for (uint32_t i = old_size; i < size; i++) {
			ZVAL_NULL(&entries[i].key);
			ZVAL_NULL(&entries[i].value);
		}
		return;
	}
	/* Reduce the size and invalidate memory. If a destructor unexpectedly changes the size then read the new size and keep removing elements. */
	const uint32_t entries_to_remove = old_size - size;
	if (entries_to_remove == 0) {
		return;
	}

	zval_pair *old_entries = intern->array.entries;
	zval *old_copy;
	if (size == 0) {
		old_copy = (zval *)old_entries;
		teds_mutableiterable_entries_set_empty_list(&intern->array);
	} else {
		old_copy = teds_zval_copy_range((const zval *)&old_entries[size], entries_to_remove * 2);
		intern->array.size = size;

		/* If only a quarter of the reserved space is used, then shrink the capacity but leave some room to grow. */
		if ((zend_ulong) size < (intern->array.capacity >> 2)) {
			const uint32_t size = old_size - 1;
			const uint32_t capacity = size > 2 ? size * 2 : 4;
			if (capacity < intern->array.capacity) {
				teds_mutableiterable_shrink_capacity(&intern->array, size, capacity, old_entries);
			}
		}
	}

	teds_zval_dtor_range(old_copy, entries_to_remove * 2);
	efree(old_copy);
}

/* Create this from an iterable */
PHP_METHOD(Teds_MutableIterable, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(object);

	if (UNEXPECTED(!teds_mutableiterable_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\MutableIterable::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}
	if (!iterable) {
		teds_mutableiterable_entries_set_empty_list(&intern->array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_mutableiterable_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_mutableiterable_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_MutableIterable, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

void teds_mutableiterable_it_dtor(zend_object_iterator *iter)
{
	teds_intrusive_dllist_remove(&Z_MUTABLEITERABLE_ENTRIES_P(&iter->data)->active_iterators, &((teds_mutableiterable_it*)iter)->dllist_node);
	zval_ptr_dtor(&iter->data);
}

void teds_mutableiterable_it_rewind(zend_object_iterator *iter)
{
	((teds_mutableiterable_it*)iter)->current = 0;
}

int teds_mutableiterable_it_valid(zend_object_iterator *iter)
{
	teds_mutableiterable_it     *iterator = (teds_mutableiterable_it*)iter;
	teds_mutableiterable *object   = Z_MUTABLEITERABLE_P(&iter->data);

	if (((zend_ulong) iterator->current) < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval_pair *teds_mutableiterable_read_offset_helper(teds_mutableiterable *intern, uint32_t offset)
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

static zval *teds_mutableiterable_it_get_current_data(zend_object_iterator *iter)
{
	teds_mutableiterable_it     *iterator = (teds_mutableiterable_it*)iter;
	teds_mutableiterable *object   = Z_MUTABLEITERABLE_P(&iter->data);

	zval_pair *data = teds_mutableiterable_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_mutableiterable_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_mutableiterable_it     *iterator = (teds_mutableiterable_it*)iter;
	teds_mutableiterable *object   = Z_MUTABLEITERABLE_P(&iter->data);

	zval_pair *data = teds_mutableiterable_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_mutableiterable_it_move_forward(zend_object_iterator *iter)
{
	((teds_mutableiterable_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_mutableiterable_it_funcs = {
	teds_mutableiterable_it_dtor,
	teds_mutableiterable_it_valid,
	teds_mutableiterable_it_get_current_data,
	teds_mutableiterable_it_get_current_key,
	teds_mutableiterable_it_move_forward,
	teds_mutableiterable_it_rewind,
	NULL,
	teds_internaliterator_get_gc,
};


zend_object_iterator *teds_mutableiterable_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_mutableiterable_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_mutableiterable_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	zend_object *obj = Z_OBJ_P(object);
	ZVAL_OBJ_COPY(&iterator->intern.data, obj);
	iterator->intern.funcs = &teds_mutableiterable_it_funcs;

	(void) ce;

	teds_intrusive_dllist_prepend(&teds_mutableiterable_from_object(obj)->array.active_iterators, &iterator->dllist_node);

	return &iterator->intern;
}

PHP_METHOD(Teds_MutableIterable, __unserialize)
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
	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	if (UNEXPECTED(!teds_mutableiterable_entries_uninitialized(&intern->array))) {
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
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\MutableIterable::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}
		ZVAL_COPY_DEREF(it++, val);
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it == &entries[0].key + raw_size);

	intern->array.size = num_entries;
	intern->array.capacity = num_entries;
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


static void teds_mutableiterable_entries_init_from_array_pairs(teds_mutableiterable_entries *array, zend_array *raw_data)
{
	uint32_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->capacity = 0;
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
		num_entries = 0;
	}

	array->entries = entries;
	array->size = actual_size;
	array->capacity = num_entries;
}

static void teds_mutableiterable_entries_init_from_traversable_pairs(teds_mutableiterable_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->capacity = 0;
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
	array->capacity = size;
	array->entries = entries;
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static zend_object* create_from_pairs(zval *iterable) {
	zend_object *object = teds_mutableiterable_new(teds_ce_MutableIterable);
	teds_mutableiterable *intern = teds_mutableiterable_from_object(object);
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_mutableiterable_entries_init_from_array_pairs(&intern->array, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_mutableiterable_entries_init_from_traversable_pairs(&intern->array, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_MutableIterable, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_MutableIterable, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_mutableiterable_new(teds_ce_MutableIterable);
	teds_mutableiterable *intern = teds_mutableiterable_from_object(object);
	teds_mutableiterable_entries_init_from_array_pairs(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_MutableIterable, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);

	if (teds_mutableiterable_entries_empty_size(&intern->array)) {
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

	RETURN_ARR(flat_entries_array);
}


PHP_METHOD(Teds_MutableIterable, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
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

PHP_METHOD(Teds_MutableIterable, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
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

PHP_METHOD(Teds_MutableIterable, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_mutableiterable_entries *const array = Z_MUTABLEITERABLE_ENTRIES_P(ZEND_THIS);
	if (!array->size) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *const values = zend_new_array(array->size);
	/* Initialize return array */

	/* Go through values and add values to the return array */
	for (uint32_t i = 0; i < array->size; i++) {
		zval_pair *entry = &array->entries[i];
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

PHP_METHOD(Teds_MutableIterable, keyAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		TEDS_THROW_INVALID_COLLECTION_OFFSET_EXCEPTION();
	}
	RETURN_COPY(&intern->array.entries[offset].key);
}

PHP_METHOD(Teds_MutableIterable, valueAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		TEDS_THROW_INVALID_COLLECTION_OFFSET_EXCEPTION();
	}
	RETURN_COPY(&intern->array.entries[offset].value);
}

PHP_METHOD(Teds_MutableIterable, indexOfKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(key, &entries[i].key)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_MutableIterable, indexOfValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(value, &entries[i].value)) {
			RETURN_LONG(i);
		}
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_MutableIterable, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(key, &entries[i].key)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_MutableIterable, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	zval_pair *entries = intern->array.entries;
	for (uint32_t i = 0; i < len; i++) {
		if (teds_is_identical_inline(value, &entries[i].value)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_MutableIterable, setKeyAt)
{
	zend_long offset;
	zval *key;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		TEDS_THROW_INVALID_COLLECTION_OFFSET_EXCEPTION();
	}
	zval old_key;
	zval *old_key_ptr = &intern->array.entries[offset].key;
	ZVAL_COPY_VALUE(&old_key, old_key_ptr);
	ZVAL_COPY(old_key_ptr, key);
	zval_ptr_dtor(&old_key);
}

PHP_METHOD(Teds_MutableIterable, setValueAt)
{
	zend_long offset;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		TEDS_THROW_INVALID_COLLECTION_OFFSET_EXCEPTION();
	}
	zval old_value;
	zval *old_value_ptr = &intern->array.entries[offset].value;
	ZVAL_COPY_VALUE(&old_value, old_value_ptr);
	ZVAL_COPY(old_value_ptr, value);
	zval_ptr_dtor(&old_value);
}

PHP_METHOD(Teds_MutableIterable, push)
{
	zval *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t old_size = intern->array.size;
	const uint32_t old_capacity = intern->array.capacity;

	if (old_size >= old_capacity) {
		ZEND_ASSERT(old_size == old_capacity);
		teds_mutableiterable_raise_capacity(intern, old_size ? old_size * 2 : 4);
	}
	zval_pair *dest = &intern->array.entries[old_size];
	ZVAL_COPY(&dest->key, key);
	ZVAL_COPY(&dest->value, value);
	intern->array.size++;

	TEDS_RETURN_VOID();
}

void teds_mutableiterable_adjust_iterators_before_remove(teds_mutableiterable_entries *array, teds_intrusive_dllist_node *node, const uint32_t removed_offset)
{
	const zend_object *const obj = teds_mutableiterable_to_object(array);
	const uint32_t old_size = array->size;
	ZEND_ASSERT(removed_offset < old_size);
	do {
		teds_mutableiterable_it *it = teds_mutableiterable_it_from_node(node);
		ZEND_ASSERT(Z_OBJ(it->intern.data) == obj);
		if (it->current >= removed_offset && it->current < old_size) {
			it->current--;
		}
		ZEND_ASSERT(node != node->next);
		node = node->next;
	} while (node != NULL);
}

PHP_METHOD(Teds_MutableIterable, pop)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_mutableiterable         *const intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	teds_mutableiterable_entries *const array = &intern->array;
	const uint32_t old_size = array->size;
	if (old_size == 0) {
		zend_throw_exception_ex(spl_ce_UnderflowException, 0, "Cannot pop from empty %s", ZSTR_VAL(intern->std.ce->name));
		RETURN_THROWS();
	}
	const uint32_t old_capacity = array->capacity;
	if (UNEXPECTED(array->active_iterators.first)) {
		teds_mutableiterable_adjust_iterators_before_remove(array, array->active_iterators.first, old_size - 1);
	}
	array->size--;
	zval_pair *entry = &array->entries[array->size];
	RETVAL_ARR(zend_new_pair(&entry->key, &entry->value));
	if (old_size < (old_capacity >> 2)) {
		/* Shrink the storage if only a quarter of the capacity is used  */
		const uint32_t size = old_size - 1;
		const uint32_t capacity = size > 2 ? size * 2 : 4;
		if (capacity < old_capacity) {
			teds_mutableiterable_shrink_capacity(array, size, capacity, array->entries);
		}
	}
}

PHP_METHOD(Teds_MutableIterable, shrinkToFit)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	const uint32_t size = intern->array.size;
	const uint32_t old_capacity = intern->array.capacity;
	if (size >= old_capacity) {
		ZEND_ASSERT(size == old_capacity);
		return;
	}

	if (size == 0) {
		efree(intern->array.entries);
		intern->array.entries = (zval_pair *)empty_entry_list;
	} else {
		intern->array.entries = safe_erealloc(intern->array.entries, size, sizeof(zval_pair), 0);
	}
	intern->array.capacity = size;
	TEDS_RETURN_VOID();
}

static void teds_mutableiterable_return_pairs(zval *return_value, teds_mutableiterable *intern)
{
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
			zval tmp;
			Z_TRY_ADDREF_P(&entries[i].key);
			Z_TRY_ADDREF_P(&entries[i].value);
			ZVAL_ARR(&tmp, zend_new_pair(&entries[i].key, &entries[i].value));
			ZEND_HASH_FILL_ADD(&tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_MutableIterable, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_mutableiterable *intern = Z_MUTABLEITERABLE_P(ZEND_THIS);
	teds_mutableiterable_return_pairs(return_value, intern);
}

PHP_MINIT_FUNCTION(teds_mutableiterable)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_MutableIterable = register_class_Teds_MutableIterable(zend_ce_aggregate, teds_ce_Collection, php_json_serializable_ce);
	teds_ce_MutableIterable->create_object = teds_mutableiterable_new;

	memcpy(&teds_handler_MutableIterable, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_MutableIterable.offset          = XtOffsetOf(teds_mutableiterable, std);
	teds_handler_MutableIterable.clone_obj       = teds_mutableiterable_clone;
	teds_handler_MutableIterable.count_elements  = teds_mutableiterable_count_elements;
	teds_handler_MutableIterable.get_properties_for = teds_mutableiterable_get_properties_for;
	teds_handler_MutableIterable.get_gc          = teds_mutableiterable_get_gc;
	teds_handler_MutableIterable.free_obj        = teds_mutableiterable_free_storage;

	teds_ce_MutableIterable->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_MutableIterable->get_iterator = teds_mutableiterable_get_iterator;

	return SUCCESS;
}
