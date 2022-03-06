#ifndef TEDS_H
#define TEDS_H

#include "zend_API.h"
#include "ext/spl/spl_engine.h"
#include "teds_bswap.h"

#define TEDS_MINIT_IGNORE_UNUSED() do { (void) type; (void) module_number; } while (0)
/* Avoid unused parameter warning with -Wunused */
#define TEDS_RETURN_VOID() do { (void) return_value; return; } while (0)

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

#define CONVERT_OFFSET_TO_LONG_OR_THROW(index, offset_zv) do { \
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

#define THROW_IF_OFFSET_NOT_LONG(zv) do { \
	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) { \
		teds_get_offset(offset_zv); \
		if (UNEXPECTED(EG(exception))) { \
			return NULL; \
		} \
	} \
} while(0)

typedef struct _teds_strict_hash_node {
	zend_array *ht;
	struct _teds_strict_hash_node *prev;
} teds_strict_hash_node;

zend_long teds_strict_hash_array(HashTable *ht, teds_strict_hash_node *node);

inline static uint64_t teds_convert_double_to_uint64_t(double *value) {
	double tmp = *value;
	if (UNEXPECTED(tmp == 0)) {
		/* Signed positive and negative 0 have different bits. However, $signedZero === $signedNegativeZero in php and many other languages. */
		return 0;
	}
	if (UNEXPECTED(tmp != tmp)) {
		/*
		 * See https://en.wikipedia.org/wiki/NaN
		 * For nan, the 12 most significant bits are:
		 * - 1 sign bit (0 or 1)
		 * - 11 sign bits
		 * (and at least one of the significand bits must be non-zero)
		 *
		 * Here, 0xff is the most significant byte with the sign and part of the exponent,
		 * and 0xf8 is the second most significant byte with part of the exponent and significand.
		 *
		 * Return an arbitrary choice of 0xff f_, with bytes in the reverse order.
		 */
		return 0xf8ff;
	}
	uint8_t *data = (uint8_t *)value;
#ifndef WORDS_BIGENDIAN
	return
		(((uint64_t)data[0]) << 56) |
		(((uint64_t)data[1]) << 48) |
		(((uint64_t)data[2]) << 40) |
		(((uint64_t)data[3]) << 32) |
		(((uint64_t)data[4]) << 24) |
		(((uint64_t)data[5]) << 16) |
		(((uint64_t)data[6]) << 8) |
		(((uint64_t)data[7]));
#else
	return
		(((uint64_t)data[7]) << 56) |
		(((uint64_t)data[6]) << 48) |
		(((uint64_t)data[5]) << 40) |
		(((uint64_t)data[4]) << 32) |
		(((uint64_t)data[3]) << 24) |
		(((uint64_t)data[2]) << 16) |
		(((uint64_t)data[1]) << 8) |
		(((uint64_t)data[0]));
#endif
}

static zend_always_inline zend_long teds_strict_hash_inner(zval *value, teds_strict_hash_node *node) {
again:
	switch (Z_TYPE_P(value)) {
		case IS_NULL:
			return 8310;
		case IS_FALSE:
			return 8311;
		case IS_TRUE:
			return 8312;
		case IS_LONG:
			return Z_LVAL_P(value);
		case IS_DOUBLE:
			return teds_convert_double_to_uint64_t(&Z_DVAL_P(value)) + 8315;
		case IS_STRING:
			/* Compute the hash if needed, return it */
			return ZSTR_HASH(Z_STR_P(value));
		case IS_ARRAY:
			return teds_strict_hash_array(Z_ARR_P(value), node);
		case IS_OBJECT:
		    /* Avoid hash collisions between objects and small numbers. */
			return Z_OBJ_HANDLE_P(value) + 31415926;
		case IS_RESOURCE:
			return Z_RES_HANDLE_P(value) + 27182818;
		case IS_REFERENCE:
			value = Z_REFVAL_P(value);
			goto again;
		case IS_INDIRECT:
			value = Z_INDIRECT_P(value);
			goto again;
		default:
			ZEND_UNREACHABLE();
	}
}

/* This assumes that pointers differ in low addresses rather than high addresses.
 * Copied from code written for igbinary. */
static zend_always_inline uint64_t teds_inline_hash_of_uint64(uint64_t orig) {
	/* Works best when data that frequently differs is in the least significant bits of data */
	uint64_t data = orig * 0x5e2d58d8b3bce8d9;
	/* bswap is a single assembly instruction on recent compilers/platforms */
	return bswap_64(data);
}

/* {{{ Generate a hash */
static zend_always_inline zend_long teds_strict_hash(zval *value) {
	uint64_t raw_data = teds_strict_hash_inner(value, NULL);
	return teds_inline_hash_of_uint64(raw_data);
}
/* }}} */

static zend_always_inline uint32_t teds_strict_hash_uint32_t(zval *value) {
	return (uint32_t) (zend_ulong) teds_strict_hash(value);
}

zend_long teds_stable_compare(const zval *v1, const zval *v2);
int teds_stable_compare_wrap(const void *v1, const void *v2);

/* Check that the zval is not undefined and not a reference/indirect */
#define TEDS_ASSERT_ZVAL_IS_VALUE_TYPE(v) ZEND_ASSERT(Z_TYPE_P((v)) >= IS_NULL && Z_TYPE_P((v)) <= IS_RESOURCE)

#endif	/* TEDS_H */
