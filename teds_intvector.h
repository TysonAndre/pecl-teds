/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_INTVECTOR_H
#define TEDS_INTVECTOR_H

extern zend_class_entry *teds_ce_IntVector;
extern zend_class_entry *teds_ce_SortedIntVectorSet;

PHP_MINIT_FUNCTION(teds_intvector);

/* TODO reuse in LowMemoryVector */
#define TEDS_PROMOTE_INT_TYPE_AND_RETURN(int_smaller, entries_smaller, int_larger, entries_larger, TEDS_INTVECTOR__TYPE_LARGER) do { \
		ZEND_ASSERT(sizeof(int_smaller) < sizeof(int_larger)); \
		array->type_tag = TEDS_INTVECTOR__TYPE_LARGER; \
		int_smaller *const original_entries = array->entries_smaller; \
		const int_smaller *src = original_entries; \
		const size_t size = array->size; \
		const size_t capacity = size >= 2 ? size * 2 : 4; \
		array->capacity = capacity; \
		int_larger *const entries_larger  = safe_emalloc(capacity, sizeof(int_larger), 0); \
		const int_larger *const end = entries_larger + size; \
		int_larger *dst = entries_larger; \
		array->entries_larger = entries_larger; \
		while (dst < end) { \
			*dst++ = *src++; \
		} \
		if (array->capacity > 0) { \
			efree(original_entries); \
		} \
		return; \
	} while (0)

static zend_always_inline zend_string *teds_create_string_from_entries_int8(const char *raw, const size_t len) {
	return zend_string_init(raw, len, 0);
}

static zend_always_inline zend_string *teds_create_string_from_entries_int16(const char *raw, const size_t len) {
#ifdef WORDS_BIGENDIAN
	zend_string *const result = zend_string_alloc(len * sizeof(int16_t), 0);
	uint16_t *dst = (uint16_t *)ZSTR_VAL(result);
	const uint16_t *src = (const uint16_t *)raw;
	const uint16_t *const end = src + len;
	for (;src < end; src++, dst++) {
		const uint16_t v = *src;
		*dst = teds_bswap_16(*src);
	}
	*(char *)dst = '\0';
	return result;
#else
	return zend_string_init(raw, len * sizeof(int16_t), 0);
#endif
}

static zend_always_inline zend_string *teds_create_string_from_entries_int32(const char *raw, const size_t len) {
#ifdef WORDS_BIGENDIAN
	zend_string *const result = zend_string_alloc(len * sizeof(int32_t), 0);
	uint32_t *dst = (uint32_t *)ZSTR_VAL(result);
	const uint32_t *src = (const uint32_t *)raw;
	const uint32_t *const end = src + len;
	for (;src < end; src++, dst++) {
		/* This compiles down to a bswap assembly instruction in optimized builds */
		*dst = bswap_32(*src);
	}
	*(char *)dst = '\0';
	return result;
#else
	return zend_string_init(raw, len * sizeof(int32_t), 0);
#endif
}

static zend_always_inline zend_string *teds_create_string_from_entries_int64(const char *raw, const size_t len) {
#ifdef WORDS_BIGENDIAN
	zend_string *const result = zend_string_alloc(len * sizeof(int64_t), 0);
	uint64_t *dst = (uint64_t *)ZSTR_VAL(result);
	const uint64_t *src = (const uint64_t *)raw;
	const uint64_t *const end = src + len;
	for (;src < end; src++, dst++) {
		/* This compiles down to a bswap assembly instruction in optimized builds */
		*dst = bswap_64(*src);
	}
	*(char *)dst = '\0';
	return result;
#else
	return zend_string_init(raw, len * sizeof(int64_t), 0);
#endif
}

#endif	/* TEDS_INTVECTOR_H */
