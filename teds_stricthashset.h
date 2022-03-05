/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STRICTHASHSET_H
#define TEDS_STRICTHASHSET_H

#include "Zend/zend_types.h"
#include "teds_util.h"

PHP_MINIT_FUNCTION(teds_stricthashset);

#define TEDS_STRICTHASHSET_HASH_TO_BUCKET(ht, idx) \
	TEDS_STRICTHASHSET_HASH_TO_BUCKET_EX((ht)->arData, idx)

/* Same as HT_SIZE_TO_MAX for now, code duplication is deliberate in case php-src's implementation changes. */
#define TEDS_STRICTHASHSET_IT_NEXT(entry) Z_NEXT((entry)->key)

#define TEDS_STRICTHASHSET_SIZE_TO_MASK(nTableSize) \
	((uint32_t)(-((nTableSize) + (nTableSize))));

#define TEDS_STRICTHASHSET_HASH_EX(data, idx) HT_HASH_EX((data), (idx))

#define TEDS_STRICTHASHSET_HASH_SIZE(nTableMask) HT_HASH_SIZE((nTableMask))

#define TEDS_STRICTHASHSET_DATA_SIZE(nTableSize) \
	((nTableSize) * sizeof(teds_stricthashset_entry))

#define TEDS_STRICTHASHSET_SIZE_EX(entry) (nTableSize, nTableMask) \
	(TEDS_STRICTHASHSET_DATA_SIZE((nTableSize)) + TEDS_STRICTHASHSET_HASH_SIZE((nTableMask)))

#define TEDS_STRICTHASHSET_MEMORY_PER_ENTRY (sizeof(teds_stricthashset_entry) + (sizeof(uint32_t) * 2))

#define TEDS_STRICTHASHSET_MIN_MASK ((uint32_t) -2)
#define TEDS_STRICTHASHSET_INVALID_INDEX ((uint32_t) -1)

/* Based on ZEND_HASH_MAP_FOREACH_FROM */
#define TEDS_STRICTHASHSET_FOREACH_CHECK_MODIFY_VAL(_stricthashset, v) do { \
	const teds_stricthashset_entries *const __stricthashset = (_stricthashset); \
	uint32_t _i = 0; \
	for (; _i < __stricthashset->nNumUsed; _i++) { \
		teds_stricthashset_entry *_p = &(__stricthashset->arData[_i]); \
		v = &_p->key; \
		if (Z_TYPE_P(v) == IS_UNDEF) { continue; }

#define TEDS_STRICTHASHSET_FOREACH(_stricthashset) do { \
	const teds_stricthashset_entries *const __stricthashset = (_stricthashset); \
	teds_stricthashset_entry *_p = __stricthashset->arData; \
	teds_stricthashset_entry *const _end = _p + __stricthashset->nNumUsed; \
	for (; _p != _end; _p++) { \
		zval *_z = &_p->key; \
		if (Z_TYPE_P(_z) == IS_UNDEF) { continue; }

#define TEDS_STRICTHASHSET_FOREACH_VAL_ASSERT_NO_GAPS(_stricthashset, v) do { \
	const teds_stricthashset_entries *const __stricthashset = (_stricthashset); \
	teds_stricthashset_entry *_p = __stricthashset->arData; \
	teds_stricthashset_entry *const _end = _p + __stricthashset->nNumUsed; \
	for (; _p != _end; _p++) { \
		v = &_p->key; \
		ZEND_ASSERT(Z_TYPE_P(v) != IS_UNDEF);

#define TEDS_STRICTHASHSET_FOREACH_VAL(stricthashset, v) TEDS_STRICTHASHSET_FOREACH(stricthashset) \
		v = _z;

#define TEDS_STRICTHASHSET_FOREACH_BUCKET(stricthashset, b) TEDS_STRICTHASHSET_FOREACH(stricthashset) \
		b = _p;

#define TEDS_STRICTHASHSET_FOREACH_END() \
	} \
} while (0)

typedef struct _teds_stricthashset_entry {
	zval key;   /* TODO: Stores Z_NEXT - similar to https://github.com/php/php-src/pull/6588 */
	uint32_t h;
} teds_stricthashset_entry;

/* See Zend/zend_types.h for the zend_array layout this is based on. */
typedef struct _teds_stricthashset_entries {
	union {
		teds_stricthashset_entry *arData;
		uint32_t *arHash;
	};
	uint32_t nNumOfElements; /* Number of elements in this set, a.k.a. count() */
	uint32_t nTableSize; /* Power of 2 size, aka capacity() */
	uint32_t nNumUsed; /* Number of buckets used, including gaps left by remove. */
	uint32_t nTableMask; /* -nTableSize or TEDS_STRICTHASHSET_MIN_MASK, e.g. 0xfffffff0 for an array of size 8 with 16 buckets. */
	teds_intrusive_dllist active_iterators;
	/* TODO could track uint32_t firstUsed for cases where removal of first is common, e.g. LRU caches */
	bool should_rebuild_properties;
} teds_stricthashset_entries;

typedef struct _teds_stricthashset {
	teds_stricthashset_entries	array;
	zend_object				    std;
} teds_stricthashset;

extern const uint32_t teds_stricthashset_empty_bucket_list[2];

/* These are also used for Teds\unique_values() */
void teds_stricthashset_entries_init_from_array(teds_stricthashset_entries *array, zend_array *values);

/* These are also used for Teds\unique_values() */
void teds_stricthashset_entries_init_from_traversable(teds_stricthashset_entries *array, zend_object *obj);

teds_stricthashset_entry *teds_stricthashset_entries_find_key(const teds_stricthashset_entries *array, zval *key, zend_ulong h);

void teds_stricthashset_entries_dtor(teds_stricthashset_entries *array);

void teds_stricthashset_entries_dtor_range(teds_stricthashset_entry *start, size_t from, size_t to);

static zend_always_inline void teds_stricthashset_entries_array_free(teds_stricthashset_entry *entries, size_t capacity) {
	ZEND_ASSERT(teds_is_pow2(capacity));
	ZEND_ASSERT(entries != empty_entry_list);
	uint32_t *data = ((uint32_t *)entries) - 2 * capacity;
	efree(data);
}

static zend_always_inline void teds_stricthashset_entries_release(teds_stricthashset_entries *array) {
	ZEND_ASSERT(array->arData != NULL);
	teds_stricthashset_entries_array_free(array->arData, array->nTableSize);
}

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_stricthashset_entries_empty_capacity(teds_stricthashset_entries *array)
{
	ZEND_ASSERT(array->nNumOfElements <= array->nTableSize);
	if (array->nTableSize > 0) {
		ZEND_ASSERT(array->arData != empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->arData == empty_entry_list || array->arData == NULL);
	return true;
}

#endif	/* TEDS_STRICTHASHSET_H */
