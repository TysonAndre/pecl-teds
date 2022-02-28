#include "teds_util.h"

HashTable* teds_noop_empty_array_get_properties(zend_object *obj) {
	(void)obj;
	/* Thankfully, anything using Z_OBJPROP_P for infinite recursion detection (var_export) won't need to worry about infinite recursion, all fields are integers and there are no properties. */
	return (HashTable*)&zend_empty_array;
}

HashTable* teds_noop_get_gc(zend_object *obj, zval **table, int *n) {
	/* Zend/zend_gc.c does not initialize table or n. So we need to set n to 0 at minimum. */
	*n = 0;
	(void) table;
	(void) obj;
	/* Nothing needs to be garbage collected */
	return NULL;
}
