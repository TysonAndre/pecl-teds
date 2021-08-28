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

#include <ctype.h>
#include "zend_interfaces.h"
#include "zend_types.h"
#include "zend_smart_str.h"
#include "ext/spl/spl_iterators.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#include "teds_deque.h"
#include "teds_immutablekeyvaluesequence.h"
#include "teds_immutablesequence.h"
#include "teds_keyvaluevector.h"
#include "teds_vector.h"

#include "teds_arginfo.h"
#include "php_teds.h"

#if PHP_VERSION_ID >= 80100
#include "zend_enum.h"
#endif

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
static zend_always_inline void php_array_reduce(HashTable *htbl, zend_fcall_info fci, zend_fcall_info_cache fci_cache, zval* return_value) /* {{{ */
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
			bool retval_true;
			ZVAL_COPY(&args[0], operand);

			/* Treat the operand like an array of size 1  */
			result = zend_call_function(&fci, &fci_cache);
			zval_ptr_dtor(operand);
			if (result == FAILURE) {
				return;
			}
			retval_true = zend_is_true(&retval);
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
			php_array_reduce(Z_ARRVAL_P(input), fci, fci_cache, return_value);
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

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(teds)
{
	PHP_MINIT(teds_deque)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_immutablekeyvaluesequence)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_immutablesequence)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_keyvaluevector)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(teds_vector)(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(teds)
{
#if defined(ZTS) && defined(COMPILE_DL_TEDS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(teds)
{
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
	NULL,				/* PHP_MSHUTDOWN - Module shutdown */
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
