#ifndef TEDS_UTIL
#define TEDS_UTIL

#include "zend_types.h"
#define TEDS_NODE_RED 0
#define TEDS_NODE_BLACK 1

static zend_always_inline HashTable *teds_new_array_check_overflow(size_t n) {
#if SIZEOF_ZEND_LONG > 4
	if (UNEXPECTED(n >= HT_MAX_SIZE)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%lu * %zu + %zu)", n, sizeof(Bucket), sizeof(Bucket));
	}
#endif
	return zend_new_array(n);
}

/* Assume that on 64-bit systems, there won't be enough memory (9.22+18 bytes) to overflow */
static zend_always_inline bool teds_offset_within_size_t(const zend_long offset, const size_t len) {
	return ((zend_ulong) offset) < len && EXPECTED(SIZEOF_ZEND_LONG > 4 || offset >= 0);
}

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

/* Deliberately called instead of zend_objects_clone_members for final classes.
 * The properties HashTable is a duplicate of the data already in this value wasting memory,
 * we're trying to avoid it to the extent possible in get_properties_for.
 */
static zend_always_inline void teds_assert_object_has_empty_member_list(const zend_object *new_object) {
	ZEND_ASSERT(new_object->ce->ce_flags & ZEND_ACC_FINAL);
	ZEND_ASSERT(new_object->ce->default_properties_count == 0);
	ZEND_ASSERT(new_object->ce->clone == NULL);
	ZEND_ASSERT(new_object->properties == NULL);
}

static zend_always_inline size_t teds_is_pow2(size_t nSize) {
	return (nSize & (nSize - 1)) == 0 && nSize > 0;
}

static zend_always_inline size_t teds_next_pow2_capacity(size_t nSize, size_t min) {
	if (nSize < min) {
		return min;
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
#define TEDS_DEQUE_MIN_CAPACITY 4
/* Based on zend_hash_check_size which supports 32-bit integers. Finds the next largest power of 2. */
static zend_always_inline size_t teds_deque_next_pow2_capacity(size_t nSize) {
	return teds_next_pow2_capacity(nSize, TEDS_DEQUE_MIN_CAPACITY);
}

#define TEDS_STRICTSET_MIN_CAPACITY 8
#define TEDS_STRICTMAP_MIN_CAPACITY TEDS_STRICTSET_MIN_CAPACITY
static zend_always_inline size_t teds_strictset_next_pow2_capacity(size_t nSize) {
	return teds_next_pow2_capacity(nSize, TEDS_STRICTSET_MIN_CAPACITY);
}
#define teds_strictmap_next_pow2_capacity teds_strictset_next_pow2_capacity

/* TODO remove when rewriting as binary tree */
#define teds_sortedstrictmap_next_pow2_capacity teds_deque_next_pow2_capacity
#define teds_sortedstrictset_next_pow2_capacity teds_deque_next_pow2_capacity
/* TODO: Allow arbitrary capacities for stablesortedlistset,
 * this restriction is copied over from StrictMap. */
#define teds_stablesortedlistset_next_pow2_capacity teds_deque_next_pow2_capacity
#define teds_stablesortedlistmap_next_pow2_capacity teds_deque_next_pow2_capacity

#endif
