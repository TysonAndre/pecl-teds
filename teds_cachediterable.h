/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_CACHEDITERABLE_H
#define TEDS_CACHEDITERABLE_H

#include "teds.h"

extern zend_class_entry *teds_ce_CachedIterable;

PHP_MINIT_FUNCTION(teds_cachediterable);

extern TEDS_COUNT_ELEMENTS_RETURN_TYPE teds_size_t_count_elements(zend_object *object, zend_long *count);

#endif /* TEDS_CACHEDITERABLE_H */
