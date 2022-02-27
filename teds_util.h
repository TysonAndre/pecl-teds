#ifndef TEDS_UTIL
#define TEDS_UTIL

#include "zend_types.h"
#define TEDS_NODE_RED 0
#define TEDS_NODE_BLACK 1

/*
 * Copied from php's value of HT_MAX_SIZE.
 *
 * The maximum collection size of generic zvals is limited to the max array size because:
 * 1. gc is limited to uint32_t
 * 2. That's 32GB on 64-bit systems for 16-byte zvals
 * 3. get_properties requires that collections can cast to arrays.
 */
#if HT_MAX_SIZE > 0x80000000
#error Expected HT_MAX_SIZE to fit in a uint32_t
#endif

#define TEDS_MAX_ZVAL_COLLECTION_SIZE HT_MAX_SIZE
#define TEDS_MAX_ZVAL_PAIR_COUNT (TEDS_MAX_ZVAL_COLLECTION_SIZE / 2)

#define empty_entry_list ((const void*) (((uint8_t*)NULL) + 16))
static zend_always_inline bool teds_is_nan(const double v) {
	return v != v;
}

static bool teds_is_identical_inline(zval *op1, zval *op2);

/* return int to be compatible with compare_func_t. Copied from Zend/zend_operators.h to support inlining.  */
static int teds_hash_zval_identical_function(zval *z1, zval *z2) /* {{{ */
{
	/* is_identical_function() returns 1 in case of identity and 0 in case
	 * of a difference;
	 * whereas this comparison function is expected to return 0 on identity,
	 * and non zero otherwise.
	 */
	ZVAL_DEREF(z1);
	ZVAL_DEREF(z2);
	return !teds_is_identical_inline(z1, z2);
}
/* }}} */

static zend_always_inline bool teds_is_identical_inline(zval *op1, zval *op2) /* {{{ Copied from zend_is_identical to support inlining for repeated calls in Vector. */
{
	if (Z_TYPE_P(op1) != Z_TYPE_P(op2)) {
		return 0;
	}
	switch (Z_TYPE_P(op1)) {
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			return 1;
		case IS_LONG:
			return (Z_LVAL_P(op1) == Z_LVAL_P(op2));
		case IS_RESOURCE:
			return (Z_RES_P(op1) == Z_RES_P(op2));
		case IS_DOUBLE:
			return (Z_DVAL_P(op1) == Z_DVAL_P(op2));
		case IS_STRING:
			return zend_string_equals(Z_STR_P(op1), Z_STR_P(op2));
		case IS_ARRAY:
			return (Z_ARRVAL_P(op1) == Z_ARRVAL_P(op2) ||
				zend_hash_compare(Z_ARRVAL_P(op1), Z_ARRVAL_P(op2), (compare_func_t) teds_hash_zval_identical_function, 1) == 0);
		case IS_OBJECT:
			return (Z_OBJ_P(op1) == Z_OBJ_P(op2));
		default:
			return 0;
	}
}
/* }}} */

static zend_always_inline HashTable *teds_new_array_check_overflow(size_t n) {
#if SIZEOF_SIZE_T > 4
	if (UNEXPECTED(n >= HT_MAX_SIZE)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%lu * %zu + %zu)", n, sizeof(Bucket), sizeof(Bucket));
	}
#endif
	return zend_new_array(n);
}

/* Assume that on 64-bit systems, there won't be enough memory (9.22+18 bytes) to overflow */
static zend_always_inline bool teds_offset_within_size_t(const zend_long offset, const size_t len) {
	return ((zend_ulong) offset) < len && EXPECTED(SIZEOF_SIZE_T > 4 || offset >= 0);
}

static inline zval *teds_zval_copy_range(const zval *original, size_t n) {
	const size_t bytes = n * sizeof(zval);
	zval *copy = emalloc(bytes);
	memcpy(copy, original, bytes);
	return copy;
}

int teds_hash_zval_identical_or_both_nan_function(zval *op1, zval *op2);

/** For use with strict_hash. This is a modified version of teds_is_identical_inline. */
static inline int teds_is_identical_or_both_nan(const zval *op1, const zval *op2) {
	if (Z_TYPE_P(op1) != Z_TYPE_P(op2)) {
		return 0;
	}
	switch (Z_TYPE_P(op1)) {
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			return 1;
		case IS_LONG:
			return (Z_LVAL_P(op1) == Z_LVAL_P(op2));
		case IS_RESOURCE:
			return (Z_RES_P(op1) == Z_RES_P(op2));
		case IS_DOUBLE:
			if (UNEXPECTED(teds_is_nan(Z_DVAL_P(op1)))) {
				return teds_is_nan(Z_DVAL_P(op2));
			}
			return (Z_DVAL_P(op1) == Z_DVAL_P(op2));
		case IS_STRING:
			return zend_string_equals(Z_STR_P(op1), Z_STR_P(op2));
		case IS_ARRAY:
			return (Z_ARRVAL_P(op1) == Z_ARRVAL_P(op2) ||
				zend_hash_compare(Z_ARRVAL_P(op1), Z_ARRVAL_P(op2), (compare_func_t) teds_hash_zval_identical_or_both_nan_function, 1) == 0);
		case IS_OBJECT:
			return (Z_OBJ_P(op1) == Z_OBJ_P(op2));
		default:
			ZEND_UNREACHABLE();
			return 0;
	}
}

