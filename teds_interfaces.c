/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/
#include "Zend/zend_interfaces.h"
#include "php.h"
#include "teds_interfaces.h"
#include "teds_interfaces_arginfo.h"

zend_class_entry *teds_ce_Collection;
zend_class_entry *teds_ce_Sequence;
zend_class_entry *teds_ce_Map;
zend_class_entry *teds_ce_Set;

void teds_register_interfaces(void)
{
	teds_ce_Collection = register_class_Teds_Collection(zend_ce_traversable, zend_ce_countable);
	teds_ce_Sequence = register_class_Teds_Sequence(teds_ce_Collection, zend_ce_arrayaccess);
	teds_ce_Map = register_class_Teds_Map(teds_ce_Collection, zend_ce_arrayaccess);
	teds_ce_Set = register_class_Teds_Set(teds_ce_Collection);
}
