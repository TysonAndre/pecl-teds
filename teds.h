#ifndef TEDS_H
#define TEDS_H

#include "zend_API.h"
#include "ext/spl/spl_engine.h"

static zend_always_inline void teds_convert_zval_list_to_php_array_list(zval *return_value, zval *entries, size_t len)
{
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (size_t i = 0; i < len; i++) {
			Z_TRY_ADDREF_P(&entries[i]);
			ZEND_HASH_FILL_ADD(&entries[i]);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

static zend_always_inline zend_long teds_get_offset(zval *offset_zv) {
	if (Z_TYPE_P(offset_zv) != IS_LONG) {
		return spl_offset_convert_to_long(offset_zv);
	} else {
		return Z_LVAL_P(offset_zv);
	}
}

#endif	/* TEDS_H */
