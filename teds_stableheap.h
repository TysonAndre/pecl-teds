/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STABLEHEAP_H
#define TEDS_STABLEHEAP_H

extern zend_class_entry *teds_ce_StableMinHeap;
extern zend_class_entry *teds_ce_StableMaxHeap;

PHP_MINIT_FUNCTION(teds_stableheap);

#endif	/* TEDS_STABLEHEAP_H */
