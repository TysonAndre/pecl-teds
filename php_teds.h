/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                 |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_TEDS_H
# define PHP_TEDS_H

/** Module entry of teds. */
extern zend_module_entry teds_module_entry;
#define phpext_teds_ptr &teds_module_entry

PHP_MINIT_FUNCTION(teds);

# define PHP_TEDS_VERSION "0.1.0dev"

# if defined(ZTS) && defined(COMPILE_DL_TEDS)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

void php_teds_ex(zval *struc, int level, smart_str *buf);

#endif	/* PHP_TEDS_H */
