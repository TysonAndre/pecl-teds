/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                 |
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
#include "teds_immutablekeyvaluesequence.h"
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

static inline void php_array_until(zval *return_value, HashTable *htbl, zend_fcall_info fci, zend_fcall_info_cache fci_cache, int stop_value) /* {{{ */
{
	zval args[1];
	zend_bool have_callback = 0;
	zval *operand;
	zval retval;
	int result;

	if (zend_hash_num_elements(htbl) == 0) {
		RETURN_BOOL(!stop_value);
	}

	if (ZEND_FCI_INITIALIZED(fci)) {
		have_callback = 1;
		fci.retval = &retval;
		fci.param_count = 1;
		fci.params = args;
	}

	ZEND_HASH_FOREACH_VAL(htbl, operand) {
		if (have_callback) {
			zend_bool retval_true;
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
				RETURN_BOOL(stop_value);
			}
		} else {
			if (zend_is_true(operand) == stop_value) {
				RETURN_BOOL(stop_value);
			}
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_BOOL(!stop_value);
}
/* }}} */

typedef struct {
	zval                   *obj;
	int                    stop_value;
	int                    result;
	int                    found;
	zend_long              has_callback;
	zend_fcall_info        fci;
	zend_fcall_info_cache  fcc;
} php_iterator_until_info;

static int php_traversable_func_until(zend_object_iterator *iter, void *puser) /* {{{ */
{
	zend_fcall_info fci;
	zval retval;
	php_iterator_until_info *until_info = (php_iterator_until_info*) puser;
	int result;
	zend_bool stop;

	fci = until_info->fci;
	if (ZEND_FCI_INITIALIZED(fci)) {
		zval *operand = iter->funcs->get_current_data(iter);
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
		stop = zend_is_true(iter->funcs->get_current_data(iter)) == until_info->stop_value;
	}
	if (stop) {
		until_info->found = 1;
		return ZEND_HASH_APPLY_STOP;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static inline void php_iterable_until(INTERNAL_FUNCTION_PARAMETERS, int stop_value) /* {{{ */
{
	zval *input;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(input)) {
		case IS_ARRAY:
			php_array_until(return_value, Z_ARRVAL_P(input), fci, fci_cache, stop_value);
			return;
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(input), zend_ce_traversable)) {
				php_iterator_until_info until_info;

				until_info.obj = input;
				until_info.fci = fci;
				until_info.fcc = fci_cache;

				until_info.stop_value = stop_value;
				until_info.result = SUCCESS;
				until_info.found = 0;

				if (spl_iterator_apply(until_info.obj, php_traversable_func_until, (void*)&until_info) == SUCCESS && until_info.result == SUCCESS) {
					RETURN_BOOL(!(until_info.found ^ stop_value));
				}
				return;
			}
			/* fallthrough */
		default:
			zend_argument_type_error(1, "must be of type iterable, %s given", zend_zval_type_name(input));
			RETURN_THROWS();
			break;
	}
}

/* {{{ proto bool all(array input, ?callable predicate = null)
   Determines whether the predicate holds for all elements in the array */
PHP_FUNCTION(all)
{
	php_iterable_until(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool any(array input, ?callable predicate = null)
   Determines whether the predicate holds for at least one element in the array */
PHP_FUNCTION(any)
{
	php_iterable_until(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(teds)
{
	PHP_MINIT(teds_immutablekeyvaluesequence)(INIT_FUNC_ARGS_PASSTHRU);
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
