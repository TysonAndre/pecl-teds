#include "teds_util.h"

/* Override get_properties_for and use the default implementation of get_properties. See https://github.com/php/php-src/issues/9697#issuecomment-1273613175 */
HashTable* teds_noop_empty_array_get_properties_for(zend_object *obj, zend_prop_purpose purpose) {
	(void)obj;
	(void)purpose;
	return NULL;
}

HashTable* teds_noop_get_gc(zend_object *obj, zval **table, int *n) {
	/* Zend/zend_gc.c does not initialize table or n. So we need to set n to 0 at minimum. */
	*n = 0;
	(void) table;
	(void) obj;
	/* Nothing needs to be garbage collected */
	return NULL;
}

HashTable *teds_internaliterator_get_gc(zend_object_iterator *iter, zval **table, int *n)
{
	*table = &iter->data;
	*n = 1;
	return NULL;
}

const zval teds_empty_entry_list[1];
