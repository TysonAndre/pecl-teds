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

static zend_always_inline zend_long teds_get_offset(const zval *offset) {
	try_again:
	switch (Z_TYPE_P(offset)) {
		case IS_STRING: {
			zend_ulong index;
			if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), index)) {
				return (zend_long) index;
			}
			break;
		}
		case IS_DOUBLE:
#if PHP_VERSION_ID >= 80100
			return zend_dval_to_lval_safe(Z_DVAL_P(offset));
#else
			return zend_dval_to_lval(Z_DVAL_P(offset));
#endif
		case IS_LONG:
			return Z_LVAL_P(offset);
		case IS_FALSE:
			return 0;
		case IS_TRUE:
			return 1;
		case IS_REFERENCE:
			offset = Z_REFVAL_P(offset);
			goto try_again;
		case IS_RESOURCE:
			/* The type changed from int to zend_long in php 8.1. This is not really important in practice. */
			zend_error(E_WARNING, "Resource ID#" ZEND_LONG_FMT " used as offset, casting to integer (" ZEND_LONG_FMT ")",
				(zend_long)Z_RES_HANDLE_P(offset), (zend_long)Z_RES_HANDLE_P(offset));
			return Z_RES_HANDLE_P(offset);
	}

	zend_type_error("Illegal offset type %s", zend_zval_type_name(offset));
	return 0;
}

#define CONVERT_OFFSET_TO_LONG_OR_THROW(index, zv) do { \
	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) { \
		index = teds_get_offset(offset_zv); \
		if (UNEXPECTED(EG(exception))) { \
			return; \
		} \
	} else { \
		index = Z_LVAL_P(offset_zv); \
	} \
} while(0)

#define CONVERT_OFFSET_TO_LONG_OR_THROW_RETURN_NULLPTR(index, zv) do { \
	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) { \
		index = teds_get_offset(offset_zv); \
		if (UNEXPECTED(EG(exception))) { \
			return NULL; \
		} \
	} else { \
		index = Z_LVAL_P(offset_zv); \
	} \
} while(0)

#endif	/* TEDS_H */
