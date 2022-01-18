/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c but has lower overhead (when size is known) and is more efficient to enstableheap and remove elements from the end of the list */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds.h"
#include "teds_stableheap_arginfo.h"
#include "teds_stableheap.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_heap.h"

#include <stdbool.h>

zend_class_entry *teds_ce_StableMinHeap;
zend_class_entry *teds_ce_StableMaxHeap;

PHP_METHOD(Teds_StableMaxHeap, compare)
{
	zval *v1;
	zval *v2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(v1)
		Z_PARAM_ZVAL(v2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(teds_stable_compare(v1, v2));
}

PHP_METHOD(Teds_StableMinHeap, compare)
{
	zval *v1;
	zval *v2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(v1)
		Z_PARAM_ZVAL(v2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(teds_stable_compare(v2, v1));
}

PHP_MINIT_FUNCTION(teds_stableheap)
{
	teds_ce_StableMinHeap = register_class_Teds_StableMinHeap(spl_ce_SplMinHeap);
	teds_ce_StableMaxHeap = register_class_Teds_StableMaxHeap(spl_ce_SplMinHeap);

	return SUCCESS;
}
