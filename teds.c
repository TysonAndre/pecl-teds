/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

#if PHP_VERSION_ID < 80000
#error PHP versions prior to php 8.0 are not supported
#endif

#if PHP_VERSION_ID < 80100
#define RETURN_COPY_DEREF(zv)			do { ZVAL_COPY_DEREF(return_value, zv); return; } while (0)
#endif

#include <ctype.h>
#include "zend_interfaces.h"
#include "zend_types.h"
#include "zend_smart_str.h"
#include "zend_strtod_int.h" /* WORDS_BIGENDIAN */
#include "ext/spl/spl_iterators.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#include "teds_bitset.h"
#include "teds_deque.h"
#include "teds_lowmemoryvector.h"
#include "teds_immutablekeyvaluesequence.h"
#include "teds_immutablesequence.h"
#include "teds_intvector.h"
#include "teds_interfaces.h"
#include "teds_keyvaluevector.h"
#include "teds_sortedstrictmap.h"
#include "teds_sortedstrictset.h"
#include "teds_stableheap.h"
#include "teds_stablesortedlistmap.h"
#include "teds_stablesortedlistset.h"
#include "teds_strictmap.h"
#include "teds_strictset.h"
#include "teds_vector.h"
#include "teds.h"

#include "teds_arginfo.h"
#include "php_teds.h"

#if PHP_VERSION_ID >= 80100
#include "zend_enum.h"
#endif

#define TEDS_STR_DEFS \
	X(found)

#define TEDS_STR(x) teds_str_ ## str

#define X(str) \
	static zend_string *TEDS_STR(str);
TEDS_STR_DEFS
#undef X

#ifndef ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) \
	ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
