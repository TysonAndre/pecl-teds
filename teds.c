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
#include "zend_types.h"
#include "zend_smart_str.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "teds_immutablekeyvaluesequence.h"
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
	NULL,				/* zend_function_entry */
	PHP_MINIT(teds),		/* PHP_MINIT - Module initialization */
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
