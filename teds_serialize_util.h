#ifndef TEDS_SERIALIZE_UTIL_H
#define TEDS_SERIALIZE_UTIL_H

#include "Zend/zend_types.h"
#include "Zend/zend_hash.h"
#include "Zend/zend_portability.h"

static zend_always_inline HashTable *teds_create_serialize_pair(const uint8_t type, zval *data) {
	zval type_zv;
	ZVAL_LONG(&type_zv, type);
	return zend_new_pair(&type_zv, data);
}

static zend_always_inline uint16_t teds_serialize_read_uint16_t(const uint8_t *data) {
	return
		(data[0] << 0) +
		(data[1] << 8) +
		(data[2] << 16) +
		(data[3] << 24);
}

static zend_always_inline void teds_serialize_write_uint16_t(uint8_t *dst, uint16_t value)
{
	/* The compiler will optimize this to a write on little-endian platforms, bswap on big-endian platforms */
	dst[0] = (value >> 0) & 0xff;
	dst[1] = (value >> 8) & 0xff;
}

static zend_always_inline uint32_t teds_serialize_read_uint32_t(const uint8_t *data) {
	return
		(data[0] << 0) +
		(data[1] << 8) +
		(data[2] << 16) +
		(data[3] << 24);
}

static zend_always_inline void teds_serialize_write_uint32_t(uint8_t *dst, uint32_t value)
{
	/* The compiler will optimize this to a write on little-endian platforms, bswap on big-endian platforms */
	dst[0] = (value >> 0) & 0xff;
	dst[1] = (value >> 8) & 0xff;
	dst[2] = (value >> 16) & 0xff;
	dst[3] = (value >> 24) & 0xff;
}

#define TEDS_SERIALIZE_SIZE_TAG_64 255
#define TEDS_SERIALIZE_SIZE_TAG_32 254
#define TEDS_SERIALIZE_SIZE_TAG_16 253

static zend_always_inline uint8_t teds_serialize_write_variable_len_size(uint8_t *dst, uint32_t size)
{
	if (size < TEDS_SERIALIZE_SIZE_TAG_16) {
		*dst = size;
		return 1;
	}
	if (size < UINT16_MAX) {
		dst[0] = TEDS_SERIALIZE_SIZE_TAG_16;
		teds_serialize_write_uint16_t(dst + 1, size);
		return 3;
	}
	dst[0] = TEDS_SERIALIZE_SIZE_TAG_32;
	teds_serialize_write_uint32_t(dst + 1, size);
	return 5;
}

static zend_always_inline bool teds_serialize_read_variable_len_size(const uint8_t **const src, const uint8_t *const end, uint32_t *const out_size)
{
	const uint8_t *src_orig = *src;
	/* Zero-length strings would use only 1 byte for the length. 0x00. */
	if (UNEXPECTED(src_orig + 1 > end)) {
too_much_data:
		zend_throw_exception(spl_ce_RuntimeException, "Teds unserialize failed: not enough data for variable-length size", 0);
		return false;
	}

	const uint8_t size_or_tag = *src_orig;
	if (size_or_tag < TEDS_SERIALIZE_SIZE_TAG_16) {
		++*src;
		*out_size = size_or_tag;
		return true;
	}
	if (size_or_tag == TEDS_SERIALIZE_SIZE_TAG_16) {
		if (UNEXPECTED(src_orig + 3 >= end)) {
			goto too_much_data;
		}
		*src += 3;
		*out_size = teds_serialize_read_uint16_t(src_orig + 1);
		return true;
	}
	if (EXPECTED(size_or_tag == TEDS_SERIALIZE_SIZE_TAG_32)) {
		if (UNEXPECTED(src_orig + 5 >= end)) {
			goto too_much_data;
		}
		*src += 5;
		*out_size = teds_serialize_read_uint32_t(src_orig + 1);
		return true;
	}
	zend_throw_exception(spl_ce_RuntimeException, "Teds unserialize failed: unsupported size tag (64-bit sizes not supported)", 0);
	return false;
}
#endif
