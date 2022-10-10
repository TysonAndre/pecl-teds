/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_IMMUTABLEITERABLE_H
#define TEDS_IMMUTABLEITERABLE_H

#include "teds_zval_pair.h"

extern zend_class_entry *teds_ce_ImmutableIterable;

PHP_MINIT_FUNCTION(teds_immutableiterable);

void teds_build_properties_for_immutable_zval_pairs(HashTable *ht, zval_pair *entries, const uint32_t len);
zend_array *teds_zval_pairs_to_refcounted_pairs(zval_pair *entries, uint32_t len);

#endif	/* TEDS_IMMUTABLEITERABLE_H */
