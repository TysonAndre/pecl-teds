/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c but has lower overhead (when size is known) and is more efficient to push and remove elements from the end of the list */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds.h"
#include "teds_immutablesortedstringset_arginfo.h"
#include "teds_immutablesortedstringset.h"
#include "teds_interfaces.h"
#include "teds_exceptions.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"
#include "teds_serialize_util.h"
#include "Zend/zend_smart_str.h"

#include <stdbool.h>

static zend_always_inline zend_string *teds_immutablesortedstringset_convert_zval_value_to_string(const zval *offset) {
	switch (Z_TYPE_P(offset)) {
		case IS_STRING:
			return Z_STR_P(offset);
		case IS_REFERENCE:
			offset = Z_REFVAL_P(offset);
			if (Z_TYPE_P(offset) == IS_STRING) {
				return Z_STR_P(offset);
			}
			break;
	}

	zend_type_error("Illegal Teds\\ImmutableSortedStringSet value type %s", zend_zval_type_name(offset));
	return NULL;
}

#define TEDS_IMMUTABLESORTEDSTRINGSET_THROW_UNSUPPORTEDOPERATIONEXCEPTION() TEDS_THROW_UNSUPPORTEDOPERATIONEXCEPTION("Teds\\ImmutableSortedStringSet is immutable")

#define TEDS_IMMUTABLESORTEDSTRINGSET_VALUE_TO_STRING_OR_THROW(value, value_zv) do { \
	if (UNEXPECTED(Z_TYPE_P((value_zv)) != IS_STRING)) { \
		value = teds_immutablesortedstringset_convert_zval_value_to_string((value_zv)); \
		if (UNEXPECTED(EG(exception))) { \
			return; \
		} \
	} else { \
		value = Z_STR_P((value_zv)); \
	} \
} while(0)


/* Though rare, it is possible to have 64-bit zend_longs and a 32-bit size_t. */
#define MAX_ZVAL_COUNT ((SIZE_MAX / sizeof(zval)) - 1)
#define MAX_VALID_OFFSET ((size_t)(MAX_ZVAL_COUNT > ZEND_LONG_MAX ? ZEND_LONG_MAX : MAX_ZVAL_COUNT))

zend_object_handlers teds_handler_ImmutableSortedStringSet;
zend_class_entry *teds_ce_ImmutableSortedStringSet;

typedef struct _teds_immutablesortedstringset_substring_info {
	uint32_t offset;
	uint32_t length;
} teds_immutablesortedstringset_substring_info;

typedef struct _teds_immutablesortedstringset_entries {
	/* Size values referencing a sorted array */
	teds_immutablesortedstringset_substring_info *substring_infos;
	/* Number of elements */
	uint32_t size;
	/* Pointer into the ZSTR_VAL of a Z_STR that may or may not be refcounted */
	const char *raw_bytes;
} teds_immutablesortedstringset_entries;

typedef struct _teds_immutablesortedstringset {
	teds_immutablesortedstringset_entries array;
	zend_object				              std;
} teds_immutablesortedstringset;

/* Used by InternalIterator returned by ImmutableSortedStringSet->getIterator() */
typedef struct _teds_immutablesortedstringset_it {
	zend_object_iterator intern;
	size_t               current;
	/* Temporary memory location to store the most recent get_current_value() result */
	zval                 tmp;
} teds_immutablesortedstringset_it;

static zend_always_inline int teds_binary_strcmp(const char* a, const size_t a_len, const char *b, const size_t b_len);

static zend_always_inline zval *teds_immutablesortedstringset_entries_read_offset(const teds_immutablesortedstringset_entries *intern, const size_t offset, zval *tmp);
static void teds_immutablesortedstringset_entries_init_from_array_values(teds_immutablesortedstringset_entries *array, zend_array *raw_bytes);
static zend_array *teds_immutablesortedstringset_entries_to_refcounted_array(const teds_immutablesortedstringset_entries *array);

