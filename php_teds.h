/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_TEDS_H
# define PHP_TEDS_H

#include "teds_internaliterator.h"
#include "zend_long.h"

#if SIZEOF_ZEND_LONG > SIZEOF_SIZE_T
// See php-src/Zend/zend_range_check.h
// > Furthermore, by the current design, size_t can always
// > overflow zend_long.
// This error can happen when compiling php with a 32-bit compiler configuration but compiling the extension with a 64-bit compiler configuration.
#error Expected SIZEOF_ZEND_LONG <= SIZEOF_SIZE_T to be guaranteed by php-src.
#endif

/** Module entry of teds. */
extern zend_module_entry teds_module_entry;
#define phpext_teds_ptr &teds_module_entry

struct _teds_intrusive_dllist;

PHP_MINIT_FUNCTION(teds);

# define PHP_TEDS_VERSION "2.0.0"

# if defined(ZTS) && defined(COMPILE_DL_TEDS)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

void php_teds_ex(zval *struc, int level, smart_str *buf);

#endif	/* PHP_TEDS_H */