static inline bool teds_zval_range_contains(const zval *target, const zval *original, uint32_t n) {
	for (const zval *const end = original + n; original != end; original++) {
		if (teds_is_identical_inline((zval *)original, (zval *)target)) {
			return true;
		}
	}
	return false;
}

static inline void teds_zval_range_zval_indexof(zval *return_value, const zval *target, const zval *const original, uint32_t n) {
	for (const zval *it = original, *const end = original + n; it != end; it++) {
		if (teds_is_identical_inline((zval *)it, (zval *)target)) {
			RETURN_LONG(it - original);
		}
	}
	RETURN_NULL();
}

#define TEDS_RETURN_ZVAL_LIST_INDEXOF(target, values, n) do { teds_zval_range_zval_indexof(return_value, (target), (values), (n)); return; } while (0)

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

static zend_always_inline uint32_t teds_next_pow2_capacity_uint32(uint32_t nSize, uint32_t min) {
	if (nSize < min) {
		return min;
	}
	/* Note that for values such as 63 or 31 of the form ((2^n) - 1),
	 * subtracting and xor are the same things for numbers in the range of 0 to the max. */
#ifdef ZEND_WIN32
	unsigned long index;
	if (BitScanReverse(&index, nSize - 1)) {
		return 0x2u << ((31 - index) ^ 0x1f);
	}
	/* nSize is ensured to be in the valid range, fall back to it
	 * rather than using an undefined bit scan result. */
	return nSize;
#elif (defined(__GNUC__) || __has_builtin(__builtin_clz))  && defined(PHP_HAVE_BUILTIN_CLZ)
	return 0x2u << (__builtin_clz(nSize - 1) ^ 0x1f);
#endif
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
	return nSize + 1;
}

static zend_always_inline size_t teds_next_pow2_capacity(size_t nSize, size_t min) {
#if SIZEOF_SIZE_T <= 4
	return teds_next_pow2_capacity_uint32(nSize, min);
#else
	if (nSize < min) {
		return min;
	}
	/* Note that for values such as 63 or 31 of the form ((2^n) - 1),
	 * subtracting and xor are the same things for numbers in the range of 0 to the max. */
#ifdef ZEND_WIN32
	unsigned long index;
	if (BitScanReverse64(&index, nSize - 1)) {
		return 0x2u << ((63 - index) ^ 0x3f);
	}
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
	nSize |= (nSize >> 32);
	return nSize + 1;
#endif
#endif
}

#define TEDS_DEQUE_MIN_CAPACITY 4
/* Based on zend_hash_check_size which supports 32-bit integers. Finds the next largest power of 2. */
static zend_always_inline size_t teds_deque_next_pow2_capacity(size_t nSize) {
	return teds_next_pow2_capacity(nSize, TEDS_DEQUE_MIN_CAPACITY);
}

#define TEDS_STRICTHASHSET_MIN_CAPACITY 8
#define TEDS_STRICTHASHMAP_MIN_CAPACITY TEDS_STRICTHASHSET_MIN_CAPACITY
static zend_always_inline size_t teds_stricthashset_next_pow2_capacity(size_t nSize) {
	return teds_next_pow2_capacity(nSize, TEDS_STRICTHASHSET_MIN_CAPACITY);
}
#define teds_stricthashmap_next_pow2_capacity teds_stricthashset_next_pow2_capacity

/* TODO remove when rewriting as binary tree */
#define teds_stricttreemap_next_pow2_capacity teds_deque_next_pow2_capacity
#define teds_stricttreeset_next_pow2_capacity teds_deque_next_pow2_capacity
#define teds_strictheap_next_pow2_capacity teds_deque_next_pow2_capacity
/* TODO: Allow arbitrary capacities for strictsortedvectorset,
 * this restriction is copied over from StrictHashMap. */
#define teds_strictsortedvectorset_next_pow2_capacity teds_deque_next_pow2_capacity
#define teds_strictsortedvectormap_next_pow2_capacity teds_deque_next_pow2_capacity

static zend_always_inline int teds_has_dimension_helper(zval *value, int check_empty)
{
	if (check_empty) {
		return zend_is_true(value);
	}
	return Z_TYPE_P(value) != IS_NULL;
}

#endif