static zend_always_inline zend_string *teds_immutablesortedstringset_entries_get_backing_zend_string(const teds_immutablesortedstringset_entries *array) {
	const char *bytes = array->raw_bytes;
	if (bytes == NULL) {
		return NULL;
	}
	zend_string *result = (zend_string *)(bytes - XtOffsetOf(zend_string, val));
	ZEND_ASSERT(ZSTR_IS_INTERNED(result) || GC_REFCOUNT(result) > 0);
	ZEND_ASSERT(ZSTR_LEN(result) >= array->size);
	return result;
}

/*
 * If a size this large is encountered, assume the allocation will likely fail or
 * future changes to the size will overflow.
 */
static ZEND_COLD void teds_error_noreturn_max_immutablesortedstringset_size()
{
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\ImmutableSortedStringSet size");
}

static zend_always_inline teds_immutablesortedstringset *teds_immutablesortedstringset_from_object(zend_object *obj)
{
	return (teds_immutablesortedstringset*)((char*)(obj) - XtOffsetOf(teds_immutablesortedstringset, std));
}
#define teds_immutablesortedstringset_entries_from_object(obj) (&teds_immutablesortedstringset_from_object((obj))->array)

#define Z_IMMUTABLESORTEDSTRINGSET_P(zv)  (teds_immutablesortedstringset_from_object(Z_OBJ_P((zv))))
#define Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(zv)  (&(Z_IMMUTABLESORTEDSTRINGSET_P((zv))->array))

static zend_always_inline bool teds_immutablesortedstringset_entries_empty_size(const teds_immutablesortedstringset_entries *array)
{
	if (array->size > 0) {
		ZEND_ASSERT(array->raw_bytes != NULL);
		return false;
	}
	return true;
}

static zend_always_inline bool teds_immutablesortedstringset_entries_uninitialized(const teds_immutablesortedstringset_entries *array)
{
	if (array->raw_bytes == NULL) {
		ZEND_ASSERT(array->size == 0);
		return true;
	}
	return false;
}

static zend_always_inline void teds_immutablesortedstringset_entries_set_empty_list(teds_immutablesortedstringset_entries *array) {
	array->size = 0;
	array->raw_bytes = ZSTR_VAL(ZSTR_EMPTY_ALLOC());
}

static void teds_immutablesortedstringset_entries_sort_and_deduplicate_and_set_list(teds_immutablesortedstringset_entries *const array, zend_string **const raw_zend_strings, uint32_t num_strings);

static void teds_immutablesortedstringset_entries_init_from_traversable(teds_immutablesortedstringset_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_immutablesortedstringset_entries_set_empty_list(array);
	zval tmp_obj;
	ZVAL_OBJ(&tmp_obj, obj);
	iter = ce->get_iterator(ce, &tmp_obj, 0);

	if (UNEXPECTED(EG(exception))) {
		return;
	}

	zend_string **raw_zend_strings = NULL;
	uint32_t size = 0;
	size_t capacity = 0;

	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (funcs->rewind) {
		funcs->rewind(iter);
		if (UNEXPECTED(EG(exception))) {
			goto cleanup;
		}
	}

	/* Reindex keys from 0. */
	while (funcs->valid(iter) == SUCCESS) {
		if (EG(exception)) {
			goto cleanup;
		}
		zval *value_zv = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			goto cleanup;
		}

		zend_string *value;
		if (UNEXPECTED(Z_TYPE_P(value_zv) != IS_STRING)) {
			value = teds_immutablesortedstringset_convert_zval_value_to_string(value_zv);
			if (UNEXPECTED(EG(exception))) {
				goto cleanup;
			}
		} else {
			value = Z_STR_P(value_zv);
		}
		if (size >= capacity) {
			ZEND_ASSERT(size == capacity);
			if (capacity > 0) {
				if (capacity >= TEDS_MAX_ZVAL_COLLECTION_SIZE) {
					teds_error_noreturn_max_immutablesortedstringset_size();
				}
				capacity *= 2;
				raw_zend_strings = safe_erealloc(raw_zend_strings, capacity, sizeof(zend_string *), 0);
			} else {
				capacity = 4;
				raw_zend_strings = emalloc(capacity * sizeof(zend_string *));
			}
		}
		raw_zend_strings[size++] = value;

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			goto cleanup;
		}
	}

	/* free excess capacity */
	teds_immutablesortedstringset_entries_sort_and_deduplicate_and_set_list(array, raw_zend_strings, size);