#endif

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* Copied from array_reduce implementation in https://github.com/php/php-src/blob/master/ext/standard/array.c */
static zend_always_inline void teds_array_reduce(HashTable *htbl, zend_fcall_info fci, zend_fcall_info_cache fci_cache, zval* return_value) /* {{{ */
{
	zval args[2];
	zval *operand;
	zval retval;

	if (zend_hash_num_elements(htbl) == 0) {
		return;
	}

	fci.retval = &retval;
	fci.param_count = 2;

	ZEND_HASH_FOREACH_VAL(htbl, operand) {
		ZVAL_COPY_VALUE(&args[0], return_value);
		ZVAL_COPY(&args[1], operand);
		fci.params = args;

		if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
			zval_ptr_dtor(&args[1]);
			zval_ptr_dtor(&args[0]);
			ZVAL_COPY_VALUE(return_value, &retval);
			if (UNEXPECTED(Z_ISREF_P(return_value))) {
				zend_unwrap_reference(return_value);
			}
		} else {
			zval_ptr_dtor(&args[1]);
			zval_ptr_dtor(&args[0]);
			RETURN_NULL();
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

typedef struct {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval args[2];
} traversable_reduce_data;

static int teds_traversable_reduce_elem(zend_object_iterator *iter, void *puser) /* {{{ */
{
	traversable_reduce_data *reduce_data = puser;
	zend_fcall_info *fci = &reduce_data->fci;
	ZEND_ASSERT(ZEND_FCI_INITIALIZED(*fci));

	zval *operand = iter->funcs->get_current_data(iter);
	if (UNEXPECTED(!operand || EG(exception))) {
		return ZEND_HASH_APPLY_STOP;
	}
	ZVAL_DEREF(operand);
	ZVAL_COPY_VALUE(&reduce_data->args[0], fci->retval);
	ZVAL_COPY(&reduce_data->args[1], operand);
	ZVAL_NULL(fci->retval);
	int result = zend_call_function(&reduce_data->fci, &reduce_data->fcc);
	zval_ptr_dtor(operand);
	zval_ptr_dtor(&reduce_data->args[0]);
	if (UNEXPECTED(result == FAILURE || EG(exception))) {
		return ZEND_HASH_APPLY_STOP;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static zend_always_inline void teds_traversable_reduce(zval *obj, zend_fcall_info fci, zend_fcall_info_cache fci_cache, zval* return_value) /* {{{ */
{
	traversable_reduce_data reduce_data;
	reduce_data.fci = fci;
	reduce_data.fci.retval = return_value;
	reduce_data.fci.param_count = 2;
	reduce_data.fci.params = reduce_data.args;
	reduce_data.fcc = fci_cache;

	spl_iterator_apply(obj, teds_traversable_reduce_elem, (void*)&reduce_data);
}
/* }}} */


static inline void php_array_until(zval *return_value, HashTable *htbl, zend_fcall_info fci, zend_fcall_info_cache fci_cache, int stop_value, int negate) /* {{{ */
{
	zval args[1];
	bool have_callback = 0;
	zval *operand;
	zval retval;
	int result;

	if (zend_hash_num_elements(htbl) == 0) {
		RETURN_BOOL(!stop_value ^ negate);
	}

	if (ZEND_FCI_INITIALIZED(fci)) {
		have_callback = 1;
		fci.retval = &retval;
		fci.param_count = 1;
		fci.params = args;
	}

	ZEND_HASH_FOREACH_VAL(htbl, operand) {
		if (have_callback) {
			ZVAL_COPY(&args[0], operand);

			/* Treat the operand like an array of size 1  */
			result = zend_call_function(&fci, &fci_cache);
			zval_ptr_dtor(operand);
			if (result == FAILURE) {
				return;
			}
			const bool retval_true = zend_is_true(&retval);
			zval_ptr_dtor(&retval);
			/* The user-provided callback rarely returns refcounted values. */
			if (retval_true == stop_value) {
				RETURN_BOOL(stop_value ^ negate);
			}
		} else {
			if (zend_is_true(operand) == stop_value) {
				RETURN_BOOL(stop_value ^ negate);
			}
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_BOOL(!stop_value ^ negate);
}
/* }}} */

typedef struct {
	int                    stop_value;
	int                    result;
	zend_long              has_callback;
	zend_fcall_info        fci;
	zend_fcall_info_cache  fcc;
	bool                   found;
} php_iterator_until_info;

static int teds_traversable_func_until(zend_object_iterator *iter, void *puser) /* {{{ */
{
	zend_fcall_info fci;
	zval retval;
	php_iterator_until_info *until_info = (php_iterator_until_info*) puser;
	int result;
	bool stop;

	fci = until_info->fci;
	if (ZEND_FCI_INITIALIZED(fci)) {
		zval *operand = iter->funcs->get_current_data(iter);
		if (UNEXPECTED(!operand || EG(exception))) {
			until_info->result = FAILURE;
			return ZEND_HASH_APPLY_STOP;
		}
		ZVAL_DEREF(operand);
		fci.retval = &retval;
		fci.param_count = 1;
		/* Use the operand like an array of size 1 */
		fci.params = operand;
		fci.retval = &retval;
		Z_TRY_ADDREF_P(operand);
		result = zend_call_function(&fci, &until_info->fcc);
		zval_ptr_dtor(operand);
		if (result == FAILURE) {
			until_info->result = FAILURE;
			return ZEND_HASH_APPLY_STOP;
		}

		stop = zend_is_true(&retval) == until_info->stop_value;
		zval_ptr_dtor(&retval);
	} else {
		zval *operand = iter->funcs->get_current_data(iter);
		if (UNEXPECTED(!operand || EG(exception))) {
			return ZEND_HASH_APPLY_STOP;
		}
		ZVAL_DEREF(operand);
		stop = zend_is_true(operand) == until_info->stop_value;
		zval_ptr_dtor(operand);
	}
	if (stop) {
		until_info->found = true;
		return ZEND_HASH_APPLY_STOP;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

typedef struct {
	zval target;
	int result;
	bool found;
} php_traversable_search_info;

static int teds_traversable_func_search_value(zend_object_iterator *iter, void *puser) /* {{{ */
{
	php_traversable_search_info *search_info = (php_traversable_search_info*) puser;

	zval *operand = iter->funcs->get_current_data(iter);
	if (UNEXPECTED(!operand || EG(exception))) {
		search_info->result = FAILURE;
		return ZEND_HASH_APPLY_STOP;
	}
	ZVAL_DEREF(operand);
	bool stop = fast_is_identical_function(operand, &search_info->target);
	if (stop) {
		search_info->found = true;
		return ZEND_HASH_APPLY_STOP;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static inline void teds_iterable_until(INTERNAL_FUNCTION_PARAMETERS, int stop_value, int negate) /* {{{ */
{
	zval *input;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ITERABLE(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(input)) {
		case IS_ARRAY:
			php_array_until(return_value, Z_ARRVAL_P(input), fci, fci_cache, stop_value, negate);
			return;
		case IS_OBJECT: {
			ZEND_ASSERT(instanceof_function(Z_OBJCE_P(input), zend_ce_traversable));
			php_iterator_until_info until_info;

			until_info.fci = fci;
			until_info.fcc = fci_cache;

			until_info.stop_value = stop_value;
			until_info.result = SUCCESS;
			until_info.found = false;

			if (spl_iterator_apply(input, teds_traversable_func_until, (void*)&until_info) == SUCCESS && until_info.result == SUCCESS) {
				RETURN_BOOL(!(until_info.found ^ stop_value ^ negate));
			}
			return;
		}
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

/* Determines whether the predicate holds for all elements in the array */
PHP_FUNCTION(all)
{
	teds_iterable_until(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ Determines whether the predicate holds for at least one element in the array */
PHP_FUNCTION(any)
{
	teds_iterable_until(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */

/* {{{ Determines whether the predicate holds for no elements of the array */
PHP_FUNCTION(none)
{
	teds_iterable_until(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 1);
}
/* }}} */

/* Determines whether the predicate holds for all elements in the array */
PHP_FUNCTION(includes_value)
{
	zval *input;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ITERABLE(input)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(input)) {
		case IS_ARRAY: {
			zval *entry;
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(input), entry) {
				ZVAL_DEREF(entry);
				if (fast_is_identical_function(value, entry)) {
					RETURN_TRUE;
				}
			} ZEND_HASH_FOREACH_END();
			RETURN_FALSE;
			return;
	    }
		case IS_OBJECT: {
			ZEND_ASSERT(instanceof_function(Z_OBJCE_P(input), zend_ce_traversable));
			php_traversable_search_info search_info;

			search_info.result = SUCCESS;
			search_info.found = false;
			ZVAL_COPY_VALUE(&search_info.target, value);

			if (spl_iterator_apply(input, teds_traversable_func_search_value, (void*)&search_info) == SUCCESS && search_info.result == SUCCESS) {
				RETURN_BOOL(search_info.found);
			}
			return;
		}
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

/* {{{ folds values */
PHP_FUNCTION(fold)
{
	zval *input;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval *initial = NULL;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ITERABLE(input)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_ZVAL(initial)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_COPY(return_value, initial);

	switch (Z_TYPE_P(input)) {
		case IS_ARRAY:
			teds_array_reduce(Z_ARRVAL_P(input), fci, fci_cache, return_value);
			return;
		case IS_OBJECT: {
			ZEND_ASSERT(instanceof_function(Z_OBJCE_P(input), zend_ce_traversable));
			teds_traversable_reduce(input, fci, fci_cache, return_value);
			return;
		}
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

static inline zend_array* teds_move_zend_array_from_entries(teds_strictset_entries *array) {
	const size_t size = array->nNumOfElements;
	ZEND_ASSERT(size > 0);

	/* Traversable can have more elements than HT_MAX_SIZE */
	HashTable *result = teds_new_array_check_overflow(size);
	zend_hash_real_init_packed(result);

	ZEND_HASH_FILL_PACKED(result) {
		zval *val;
		ZEND_ASSERT(array->nNumOfElements == array->nNumUsed);
		/* Because there are no removals, we can guarantee there are no gaps (IS_UNDEF) to skip over.
		 * Surprisingly, this is faster enough than TEDS_STRICTSET_FOREACH_VAL to notice.
		 * See benchmarks/unique_values.php */
		TEDS_STRICTSET_FOREACH_VAL_ASSERT_NO_GAPS(array, val) {
			ZEND_HASH_FILL_ADD(val);
		} TEDS_STRICTSET_FOREACH_END();
	} ZEND_HASH_FILL_END();

	teds_strictset_entries_release(array);

	return result;
}

static inline void teds_array_unique_values(HashTable *ht, zval *return_value)
{
	if (zend_hash_num_elements(ht) <= 1) {
		if (zend_hash_num_elements(ht) == 0) {
			RETURN_EMPTY_ARRAY();
		}
		HashTable *result = zend_new_array(1);
		HashPosition start = 0;
		zval *value = zend_hash_get_current_data_ex(ht, &start);
		ZEND_ASSERT(value != NULL);
		Z_TRY_ADDREF_P(value);
		zend_hash_next_index_insert(result, value);
		RETURN_ARR(result);
	}
	teds_strictset_entries array;
	teds_strictset_entries_init_from_array(&array, ht);
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}
	RETVAL_ARR(teds_move_zend_array_from_entries(&array));
}

static inline void teds_traversable_unique_values(zend_object *obj, zval *return_value)
{
	teds_strictset_entries array;
	teds_strictset_entries_init_from_traversable(&array, obj);
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}
	if (teds_strictset_entries_empty_capacity(&array)) {
		RETURN_EMPTY_ARRAY();
	}
	RETVAL_ARR(teds_move_zend_array_from_entries(&array));
}

/* {{{ extracts unique values from an iterable with a hash table */
PHP_FUNCTION(unique_values)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_array_unique_values(Z_ARRVAL_P(iterable), return_value);
			return;
		case IS_OBJECT: {
			ZEND_ASSERT(instanceof_function(Z_OBJCE_P(iterable), zend_ce_traversable));
			teds_traversable_unique_values(Z_OBJ_P(iterable), return_value);
			return;
		}
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

static HashTable *teds_binary_search_create_result(bool found, zval *key, zval *value)
{
	zend_array *return_ht = zend_new_array(3);
	{
		zval tmp;
		ZVAL_BOOL(&tmp, found);
		zend_hash_add_new(return_ht, TEDS_STR(found), &tmp);
	}
	/* Caller should increment references */
	zend_hash_add_new(return_ht, ZSTR_KNOWN(ZEND_STR_KEY), key);
	ZVAL_DEREF(value);
	Z_TRY_ADDREF_P(value);
	zend_hash_add_new(return_ht, ZSTR_KNOWN(ZEND_STR_VALUE), value);
	return return_ht;
}

#if PHP_VERSION_ID >= 80200
static zend_always_inline HashTable *teds_binary_search_for_zvals(HashTable *ht, zval *target, const bool use_key, const bool have_callback, zend_fcall_info *fci, zend_fcall_info_cache *fci_cache, zval *const args, zval *const retval)
{
	zval *const orig_start = ht->arPacked;
	zval *start = orig_start;
	zval *end = start + ht->nNumUsed;
	zval *largest_smaller_zval_ptr = NULL;
packed_search_start:
	while (start < end) {
		zval *const mid_orig = start + (end - start) / 2;
		zval *mid = mid_orig; /* mid <= start, mid < end */
		while (Z_TYPE_P(mid) == IS_UNDEF) {
			mid++;
			if (mid >= end) {
				end = mid_orig;
				goto packed_search_start;
			}
		}
		zend_long comparison_result;
		if (have_callback) {
			ZVAL_COPY(&args[0], target);
			zval operator;
			if (use_key) {
				ZVAL_LONG(&operator, mid - orig_start);
			} else {
				ZVAL_COPY_DEREF(&operator, mid);
			}
			ZVAL_COPY_VALUE(&args[1], &operator);

			zend_result result = zend_call_function(fci, fci_cache);
			zval_ptr_dtor(&operator);
			zval_ptr_dtor(target);
			if (UNEXPECTED(result == FAILURE)) {
				return NULL;
			}
			comparison_result = zval_get_long(retval);
			zval_ptr_dtor(retval);
		} else {
			if (use_key) {
				zval key_zv;
				ZVAL_LONG(&key_zv, mid - orig_start);
				comparison_result = zend_compare(target, &key_zv);
			} else {
				comparison_result = zend_compare(target, mid);
			}
		}
		if (UNEXPECTED(EG(exception))) {
			return NULL;
		}
		if (comparison_result == 0) {
			zval tmp;
			ZVAL_LONG(&tmp, mid - orig_start);
			return teds_binary_search_create_result(true, &tmp, mid);
		}
		if (comparison_result > 0) {
			largest_smaller_zval_ptr = mid;
			start = mid + 1;
		} else {
			end = mid_orig;
		}
	}
	if (largest_smaller_zval_ptr == NULL) {
		return NULL;
	}
	zval tmp;
	ZVAL_LONG(&tmp, largest_smaller_zval_ptr - orig_start);
	return teds_binary_search_create_result(false, &tmp, largest_smaller_zval_ptr);
}
#endif

static zend_always_inline HashTable *teds_binary_search_for_bucket(HashTable *ht, zval *target, const bool use_key, const bool have_callback, zend_fcall_info *fci, zend_fcall_info_cache *fci_cache, zval *const args, zval *const retval)
{
	Bucket *start = ht->arData;
	Bucket *end = start + ht->nNumUsed;
	Bucket *largest_smaller_bucket = NULL;
bucket_search_start:
	while (start < end) {
		Bucket *const mid_orig = start + (end - start) / 2;
		Bucket *mid = mid_orig; /* mid <= start, mid < end */
		while (Z_TYPE(mid->val) == IS_UNDEF) {
			mid++;
			if (mid >= end) {
				end = mid_orig;
				goto bucket_search_start;
			}
		}
		zend_long comparison_result;
		if (have_callback) {
			ZVAL_COPY(&args[0], target);
			zval operator;
			if (use_key) {
				if (mid->key) {
					zend_string_addref(mid->key);
					ZVAL_STR(&operator, mid->key);
				} else {
					ZVAL_LONG(&operator, mid->h);
				}
			} else {
				ZVAL_COPY_DEREF(&operator, &mid->val);
			}
			ZVAL_COPY_VALUE(&args[1], &operator);

			zend_result result = zend_call_function(fci, fci_cache);
			zval_ptr_dtor(&operator);
			zval_ptr_dtor(target);
			if (UNEXPECTED(result == FAILURE)) {
				return NULL;
			}
			comparison_result = zval_get_long(retval);
			zval_ptr_dtor(retval);
		} else {
			if (use_key) {
				zval key_zv;
				if (mid->key) {
					ZVAL_STR(&key_zv, mid->key);
				} else {
					ZVAL_LONG(&key_zv, mid->h);
				}
				comparison_result = zend_compare(target, &key_zv);
			} else {
				comparison_result = zend_compare(target, &mid->val);
			}
		}
		if (UNEXPECTED(EG(exception))) {
			return NULL;
		}
		if (comparison_result == 0) {
			zval tmp;
			if (mid->key) {
				zend_string_addref(mid->key);
				ZVAL_STR(&tmp, mid->key);
			} else {
				ZVAL_LONG(&tmp, mid->h);
			}
			return teds_binary_search_create_result(true, &tmp, &mid->val);
		}
		if (comparison_result > 0) {
			largest_smaller_bucket = mid;
			start = mid + 1;
		} else {
			end = mid_orig;
		}
	}
	if (largest_smaller_bucket == NULL) {
		return NULL;
	}
	zval tmp;
	if (largest_smaller_bucket->key) {
		zend_string_addref(largest_smaller_bucket->key);
		ZVAL_STR(&tmp, largest_smaller_bucket->key);
	} else {
		ZVAL_LONG(&tmp, largest_smaller_bucket->h);
	}
	return teds_binary_search_create_result(false, &tmp, &largest_smaller_bucket->val);
}

/* Perform a binary search on a sorted array (Assumed to be sorted by value and/or key, depending on $flags) */
PHP_FUNCTION(binary_search)
{
	HashTable *ht;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	zval *target;
	zend_bool use_key = false;
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_ARRAY_HT(ht)
		Z_PARAM_ZVAL(target)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fci_cache)
		Z_PARAM_BOOL(use_key)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_hash_num_elements(ht) > 0) {
		bool have_callback = false;
		zval retval;
		zval args[2];
		if (ZEND_FCI_INITIALIZED(fci)) {
			have_callback = true;
			fci.retval = &retval;
			fci.param_count = 2;
			fci.params = args;
		}
		HashTable *return_ht;
#if PHP_VERSION_ID >= 80200
		if (HT_IS_PACKED(ht)) {
			return_ht = teds_binary_search_for_zvals(ht, target, use_key, have_callback, &fci, &fci_cache, args, &retval);
		}
		else
#endif
		{
			return_ht = teds_binary_search_for_bucket(ht, target, use_key, have_callback, &fci, &fci_cache, args, &retval);
		}
		if (return_ht != NULL) {
			RETURN_ARR(return_ht);
		}
		if (UNEXPECTED(EG(exception))) {
			RETURN_THROWS();
		}
	}

	zval tmp;
	ZVAL_NULL(&tmp);
	RETURN_ARR(teds_binary_search_create_result(false, &tmp, &tmp));
}
/* }}} */

// Source: php_array_find from php-src/ext/standard/array.c
static zend_always_inline void teds_array_find(HashTable *htbl, zend_fcall_info fci, zend_fcall_info_cache fci_cache, zval* return_value, zval *default_value) /* {{{ */
{
	zval retval;
	zval *operand;

	if (zend_hash_num_elements(htbl) > 0) {
		fci.retval = &retval;
		fci.param_count = 1;

		ZEND_HASH_FOREACH_VAL(htbl, operand) {
			fci.params = operand;
			Z_TRY_ADDREF_P(operand);

			if (zend_call_function(&fci, &fci_cache) == SUCCESS) {
				bool found = zend_is_true(&retval);
				if (found) {
					ZVAL_COPY_VALUE(return_value, operand);
					return;
				} else {
					zval_ptr_dtor(operand);
				}
			} else {
				zval_ptr_dtor(operand);
				return;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (default_value) {
		ZVAL_COPY(return_value, default_value);
	} else {
		ZVAL_NULL(return_value);
	}
}
/* }}} */

typedef struct {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval *return_value;
	bool found;
} traversable_find_data;

static int teds_traversable_find_elem(zend_object_iterator *iter, void *puser) /* {{{ */
{
	zval retval;
	traversable_find_data *find_data = puser;
	zend_fcall_info *fci = &find_data->fci;
	ZEND_ASSERT(ZEND_FCI_INITIALIZED(*fci));

	zval *operand = iter->funcs->get_current_data(iter);
	if (UNEXPECTED(!operand || EG(exception))) {
		return ZEND_HASH_APPLY_STOP;
	}
	ZVAL_DEREF(operand);
    // Treat this as a 1-element array.
	fci->params = operand;
	fci->retval = &retval;
	Z_TRY_ADDREF_P(operand);
	int result = zend_call_function(&find_data->fci, &find_data->fcc);
	if (UNEXPECTED(result == FAILURE || EG(exception))) {
		return ZEND_HASH_APPLY_STOP;
	}
	fci->retval = &retval;
	bool found = zend_is_true(&retval);
	zval_ptr_dtor(&retval);
	if (UNEXPECTED(EG(exception))) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (found) {
		ZVAL_COPY_VALUE(find_data->return_value, operand);
		find_data->found = 1;
		return ZEND_HASH_APPLY_STOP;
	}
	zval_ptr_dtor(operand);
	return ZEND_HASH_APPLY_KEEP;
}

static zend_always_inline void teds_traversable_find(zval *obj, zend_fcall_info fci, zend_fcall_info_cache fci_cache, zval* return_value, zval *default_value) /* {{{ */
{
	traversable_find_data find_data;
	find_data.fci = fci;
	find_data.fci.param_count = 1;
	find_data.fcc = fci_cache;
	find_data.return_value = return_value;
	find_data.found = 0;

	if (spl_iterator_apply(obj, teds_traversable_find_elem, (void*)&find_data) != SUCCESS || EG(exception)) {
		return;
	}
	if (find_data.found) {
		return;
	}
	if (default_value) {
		ZVAL_COPY(return_value, default_value);
	} else {
		ZVAL_NULL(return_value);
	}
}
/* }}} */

/* {{{ Finds the first value or returns the default */
PHP_FUNCTION(find)
{
	zval *input;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval *default_value = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ITERABLE(input)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(input)) {
		case IS_ARRAY:
			teds_array_find(Z_ARRVAL_P(input), fci, fci_cache, return_value, default_value);
			return;
		case IS_OBJECT: {
			teds_traversable_find(input, fci, fci_cache, return_value, default_value);
			return;
		}
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

/* {{{ Get the value of the first element of the array */
PHP_FUNCTION(array_value_first)
{
	HashTable *target_hash;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(target_hash)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_hash_num_elements(target_hash) == 0) {
		RETURN_NULL();
	}

	HashPosition pos = 0;
	RETURN_COPY_DEREF(zend_hash_get_current_data_ex(target_hash, &pos));
}
/* }}} */

/* {{{ Get the value of the last element of the array */
PHP_FUNCTION(array_value_last)
{
	HashTable *target_hash;
	HashPosition pos;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(target_hash)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_hash_num_elements(target_hash) == 0) {
		RETURN_NULL();
	}
	zend_hash_internal_pointer_end_ex(target_hash, &pos);
	RETURN_COPY_DEREF(zend_hash_get_current_data_ex(target_hash, &pos));
}
/* }}} */

#define SPACESHIP_OP(n1, n2) ((n1) < (n2) ? -1 : (n1) > (n2) ? 1 : 0)

static zend_always_inline int teds_stable_compare_wrap(const void *v1, const void *v2) {
	return teds_stable_compare(v1, v2);
}

zend_long teds_stable_compare(const zval *v1, const zval *v2) {
	ZVAL_DEREF(v1);
	ZVAL_DEREF(v2);
	const zend_uchar t1 = Z_TYPE_P(v1);
	const zend_uchar t2 = Z_TYPE_P(v2);
	if (t1 != t2) {
		if (((1 << t1) | (1 << t2)) & ~((1 << IS_LONG) | (1 << IS_DOUBLE))) {
			/* At least one type is not a long or a double. */
			return t1 < t2 ? -1 : 1;
		}
		if (t1 == IS_DOUBLE) {
			ZEND_ASSERT(t2 == IS_LONG);

			zend_long i2 = Z_LVAL_P(v2);
			/* Windows long double is an alias of double. */
			double n1 = Z_DVAL_P(v1);
			double n2 = i2;
			if (n1 != n2) {
				return n1 < n2 ? -1 : 1;
			}
			if (EXPECTED(i2 == (zend_long)n1)) {
				/* long before double */
				return 1;
			}
			if (i2 > 0) {
				zend_ulong i1 = (zend_ulong)n1;
				return i1 < (zend_ulong) i2 ? -1 : 1;
			} else {
				zend_ulong i1 = (zend_ulong)-n1;
				return i1 < (zend_ulong) -i2 ? 1 : -1;
			}
		} else {
			ZEND_ASSERT(t1 == IS_LONG);
			ZEND_ASSERT(t2 == IS_DOUBLE);

			zend_long i1 = Z_LVAL_P(v1);
			double n1 = i1;
			double n2 = Z_DVAL_P(v2);
			if (n1 != n2) {
				return n2 < n1 ? 1 : -1;
			}
			if (EXPECTED(i1 == (zend_long)n2)) {
				return -1;
			}
			if (i1 > 0) {
				zend_ulong i2 = (zend_ulong)n2;
				return ((zend_ulong)i1) < i2 ? -1 : 1;
			} else {
				zend_ulong i2 = (zend_ulong)-n2;
				return ((zend_ulong)-i1) < i2 ? 1 : -1;
			}
		}
	}
	switch (t1) {
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			return 0;
		case IS_LONG:
			return SPACESHIP_OP(Z_LVAL_P(v1), Z_LVAL_P(v2));
		case IS_DOUBLE: {
			double n1 = Z_DVAL_P(v1);
			double n2 = Z_DVAL_P(v2);
			if (n1 == n2) {
				return 0;
			}
			if (UNEXPECTED(teds_is_nan(n2))) {
				/* Treat INF as smaller than NAN */
				return teds_is_nan(n1) ? 0 : -1;
			}
			return n1 < n2 ? -1 : 1;
		}
		case IS_STRING: {
			int result = zend_binary_zval_strcmp((zval *)v1, (zval *)v2);
			/* Avoid platform dependence, zend_binary_zval_strcmp is different on Windows */
			return SPACESHIP_OP(result, 0);
		}
		case IS_ARRAY:  {
			HashTable *h1 = Z_ARR_P(v1);
			HashTable *h2 = Z_ARR_P(v2);
			return zend_hash_compare(h1, h2, teds_stable_compare_wrap, true);
		}
		case IS_OBJECT: {
			/* Sort by class name, then by object handle, to group objects of the same class together. */
			zend_object *o1 = Z_OBJ_P(v1);
			zend_object *o2 = Z_OBJ_P(v2);
			if (o1 == o2) {
				return 0;
			}
			if (o1->ce != o2->ce) {
				zend_string *c1 = o1->ce->name;
				zend_string *c2 = o2->ce->name;
				int result = zend_binary_strcmp(ZSTR_VAL(c1), ZSTR_LEN(c1), ZSTR_VAL(c2), ZSTR_LEN(c2));
				if (result != 0) {
					return result < 0 ? -1 : 1;
				}
			}
			ZEND_ASSERT(Z_OBJ_HANDLE_P(v1) != Z_OBJ_HANDLE_P(v2)); /* Implied by o1 != o2 */
			return Z_OBJ_HANDLE_P(v1) < Z_OBJ_HANDLE_P(v2) ? -1 : 1;
		}
		case IS_RESOURCE:
			return SPACESHIP_OP(Z_RES_HANDLE_P(v1), Z_RES_HANDLE_P(v2));
		default:
			ZEND_UNREACHABLE();
	}
}

/* {{{ Compare two elements in a stable order. */
PHP_FUNCTION(stable_compare)
{
	zval *v1;
	zval *v2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(v1)
		Z_PARAM_ZVAL(v2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(teds_stable_compare(v1, v2));
}
/* }}} */

zend_long teds_strict_hash_array(HashTable *ht, teds_strict_hash_node *node) {
	zend_long num_key;
	zend_string *str_key;
	zval *field_value;

	uint64_t result = 1;

	if (zend_hash_num_elements(ht) == 0) {
		return 8313;
	}
	bool protected_recursion = false;

	teds_strict_hash_node new_node;
	teds_strict_hash_node *new_node_ptr = NULL;
	if (!(GC_FLAGS(ht) & GC_IMMUTABLE)) {
		if (UNEXPECTED(GC_IS_RECURSIVE(ht))) {
			for (zend_long i = 8712; node != NULL; node = node->prev, i++) {
				if (node->ht == ht) {
					return i;
				}
			}
		} else {
			protected_recursion = true;
			GC_PROTECT_RECURSION(ht);
		}
		new_node.prev = node;
		new_node.ht = ht;
		new_node_ptr = &new_node;
	}

	/* teds_strict_hash_inner has code to dereference IS_INDIRECT/IS_REFERENCE,
	 * but IS_INDIRECT is probably impossible as of php 8.1's removal of direct access to $GLOBALS? */
	ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, field_value) {
		/* str_key is in a hash table meaning the hash was already computed. */
		result += str_key ? ZSTR_H(str_key) : (zend_ulong) num_key;
		zend_long field_hash = teds_strict_hash_inner(field_value, new_node_ptr);
		result += (field_hash + (result << 7));
		result = teds_inline_hash_of_uint64(result);
	} ZEND_HASH_FOREACH_END();

	if (protected_recursion) {
		GC_UNPROTECT_RECURSION(ht);
	}
	return result;
}

/* This comparison function for zend_hash is expected to return 0 on identity, and non zero otherwise.
 * teds_is_identical_or_both_nan returns non-zero on identity and 0 otherwise.
 *
 * The function signature must match `int (zval *, zval *)`
 */
int teds_hash_zval_identical_or_both_nan_function(zval *op1, zval *op2) {
	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);
	return !teds_is_identical_or_both_nan(op1, op2);
}

/* {{{ Compute a strict hash that is the same number if $a === $b */
PHP_FUNCTION(strict_hash)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(teds_strict_hash(value));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(teds)
{
	teds_register_interfaces();

	PHP_MINIT(teds_bitset)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_deque)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_immutablekeyvaluesequence)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_immutablesequence)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_intvector)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_keyvaluevector)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_lowmemoryvector)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_sortedstrictmap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_sortedstrictset)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_stableheap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_stablesortedlistmap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_stablesortedlistset)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_strictmap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_strictset)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_vector)(INIT_FUNC_ARGS_PASSTHRU);
#define X(str) \
	TEDS_STR(str) = zend_new_interned_string( \
			zend_string_init(#str, sizeof(#str) - 1, 1));
	TEDS_STR_DEFS
#undef X
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(teds)
{
#define X(str) zend_string_release(TEDS_STR(str));
	TEDS_STR_DEFS
#undef X
	(void) type;
	(void) module_number;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(teds)
{
#if defined(ZTS) && defined(COMPILE_DL_TEDS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	(void) type;
	(void) module_number;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(teds)
{
	(void) ((ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU));
	php_info_print_table_start();
	php_info_print_table_header(2, "teds support", "enabled");
	php_info_print_table_row(2, "teds version", PHP_TEDS_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ teds_module_entry */
zend_module_entry teds_module_entry = {
	STANDARD_MODULE_HEADER,
	"teds",				/* Extension name */
	ext_functions,		/* zend_function_entry */
	PHP_MINIT(teds),	/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(teds), /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(teds),		/* PHP_RINIT - Request initialization */
	NULL,				/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(teds),		/* PHP_MINFO - Module info */
	PHP_TEDS_VERSION,	/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_TEDS
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(teds)
#endif
