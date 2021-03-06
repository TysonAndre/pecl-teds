/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_INTERFACES_H
#define TEDS_INTERFACES_H

#include "Zend/zend_types.h"

extern zend_class_entry *teds_ce_Collection;
extern zend_class_entry *teds_ce_Sequence;
extern zend_class_entry *teds_ce_Map;
extern zend_class_entry *teds_ce_Set;

void teds_register_interfaces(void);

#endif	/* TEDS_INTERFACES_H */