cleanup:
	if (raw_zend_strings) {
		efree(raw_zend_strings);
	}
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static void teds_immutablesortedstringset_entries_copy_ctor(teds_immutablesortedstringset_entries *to, const teds_immutablesortedstringset_entries *from)
{
	zend_long size = from->size;
	if (!size) {
		teds_immutablesortedstringset_entries_set_empty_list(to);
		return;
	}

	/* FIXME copy the entry instead */
	to->size = from->size;
	to->raw_bytes = from->raw_bytes;
	zend_string *str = teds_immutablesortedstringset_entries_get_backing_zend_string(from);
	if (str) {
		zend_string_addref(str);
	}
}

static HashTable* teds_immutablesortedstringset_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	(void)purpose;
	teds_immutablesortedstringset_entries *array = &teds_immutablesortedstringset_from_object(obj)->array;
	if (!array->size) {
		/* Similar to ext/ffi/ffi.c zend_fake_get_properties */
		return (HashTable*)&zend_empty_array;
	}
	/* var_export uses get_properties_for for infinite recursion detection rather than get_properties(Z_OBJPROP).
	 * or checking for recursion on the object itself (php_var_dump).
	 * However, ImmutableSortedStringSet can only contain integers, making infinite recursion impossible, so it's safe to return new arrays. */
	return teds_immutablesortedstringset_entries_to_refcounted_array(array);
}

static void teds_immutablesortedstringset_free_storage(zend_object *object)
{
	teds_immutablesortedstringset *intern = teds_immutablesortedstringset_from_object(object);
	teds_immutablesortedstringset_entries *array = &intern->array;
	zend_string *backing_string = teds_immutablesortedstringset_entries_get_backing_zend_string(array);
	if (backing_string) {
		zend_string_release(backing_string);
		array->raw_bytes = NULL;
		efree(array->substring_infos);
		array->substring_infos = NULL;
	}
	array->size = 0;

	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_immutablesortedstringset_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_immutablesortedstringset *intern;

	intern = zend_object_alloc(sizeof(teds_immutablesortedstringset), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_ImmutableSortedStringSet);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_ImmutableSortedStringSet;

	if (orig && clone_orig) {
		teds_immutablesortedstringset *other = teds_immutablesortedstringset_from_object(orig);
		teds_immutablesortedstringset_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.raw_bytes = NULL;
	}

	return &intern->std;
}

static zend_object *teds_immutablesortedstringset_new(zend_class_entry *class_type)
{
	return teds_immutablesortedstringset_new_ex(class_type, NULL, 0);
}

