/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STRICTHASHMAP_H
#define TEDS_STRICTHASHMAP_H

#include "Zend/zend_types.h"
#include "teds_util.h"

PHP_MINIT_FUNCTION(teds_stricthashmap);

#define TEDS_STRICTHASHMAP_HASH_TO_BUCKET(ht, idx) \
	TEDS_STRICTHASHMAP_HASH_TO_BUCKET_EX((ht)->arData, idx)

/* Same as HT_SIZE_TO_MAX for now, code duplication is deliberate in case php-src's implementation changes. */
#define TEDS_STRICTHASHMAP_IT_NEXT(entry) Z_NEXT((entry)->key)

#define TEDS_STRICTHASHMAP_SIZE_TO_MASK(nTableSize) \
	((uint32_t)(-((nTableSize) + (nTableSize))));

#define TEDS_STRICTHASHMAP_HASH_EX(data, idx) HT_HASH_EX((data), (idx))

#define TEDS_STRICTHASHMAP_HASH_SIZE(nTableMask) HT_HASH_SIZE((nTableMask))

#define TEDS_STRICTHASHMAP_DATA_SIZE(nTableSize) \
	((nTableSize) * sizeof(teds_stricthashmap_entry))

#define TEDS_STRICTHASHMAP_SIZE_EX(entry) (nTableSize, nTableMask) \
	(TEDS_STRICTHASHMAP_DATA_SIZE((nTableSize)) + TEDS_STRICTHASHMAP_HASH_SIZE((nTableMask)))

#define TEDS_STRICTHASHMAP_MEMORY_PER_ENTRY (sizeof(teds_stricthashmap_entry) + (sizeof(uint32_t) * 2))

#define TEDS_STRICTHASHMAP_MIN_MASK ((uint32_t) -2)
#define TEDS_STRICTHASHMAP_INVALID_INDEX ((uint32_t) -1)

/* Based on ZEND_HASH_MAP_FOREACH_FROM */
#define TEDS_STRICTHASHMAP_FOREACH(_stricthashmap) do { \
	const teds_stricthashmap_entries *const __stricthashmap = (_stricthashmap); \
	teds_stricthashmap_entry *_p = __stricthashmap->arData; \
	teds_stricthashmap_entry *const _end = _p + __stricthashmap->nNumUsed; \
	for (; _p != _end; _p++) { \
		zval *_z = &_p->key; \
		if (Z_TYPE_P(_z) == IS_UNDEF) { continue; }

#define TEDS_STRICTHASHMAP_FOREACH_VAL_ASSERT_NO_GAPS(_stricthashmap, v) do { \
	const teds_stricthashmap_entries *const __stricthashmap = (_stricthashmap); \
	teds_stricthashmap_entry *_p = __stricthashmap->arData; \
	teds_stricthashmap_entry *const _end = _p + __stricthashmap->nNumUsed; \
	for (; _p != _end; _p++) { \
		v = &_p->key; \
		ZEND_ASSERT(Z_TYPE_P(v) != IS_UNDEF);

#define TEDS_STRICTHASHMAP_FOREACH_VAL(stricthashmap, v) TEDS_STRICTHASHMAP_FOREACH(stricthashmap) \
		v = _z;

#define TEDS_STRICTHASHMAP_FOREACH_BUCKET(stricthashmap, b) TEDS_STRICTHASHMAP_FOREACH(stricthashmap) \
		b = _p;

#define TEDS_STRICTHASHMAP_FOREACH_END() \
	} \
} while (0)

typedef struct _teds_stricthashmap_entry {
	zval key;   /* TODO: Stores Z_NEXT - similar to https://github.com/php/php-src/pull/6588 */
	uint32_t h;
} teds_stricthashmap_entry;

/* See Zend/zend_types.h for the zend_array layout this is based on. */
typedef struct _teds_stricthashmap_entries {
	uint32_t nNumUsed; /* Number of buckets used, including gaps left by remove. */
	uint32_t nNumOfElements; /* Number of elements in this set */
	uint32_t nTableSize; /* Power of 2 size */
	uint32_t nTableMask; /* -nTableSize or TEDS_STRICTHASHMAP_MIN_MASK, e.g. 0xfffffff0 for an array of size 8 with 16 buckets. */
	union {
		teds_stricthashmap_entry *arData;
		uint32_t *arHash;
	};
} teds_stricthashmap_entries;

typedef struct _teds_stricthashmap {
	teds_stricthashmap_entries	array;
	zend_object				std;
} teds_stricthashmap;

extern const uint32_t teds_stricthashmap_empty_bucket_list[2];
#define teds_stricthashmap_empty_entry_list ((teds_stricthashmap_entry *) &teds_stricthashmap_empty_bucket_list[2])

void teds_stricthashmap_entries_init_from_array(teds_stricthashmap_entries *array, zend_array *values);

void teds_stricthashmap_entries_init_from_traversable(teds_stricthashmap_entries *array, zend_object *obj);

teds_stricthashmap_entry *teds_stricthashmap_entries_find_key(const teds_stricthashmap_entries *array, zval *key, zend_ulong h);

void teds_stricthashmap_entries_dtor(teds_stricthashmap_entries *array);

void teds_stricthashmap_entries_dtor_range(teds_stricthashmap_entry *start, size_t from, size_t to);

static zend_always_inline void teds_stricthashmap_entries_array_free(teds_stricthashmap_entry *entries, size_t capacity) {
	ZEND_ASSERT(teds_is_pow2(capacity));
	ZEND_ASSERT(entries != teds_stricthashmap_empty_entry_list);
	uint32_t *data = ((uint32_t *)entries) - 2 * capacity;
	efree(data);
}

static zend_always_inline void teds_stricthashmap_entries_release(teds_stricthashmap_entries *array) {
	ZEND_ASSERT(array->arData != NULL);
	teds_stricthashmap_entries_array_free(array->arData, array->nTableSize);
}

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_stricthashmap_entries_empty_capacity(teds_stricthashmap_entries *array)
{
	ZEND_ASSERT(array->nNumOfElements <= array->nTableSize);
	if (array->nTableSize > 0) {
		ZEND_ASSERT(array->arData != teds_stricthashmap_empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->arData == teds_stricthashmap_empty_entry_list || array->arData == NULL);
	return true;
}

#endif	/* TEDS_STRICTHASHMAP_H */
