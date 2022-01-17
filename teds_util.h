#ifndef TEDS_UTIL
#define TEDS_UTIL

#include "zend_types.h"

static inline zval *teds_zval_copy_range(const zval *original, size_t n) {
	const size_t bytes = n * sizeof(zval);
	zval *copy = emalloc(bytes);
	memcpy(copy, original, bytes);
	return copy;
}

static inline void teds_zval_dtor_range(zval *it, size_t n) {
	for (zval *end = it + n; it < end; it++) {
		zval_ptr_dtor(it);
	}
}

#define TEDS_DEQUE_MIN_CAPACITY 4
/* Based on zend_hash_check_size which supports 32-bit integers. Finds the next largest power of 2. */
static inline size_t teds_deque_next_pow2_capacity(size_t nSize) {
	if (nSize < TEDS_DEQUE_MIN_CAPACITY) {
		return TEDS_DEQUE_MIN_CAPACITY;
	}
	/* Note that for values such as 63 or 31 of the form ((2^n) - 1),
	 * subtracting and xor are the same things for numbers in the range of 0 to the max. */
#ifdef ZEND_WIN32
	unsigned long index;
#if SIZEOF_SIZE_T > 4
	if (BitScanReverse64(&index, nSize - 1)) {
		return 0x2u << ((63 - index) ^ 0x3f);
	}
#else
	if (BitScanReverse(&index, nSize - 1)) {
		return 0x2u << ((31 - index) ^ 0x1f);
	}
#endif
	/* nSize is ensured to be in the valid range, fall back to it
	 * rather than using an undefined bit scan result. */
	return nSize;
#elif (defined(__GNUC__) || __has_builtin(__builtin_clz))  && defined(PHP_HAVE_BUILTIN_CLZ)
#if SIZEOF_SIZE_T > SIZEOF_INT
	return 0x2u << (__builtin_clzl(nSize - 1) ^ (sizeof(long) * 8 - 1));
#else
	return 0x2u << (__builtin_clz(nSize - 1) ^ 0x1f);
#endif
#else
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
#if SIZEOF_SIZE_T > 4
	nSize |= (nSize >> 32);
#endif
	return nSize + 1;
#endif
}

#define teds_strictmap_next_pow2_capacity teds_deque_next_pow2_capacity
#define teds_strictset_next_pow2_capacity teds_deque_next_pow2_capacity
/* TODO remove when rewriting as binary tree */
#define teds_sortedstrictmap_next_pow2_capacity teds_deque_next_pow2_capacity

#endif
