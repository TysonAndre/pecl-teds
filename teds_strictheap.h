/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STRICTHEAP_H
#define TEDS_STRICTHEAP_H

extern zend_class_entry *teds_ce_StrictMinHeap;
extern zend_class_entry *teds_ce_StrictMaxHeap;

PHP_MINIT_FUNCTION(teds_strictheap);

#endif	/* TEDS_STRICTHEAP_H */
