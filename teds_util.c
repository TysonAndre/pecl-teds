#include "teds_util.h"

HashTable* teds_noop_empty_array_get_properties(zend_object *obj) {
	(void)obj;
	/* Thankfully, anything using Z_OBJPROP_P for infinite recursion detection (var_export) won't need to worry about infinite recursion, all fields are integers and there are no properties. */
	/* debug_zval_dump DEBUG purpose requires null or a refcounted array. */
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

