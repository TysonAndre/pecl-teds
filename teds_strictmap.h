/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STRICTMAP_H
#define TEDS_STRICTMAP_H

#include "Zend/zend_types.h"
#include "teds_util.h"

PHP_MINIT_FUNCTION(teds_strictmap);

#define TEDS_STRICTMAP_HASH_TO_BUCKET(ht, idx) \
	TEDS_STRICTMAP_HASH_TO_BUCKET_EX((ht)->arData, idx)

/* Same as HT_SIZE_TO_MAX for now, code duplication is deliberate in case php-src's implementation changes. */
#define TEDS_STRICTMAP_IT_NEXT(entry) Z_NEXT((entry)->key)

#define TEDS_STRICTMAP_SIZE_TO_MASK(nTableSize) \
	((uint32_t)(-((nTableSize) + (nTableSize))));

#define TEDS_STRICTMAP_HASH_EX(data, idx) HT_HASH_EX((data), (idx))

#define TEDS_STRICTMAP_HASH_SIZE(nTableMask) HT_HASH_SIZE((nTableMask))

#define TEDS_STRICTMAP_DATA_SIZE(nTableSize) \
	((nTableSize) * sizeof(teds_strictmap_entry))

#define TEDS_STRICTMAP_SIZE_EX(entry) (nTableSize, nTableMask) \
	(TEDS_STRICTMAP_DATA_SIZE((nTableSize)) + TEDS_STRICTMAP_HASH_SIZE((nTableMask)))

#define TEDS_STRICTMAP_MEMORY_PER_ENTRY (sizeof(teds_strictmap_entry) + (sizeof(uint32_t) * 2))

#define TEDS_STRICTMAP_MIN_MASK ((uint32_t) -2)
#define TEDS_STRICTMAP_INVALID_INDEX ((uint32_t) -1)

/* Based on ZEND_HASH_MAP_FOREACH_FROM */
#define TEDS_STRICTMAP_FOREACH(_strictmap) do { \
	const teds_strictmap_entries *const __strictmap = (_strictmap); \
	teds_strictmap_entry *_p = __strictmap->arData; \
	teds_strictmap_entry *const _end = _p + __strictmap->nNumUsed; \
	for (; _p != _end; _p++) { \
		zval *_key = &_p->key; \
		if (Z_TYPE_P(_key) == IS_UNDEF) { continue; }

#define TEDS_STRICTMAP_FOREACH_KEY_VAL(strictmap, k, v) TEDS_STRICTMAP_FOREACH(strictmap) \
		k = _key; \
		v = &_p->value;

#define TEDS_STRICTMAP_FOREACH_VAL(strictmap, v) TEDS_STRICTMAP_FOREACH(strictmap) \
		v = &_p->value;

#define TEDS_STRICTMAP_FOREACH_KEY(strictmap, k) TEDS_STRICTMAP_FOREACH(strictmap) \
		k = _key;

#define TEDS_STRICTMAP_FOREACH_BUCKET(strictmap, b) TEDS_STRICTMAP_FOREACH(strictmap) \
		b = _p;

#define TEDS_STRICTMAP_FOREACH_END() \
	} \
} while (0)

typedef struct _teds_strictmap_entry {
	zval key;   /* TODO: Stores Z_NEXT - similar to https://github.com/php/php-src/pull/6588 */
	zval value;
} teds_strictmap_entry;

/* See Zend/zend_types.h for the zend_array layout this is based on. */
typedef struct _teds_strictmap_entries {
	uint32_t nNumUsed; /* Number of buckets used, including gaps left by remove. */
	uint32_t nNumOfElements; /* Number of elements in this set */
	uint32_t nTableSize; /* Power of 2 size */
	uint32_t nTableMask; /* -nTableSize or TEDS_STRICTMAP_MIN_MASK, e.g. 0xfffffff0 for an array of size 8 with 16 buckets. */
	union {
		teds_strictmap_entry *arData;
		uint32_t *arHash;
	};
} teds_strictmap_entries;

typedef struct _teds_strictmap {
	teds_strictmap_entries	array;
	zend_object				std;
} teds_strictmap;

extern const uint32_t teds_strictmap_empty_bucket_list[2];
#define teds_strictmap_empty_entry_list ((teds_strictmap_entry *) &teds_strictmap_empty_bucket_list[2])

void teds_strictmap_entries_init_from_array(teds_strictmap_entries *array, zend_array *values);

void teds_strictmap_entries_init_from_traversable(teds_strictmap_entries *array, zend_object *obj);

teds_strictmap_entry *teds_strictmap_entries_find_key(const teds_strictmap_entries *array, zval *key, zend_ulong h);

void teds_strictmap_entries_dtor(teds_strictmap_entries *array);

void teds_strictmap_entries_dtor_range(teds_strictmap_entry *start, uint32_t from, uint32_t to);

static zend_always_inline void teds_strictmap_entries_array_free(teds_strictmap_entry *entries, uint32_t capacity) {
	ZEND_ASSERT(teds_is_pow2(capacity));
	ZEND_ASSERT(entries != teds_strictmap_empty_entry_list);
	uint32_t *data = ((uint32_t *)entries) - 2 * capacity;
	efree(data);
}

static zend_always_inline void teds_strictmap_entries_release(teds_strictmap_entries *array) {
	ZEND_ASSERT(array->arData != NULL);
	teds_strictmap_entries_array_free(array->arData, array->nTableSize);
}

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_strictmap_entries_empty_capacity(teds_strictmap_entries *array)
{
	ZEND_ASSERT(array->nNumOfElements <= array->nTableSize);
	if (array->nTableSize > 0) {
		ZEND_ASSERT(array->arData != teds_strictmap_empty_entry_list);
		return false;
	}
	ZEND_ASSERT(array->arData == teds_strictmap_empty_entry_list || array->arData == NULL);
	return true;
}

#endif	/* TEDS_STRICTMAP_H */
