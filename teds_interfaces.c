#include "Zend/zend_interfaces.h"
#include "php.h"
#include "teds_interfaces.h"
#include "teds_interfaces_arginfo.h"
#include "ext/json/php_json.h"

zend_class_entry *teds_ce_Values;
zend_class_entry *teds_ce_Collection;
zend_class_entry *teds_ce_ListInterface;


void teds_register_interfaces(void)
{
	teds_ce_Values = register_class_Teds_Values(zend_ce_traversable, zend_ce_countable);
	teds_ce_Collection = register_class_Teds_Collection(teds_ce_Values, zend_ce_arrayaccess);
	teds_ce_ListInterface = register_class_Teds_ListInterface(teds_ce_Collection);
}