static zend_object *teds_immutablesortedstringset_clone(zend_object *old_object)
{
	zend_object *new_object = teds_immutablesortedstringset_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_immutablesortedstringset_count_elements(zend_object *object, zend_long *count)
{
	const teds_immutablesortedstringset *intern = teds_immutablesortedstringset_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this immutablesortedstringset */
PHP_METHOD(Teds_ImmutableSortedStringSet, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesortedstringset *intern = Z_IMMUTABLESORTEDSTRINGSET_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get number of entries in this immutablesortedstringset */
PHP_METHOD(Teds_ImmutableSortedStringSet, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesortedstringset *intern = Z_IMMUTABLESORTEDSTRINGSET_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Create this from an optional iterable */
PHP_METHOD(Teds_ImmutableSortedStringSet, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(object);

	if (UNEXPECTED(!teds_immutablesortedstringset_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\ImmutableSortedStringSet::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}
	if (!iterable) {
		teds_immutablesortedstringset_entries_set_empty_list(array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_immutablesortedstringset_entries_init_from_array_values(array, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_immutablesortedstringset_entries_init_from_traversable(array, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

/* Clear this */
ZEND_COLD PHP_METHOD(Teds_ImmutableSortedStringSet, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	TEDS_IMMUTABLESORTEDSTRINGSET_THROW_UNSUPPORTEDOPERATIONEXCEPTION();
}

PHP_METHOD(Teds_ImmutableSortedStringSet, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_immutablesortedstringset_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_immutablesortedstringset_it_rewind(zend_object_iterator *iter)
{
	((teds_immutablesortedstringset_it*)iter)->current = 0;
}

static int teds_immutablesortedstringset_it_valid(zend_object_iterator *iter)
{
	teds_immutablesortedstringset_it     *iterator = (teds_immutablesortedstringset_it*)iter;
	teds_immutablesortedstringset *object   = Z_IMMUTABLESORTEDSTRINGSET_P(&iter->data);

	if (iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}


static zval *teds_immutablesortedstringset_it_get_current_data(zend_object_iterator *iter)
{
	teds_immutablesortedstringset_it *iterator = (teds_immutablesortedstringset_it*)iter;
	teds_immutablesortedstringset_entries *array   = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(&iter->data);
	if (UNEXPECTED(iterator->current >= array->size)) {
		teds_throw_invalid_sequence_index_exception();
		return &EG(uninitialized_zval);
	}
	return teds_immutablesortedstringset_entries_read_offset(array, iterator->current, &iterator->tmp);
}

static void teds_immutablesortedstringset_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_immutablesortedstringset_it     *iterator = (teds_immutablesortedstringset_it*)iter;
	teds_immutablesortedstringset *object   = Z_IMMUTABLESORTEDSTRINGSET_P(&iter->data);

	size_t offset = iterator->current;
	if (offset >= object->array.size) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, offset);
	}
}

static void teds_immutablesortedstringset_it_move_forward(zend_object_iterator *iter)
{
	((teds_immutablesortedstringset_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_immutablesortedstringset_it_funcs = {
	teds_immutablesortedstringset_it_dtor,
	teds_immutablesortedstringset_it_valid,
	teds_immutablesortedstringset_it_get_current_data,
	teds_immutablesortedstringset_it_get_current_key,
	teds_immutablesortedstringset_it_move_forward,
	teds_immutablesortedstringset_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_immutablesortedstringset_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	// This is final
	ZEND_ASSERT(ce == teds_ce_ImmutableSortedStringSet);
	teds_immutablesortedstringset_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_immutablesortedstringset_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_immutablesortedstringset_it_funcs;

	return &iterator->intern;
}

static void teds_immutablesortedstringset_entries_unserialize_from_zend_string(teds_immutablesortedstringset_entries *const array, zend_string *const str)
{
	if (ZSTR_LEN(str) <= 4) {
		zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet unserialize missing header", 0);
		return;
	}
	if (UNEXPECTED(ZSTR_LEN(str) > UINT32_MAX)) {
		zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet unserialize too much data", 0);
		return;
	}
	const uint8_t *const raw_bytes = (uint8_t *)ZSTR_VAL(str);
	const uint8_t *it = raw_bytes;
	const uint8_t *const end = raw_bytes + ZSTR_LEN(str);

	const uint32_t num_strings = teds_serialize_read_uint32_t(it);
	it += 4;
	teds_immutablesortedstringset_substring_info *substring_infos = NULL;

	if (UNEXPECTED(num_strings > (end - it))) {
		// fprintf(stderr, "num_strings=%d end-it=%d\n", num_strings, (int)(end-it));
not_enough_data:
		if (substring_infos) {
			efree(substring_infos);
		}
		zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet not enough data for strings", 0);
		return;
	}

	substring_infos = safe_emalloc(num_strings, sizeof(teds_immutablesortedstringset_substring_info), 0);
	for (uint32_t i = 0; i < num_strings; i++) {
		ZEND_ASSERT(it <= end);
		/* substr_len is 0 or more */
		uint32_t substr_len;
		/* it and num_strings are read and moved by reference */
		if (!teds_serialize_read_variable_len_size(&it, end, &substr_len)) {
			ZEND_ASSERT(EG(exception));
			return;
		}
		// fprintf(stderr, "substr_len=%d end=%p it=%p end-it=%d\n", substr_len, end, it, (int)(end-it));
		if (UNEXPECTED(substr_len > end - it)) {
			goto not_enough_data;
		}
		substring_infos[i].offset = it - raw_bytes;
		substring_infos[i].length = substr_len;
		if (i > 0 && UNEXPECTED(teds_binary_strcmp((const char *)it, substr_len, ((const char *)raw_bytes) + substring_infos[i - 1].offset, substring_infos[i - 1].length) <= 0)) {
			efree(substring_infos);
			zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet was given data to unserialize that was not sorted", 0);
			return;
		}
		it += substr_len;
	}
	if (it != end) {
		efree(substring_infos);
		zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet more data to unserialize than used", 0);
		return;
	}
	zend_string_addref(str);
	array->raw_bytes = (const char *)raw_bytes;
	array->substring_infos = substring_infos;
	array->size = num_strings;
}

PHP_METHOD(Teds_ImmutableSortedStringSet, __unserialize)
{
	HashTable *raw_bytes;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_bytes) == FAILURE) {
		RETURN_THROWS();
	}
	teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!teds_immutablesortedstringset_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (zend_hash_num_elements(raw_bytes) == 0) {
		teds_immutablesortedstringset_entries_set_empty_list(array);
		return;
	}

	if (UNEXPECTED(zend_hash_num_elements(raw_bytes) != 1)) {
		zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet unexpected __unserialize data: expected exactly 1 value", 0);
		RETURN_THROWS();
	}
	const zval *raw_zval = zend_hash_index_find(raw_bytes, 0);
	if (UNEXPECTED(raw_zval == NULL)) {
		zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet missing data to unserialize", 0);
		RETURN_THROWS();
	}
	if (Z_TYPE_P(raw_zval) != IS_STRING) {
		zend_throw_exception(spl_ce_RuntimeException, "ImmutableSortedStringSet expected string for binary data", 0);
		RETURN_THROWS();
	}
	zend_string *str = Z_STR_P(raw_zval);
	teds_immutablesortedstringset_entries_unserialize_from_zend_string(array, str);
}

PHP_METHOD(Teds_ImmutableSortedStringSet, unserialize)
{
	zend_string *zstr;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zstr)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *new_object = teds_immutablesortedstringset_new(teds_ce_ImmutableSortedStringSet);
	teds_immutablesortedstringset_entries *array = teds_immutablesortedstringset_entries_from_object(new_object);
	if (ZSTR_LEN(zstr) == 0) {
		teds_immutablesortedstringset_entries_set_empty_list(array);
	} else {
		teds_immutablesortedstringset_entries_unserialize_from_zend_string(array, zstr);
	}
	RETURN_OBJ(new_object);
}

PHP_METHOD(Teds_ImmutableSortedStringSet, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesortedstringset_entries *const array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *result = zend_new_array(1);
	zval tmp;
	zend_string *backing_string = teds_immutablesortedstringset_entries_get_backing_zend_string(array);
	ZVAL_STR_COPY(&tmp, backing_string);
	zend_hash_next_index_insert(result, &tmp);
	RETURN_ARR(result);
}

PHP_METHOD(Teds_ImmutableSortedStringSet, serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesortedstringset_entries *const array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		RETURN_EMPTY_STRING();
	}
	zend_string *backing_string = teds_immutablesortedstringset_entries_get_backing_zend_string(array);
	RETURN_STR_COPY(backing_string);
}

/* Deliberately copied so that inlining will work instead of being forced to make a function call */
static zend_always_inline int teds_binary_strcmp(const char* a, const size_t a_len, const char *b, const size_t b_len)
{
	const int retval = memcmp(a, b, MIN(a_len, b_len));
	if (!retval) {
		/** NOTE: zend_binary_strcmp is wrong for this for 4GB long strings. Not really important. */
		if (a_len != b_len) {
			return a_len > b_len ? 1 : -1;
		}
		return 0;
	} else {
		return retval;
	}
}

static int teds_zend_string_compare(const void *a, const void *b)
{
	const zend_string *a_str = *(const zend_string **)a;
	const zend_string *b_str = *(const zend_string **)b;

	if (a_str == b_str) {
		return 0;
	}
	return teds_binary_strcmp(ZSTR_VAL(a_str), ZSTR_LEN(a_str), ZSTR_VAL(b_str), ZSTR_LEN(b_str));
}

static zend_always_inline void teds_smart_str_append_variable_len_size(smart_str *dest, uint32_t value)
{
	smart_str_alloc(dest, 5, 0);
	uint8_t *const dst = (uint8_t *)(ZSTR_VAL(dest->s) + ZSTR_LEN(dest->s));
	const uint8_t written = teds_serialize_write_variable_len_size(dst, value);
	ZSTR_LEN(dest->s) += written;
}

static zend_always_inline void teds_smart_str_append_uint32(smart_str *dest, uint32_t value)
{
	const size_t new_len = smart_str_alloc(dest, 4, 0);
	uint8_t *const dst = (uint8_t *)(ZSTR_VAL(dest->s) + ZSTR_LEN(dest->s));
	teds_serialize_write_uint32_t(dst, value);
	ZSTR_LEN(dest->s) = new_len;
}

static zend_always_inline void teds_smart_str_append_uninitialized_uint32(smart_str *dest)
{
	size_t new_len = smart_str_alloc(dest, 4, 0);
	ZSTR_LEN(dest->s) = new_len;
}

static void teds_immutablesortedstringset_entries_sort_and_deduplicate_and_set_list(teds_immutablesortedstringset_entries *const array, zend_string **const raw_zend_strings, uint32_t num_strings) {
	qsort(raw_zend_strings, num_strings, sizeof(zend_string *), teds_zend_string_compare);
	smart_str sbuf = {0};
	teds_smart_str_append_uninitialized_uint32(&sbuf);
	teds_immutablesortedstringset_substring_info *substring_infos = safe_emalloc(num_strings, sizeof(teds_immutablesortedstringset_substring_info), 0);

	zend_string *prev_str = NULL;
	uint32_t num_unique_strings = 0;
	for (uint32_t i = 0; i < num_strings; i++) {
		zend_string *str = raw_zend_strings[i];
		if (prev_str && UNEXPECTED(zend_string_equals(str, prev_str))) {
			continue;
		}
		size_t cur_str_len = ZSTR_LEN(str);

		teds_smart_str_append_variable_len_size(&sbuf, cur_str_len);
		substring_infos[num_unique_strings].offset = ZSTR_LEN(sbuf.s);
		substring_infos[num_unique_strings].length = cur_str_len;
		num_unique_strings++;

		smart_str_append(&sbuf, str);

		prev_str = str;
	}
	if (UNEXPECTED(ZSTR_LEN(sbuf.s) > UINT32_MAX)) {
		zend_throw_exception(spl_ce_RuntimeException, "Cannot create Teds\\ImmutableSortedStringSet with over 4GB of data", 0);
		efree(substring_infos);
		smart_str_free(&sbuf);
		return;
	}
	/* TODO erealloc to remove excess capacity */
	array->size = num_unique_strings;
	zend_string *str = smart_str_extract(&sbuf);
	teds_serialize_write_uint32_t((uint8_t *) ZSTR_VAL(str), num_unique_strings);
	array->raw_bytes = ZSTR_VAL(str);
	array->substring_infos = substring_infos;
	return;
}


static void teds_immutablesortedstringset_entries_init_from_array_values(teds_immutablesortedstringset_entries *array, zend_array *array_of_strings)
{
	uint32_t num_entries = zend_hash_num_elements(array_of_strings);
	teds_immutablesortedstringset_entries_set_empty_list(array);
	if (num_entries == 0) {
		return;
	}
	zend_string **raw_zend_strings = safe_emalloc(num_entries, sizeof(zend_string*), 0);
	uint32_t num = 0;
	zval *value_zv;
	ZEND_HASH_FOREACH_VAL(array_of_strings, value_zv) {
		zend_string *value;
		if (UNEXPECTED(Z_TYPE_P(value_zv) != IS_STRING)) {
			value = teds_immutablesortedstringset_convert_zval_value_to_string(value_zv);
			if (UNEXPECTED(EG(exception))) {
				efree(raw_zend_strings);
				return;
			}
		} else {
			value = Z_STR_P(value_zv);
		}
		raw_zend_strings[num++] = value;
	} ZEND_HASH_FOREACH_END();

	ZEND_ASSERT(num == num_entries);

	teds_immutablesortedstringset_entries_sort_and_deduplicate_and_set_list(array, raw_zend_strings, num);
	efree(raw_zend_strings);
}

PHP_METHOD(Teds_ImmutableSortedStringSet, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_immutablesortedstringset_new(teds_ce_ImmutableSortedStringSet);
	teds_immutablesortedstringset_entries *array = teds_immutablesortedstringset_entries_from_object(object);
	teds_immutablesortedstringset_entries_init_from_array_values(array, array_ht);

	RETURN_OBJ(object);
}

static zend_array *teds_immutablesortedstringset_entries_to_refcounted_array(const teds_immutablesortedstringset_entries *array) {
	size_t len = array->size;
	zend_array *values = teds_new_array_check_overflow(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	const teds_immutablesortedstringset_substring_info *const substring_infos = array->substring_infos;
	const char *const raw_bytes = array->raw_bytes;

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (uint32_t i = 0; i < len; i++) {
			const teds_immutablesortedstringset_substring_info *substring_info = &substring_infos[i];
			zval tmp;
			ZVAL_STRINGL(&tmp, raw_bytes + substring_info->offset, substring_info->length);
			ZEND_HASH_FILL_ADD(&tmp);
		}
	} ZEND_HASH_FILL_END();
	return values;
}

PHP_METHOD(Teds_ImmutableSortedStringSet, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	const teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	size_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_immutablesortedstringset_entries_to_refcounted_array(array));
}

static zend_always_inline void teds_immutablesortedstringset_entries_copy_offset(const teds_immutablesortedstringset_entries *array, const zend_ulong offset, zval *dst)
{
	ZEND_ASSERT(offset < array->size);
	ZEND_ASSERT(dst != NULL);

	const teds_immutablesortedstringset_substring_info *substring_info = &array->substring_infos[offset];

	ZVAL_STRINGL(dst, array->raw_bytes + substring_info->offset, substring_info->length);
}

static zend_always_inline void teds_immutablesortedstringset_convert_zval_value_to_long_at_offset(zval *return_value, const zval *zval_this, zend_long offset)
{
	teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(zval_this);
	size_t len = array->size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		teds_throw_invalid_sequence_index_exception();
		RETURN_THROWS();
	}
	teds_immutablesortedstringset_entries_copy_offset(array, offset, return_value);
}

PHP_METHOD(Teds_ImmutableSortedStringSet, get)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_immutablesortedstringset_convert_zval_value_to_long_at_offset(return_value, ZEND_THIS, offset);
}

static const teds_immutablesortedstringset_substring_info *teds_immutablesortedstringset_entries_binary_search(const teds_immutablesortedstringset_entries *array, const zend_string *target)
{
	uint32_t end = array->size;
	if (end == 0) {
		return NULL;
	}
	const teds_immutablesortedstringset_substring_info *const substring_infos = array->substring_infos;
	const char *raw_bytes = array->raw_bytes;
	uint32_t start = 0;
	const size_t target_len = ZSTR_LEN(target);

	do {
		const uint32_t mid = start + (end - start) / 2;
		const teds_immutablesortedstringset_substring_info *const info = &substring_infos[mid];

		const int result = teds_binary_strcmp(ZSTR_VAL(target), target_len, raw_bytes + info->offset, info->length);
		if (result < 0) {
			end = mid;
		} else if (result > 0) {
			start = mid + 1;
		} else {
			return info;
		}
	} while (start < end);
	return NULL;
}

PHP_METHOD(Teds_ImmutableSortedStringSet, indexOf)
{
	zend_string *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	const teds_immutablesortedstringset_substring_info *result = teds_immutablesortedstringset_entries_binary_search(array, value);
	if (result) {
		RETURN_LONG(result - array->substring_infos);
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_ImmutableSortedStringSet, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	if (Z_TYPE_P(value) != IS_STRING) {
		RETURN_FALSE;
	}
	const teds_immutablesortedstringset_substring_info *result = teds_immutablesortedstringset_entries_binary_search(array, Z_STR_P(value));
	RETURN_BOOL(result != NULL);
}

static zend_always_inline zend_string *teds_immutablesortedstringset_entries_read_offset_as_new_string(const teds_immutablesortedstringset_entries *const array, const size_t offset)
{
	const teds_immutablesortedstringset_substring_info *substring_info = &array->substring_infos[offset];
	return zend_string_init(array->raw_bytes + substring_info->offset, substring_info->length, 0);
}

static zend_always_inline zval *teds_immutablesortedstringset_entries_read_offset(const teds_immutablesortedstringset_entries *const array, const size_t offset, zval *const tmp)
{
	ZVAL_STR(tmp, teds_immutablesortedstringset_entries_read_offset_as_new_string(array, offset));
	return tmp;
}

PHP_METHOD(Teds_ImmutableSortedStringSet, add)
{
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		RETURN_THROWS();
	}

	(void)value;
	TEDS_IMMUTABLESORTEDSTRINGSET_THROW_UNSUPPORTEDOPERATIONEXCEPTION();
}

PHP_METHOD(Teds_ImmutableSortedStringSet, last)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read last value of empty Teds\\ImmutableSortedStringSet", 0);
		RETURN_THROWS();
	}
	teds_immutablesortedstringset_entries_copy_offset(array, old_size - 1, return_value);
}

PHP_METHOD(Teds_ImmutableSortedStringSet, first)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_immutablesortedstringset_entries *array = Z_IMMUTABLESORTEDSTRINGSET_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read first value of empty Teds\\ImmutableSortedStringSet", 0);
		RETURN_THROWS();
	}
	teds_immutablesortedstringset_entries_copy_offset(array, 0, return_value);
}

PHP_MINIT_FUNCTION(teds_immutablesortedstringset)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_ImmutableSortedStringSet = register_class_Teds_ImmutableSortedStringSet(zend_ce_aggregate, teds_ce_Set, php_json_serializable_ce);
	teds_ce_ImmutableSortedStringSet->create_object = teds_immutablesortedstringset_new;

	memcpy(&teds_handler_ImmutableSortedStringSet, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_ImmutableSortedStringSet.offset          = XtOffsetOf(teds_immutablesortedstringset, std);
	teds_handler_ImmutableSortedStringSet.clone_obj       = teds_immutablesortedstringset_clone;
	teds_handler_ImmutableSortedStringSet.count_elements  = teds_immutablesortedstringset_count_elements;
	teds_handler_ImmutableSortedStringSet.get_properties  = teds_noop_empty_array_get_properties;
	teds_handler_ImmutableSortedStringSet.get_properties_for = teds_immutablesortedstringset_get_properties_for;
	teds_handler_ImmutableSortedStringSet.get_gc          = teds_noop_get_gc;
	teds_handler_ImmutableSortedStringSet.free_obj        = teds_immutablesortedstringset_free_storage;

	/*
	teds_handler_ImmutableSortedStringSet.read_dimension  = teds_immutablesortedstringset_read_dimension;
	teds_handler_ImmutableSortedStringSet.write_dimension = teds_immutablesortedstringset_write_dimension;
	teds_handler_ImmutableSortedStringSet.has_dimension   = teds_immutablesortedstringset_has_dimension;
	*/

	teds_ce_ImmutableSortedStringSet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_ImmutableSortedStringSet->get_iterator = teds_immutablesortedstringset_get_iterator;

	return SUCCESS;
}
