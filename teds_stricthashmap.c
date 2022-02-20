/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on teds_immutableiterable.c and Zend/zend_hash.c
 * TODO: associate StrictHashMap with linked list of iterators
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_stricthashmap_arginfo.h"
#include "teds_stricthashmap.h"
#include "teds_util.h"
#include "teds_interfaces.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StrictHashMap;
zend_class_entry *teds_ce_StrictHashMap;

#undef HT_HASH_TO_BUCKET_EX
#undef HT_IDX_TO_HASH
#undef HT_HASH_TO_IDX

// #define TEDS_HASH(zval) ((zval).u2.extra)
#define TEDS_STRICTHASHMAP_IT_HASH(entry) ((entry)->value.u2.extra)

/* Need at least 2 for iteration base cases */
const uint32_t teds_stricthashmap_empty_bucket_list[2] = { TEDS_STRICTHASHMAP_INVALID_INDEX, TEDS_STRICTHASHMAP_INVALID_INDEX };

static void teds_stricthashmap_entries_grow(teds_stricthashmap_entries *array);
static void teds_stricthashmap_entries_set_capacity(teds_stricthashmap_entries *array, uint32_t new_capacity);

static ZEND_NORETURN ZEND_COLD void teds_error_noreturn_max_stricthashmap_capacity(void) {
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\StrictHashMap capacity");
}

static zend_always_inline teds_stricthashmap_entry *teds_stricthashmap_entries_find_bucket(const teds_stricthashmap_entries *ht, zval *key, const uint32_t h)
{
	teds_stricthashmap_entry *p;

	teds_stricthashmap_entry *const arData = ht->arData;
	const uint32_t nIndex = h | ht->nTableMask;
	uint32_t idx = HT_HASH_EX(arData, nIndex);
	while (idx != TEDS_STRICTHASHMAP_INVALID_INDEX) {
		ZEND_ASSERT(idx < ht->nTableSize);
		p = arData + idx;
		if (TEDS_STRICTHASHMAP_IT_HASH(p) == h && teds_is_identical_or_both_nan(&p->key, key)) {
			return p;
		}
		idx = TEDS_STRICTHASHMAP_IT_NEXT(p);
	}
	return NULL;
}

static zend_always_inline teds_stricthashmap_entry *teds_stricthashmap_entries_find_bucket_computing_hash(const teds_stricthashmap_entries *ht, zval *key)
{
	return teds_stricthashmap_entries_find_bucket(ht, key, teds_strict_hash_uint32_t(key));
}
/* Returns true if a new entry was added to the map, false if updated. Based on _zend_hash_add_or_update_i. */
static zend_always_inline bool teds_stricthashmap_entries_insert(teds_stricthashmap_entries *array, zval *key, zval *value, bool add_new)
{
	teds_stricthashmap_entry *p;
	ZEND_ASSERT(Z_TYPE_P(key) != IS_UNDEF);
	ZEND_ASSERT(Z_TYPE_P(value) != IS_UNDEF);

	const zend_ulong h = teds_strict_hash_uint32_t(key);

	if (UNEXPECTED(array->nTableSize == 0)) {
		teds_stricthashmap_entries_set_capacity(array, TEDS_STRICTHASHMAP_MIN_CAPACITY);
		goto add_to_hash;
	} else if (!add_new || ZEND_DEBUG) {
		p = teds_stricthashmap_entries_find_bucket(array, key, h);

		if (p) {
			ZEND_ASSERT(!add_new);
			zval old_value;
			ZVAL_COPY_VALUE(&old_value, &p->value);
			ZVAL_COPY(&p->value, value);
			zval_ptr_dtor(&old_value);
			return false;
		}
	}

	/* If the Hash table is full, resize it */
	if (UNEXPECTED(array->nNumUsed >= array->nTableSize)) {
		teds_stricthashmap_entries_grow(array);
	}

add_to_hash:
	;
	const uint32_t idx = array->nNumUsed;
	teds_stricthashmap_entry *const arData = array->arData;
	p = arData + idx;
	const uint32_t nIndex = h | array->nTableMask;
	array->nNumUsed++;
	array->nNumOfElements++;
	TEDS_STRICTHASHMAP_IT_HASH(p) = h;
	TEDS_STRICTHASHMAP_IT_NEXT(p) = HT_HASH_EX(arData, nIndex);
	HT_HASH_EX(arData, nIndex) = idx;

	ZVAL_COPY(&p->key, key);
	ZVAL_COPY(&p->value, value);

	return true;
}

static void teds_stricthashmap_entries_clear(teds_stricthashmap_entries *array);

/* Used by InternalIterator returned by StrictHashMap->getIterator() */
typedef struct _teds_stricthashmap_it {
	zend_object_iterator intern;
	uint32_t             current;
} teds_stricthashmap_it;

static zend_always_inline teds_stricthashmap *teds_stricthashmap_from_object(zend_object *obj)
{
	return (teds_stricthashmap*)((char*)(obj) - XtOffsetOf(teds_stricthashmap, std));
}
#define teds_stricthashmap_entries_from_object(obj) (&teds_stricthashmap_from_object((obj))->array)

#define Z_STRICTHASHMAP_P(zv)  teds_stricthashmap_from_object(Z_OBJ_P((zv)))
#define Z_STRICTHASHMAP_ENTRIES_P(zv)  (&(Z_STRICTHASHMAP_P((zv)))->array)

static bool teds_stricthashmap_entries_uninitialized(teds_stricthashmap_entries *array)
{
	ZEND_ASSERT(array->nNumOfElements <= array->nTableSize);
	ZEND_ASSERT(array->nNumOfElements <= array->nNumUsed);
	if (array->arData == NULL) {
		ZEND_ASSERT(array->nTableSize == 0);
		return true;
	}
	ZEND_ASSERT(array->nTableMask > 0);
	ZEND_ASSERT(array->nTableMask <= TEDS_STRICTHASHMAP_MIN_MASK);
#if ZEND_DEBUG
	if (array->arData == teds_stricthashmap_empty_entry_list) {
		ZEND_ASSERT(array->nTableSize == 0 && array->nTableMask == HT_MIN_MASK);
	} else {
		ZEND_ASSERT(array->nTableSize > 0);
		ZEND_ASSERT(array->nTableMask < HT_MIN_MASK);
	}
#endif
	return false;
}

static zend_always_inline void teds_stricthashmap_entries_set_empty_entry_list(teds_stricthashmap_entries *array)
{
	array->nNumOfElements = 0;
	array->nNumUsed = 0;
	array->nTableSize = 0;
	array->arData = teds_stricthashmap_empty_entry_list;
	array->nTableMask = TEDS_STRICTHASHMAP_MIN_MASK;
}

static zend_always_inline size_t teds_stricthashmap_offset_bytes_for_capacity(uint32_t capacity) {
	return (capacity * sizeof(uint32_t) * 2);
}

static teds_stricthashmap_entry *teds_stricthashmap_alloc_entries(uint32_t capacity) {
	uint8_t *ptr = safe_emalloc(capacity, TEDS_STRICTHASHMAP_MEMORY_PER_ENTRY, 0);
	const size_t buckets_byte_count = teds_stricthashmap_offset_bytes_for_capacity(capacity);
	memset(ptr, TEDS_STRICTHASHMAP_INVALID_INDEX, buckets_byte_count);
	return (void *)(ptr + buckets_byte_count);
}

static zend_always_inline void teds_stricthashmap_free_entries(teds_stricthashmap_entry *old_entries, uint32_t old_capacity) {
	void * old_ptr = ((uint8_t *) old_entries) - teds_stricthashmap_offset_bytes_for_capacity(old_capacity);
	efree(old_ptr);
}

void teds_stricthashmap_entries_init_from_array(teds_stricthashmap_entries *array, zend_array *values)
{
	uint32_t size = zend_hash_num_elements(values);
	if (size > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *val;
		/* There won't be duplicates. */
		uint32_t capacity = teds_stricthashmap_next_pow2_capacity(size);
		array->nNumOfElements = 0; /* reset size in case emalloc() fails */
		array->nNumUsed = 0;
		array->nTableSize = 0;
		teds_stricthashmap_entries_set_capacity(array, capacity);

		ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
			zval key;
			if (skey) {
				ZVAL_STR(&key, skey);
			} else {
				ZVAL_LONG(&key, nkey);
			}
			ZVAL_DEREF(val);
			/* key is unique in an array */
			bool result = teds_stricthashmap_entries_insert(array, &key, val, true);
			ZEND_ASSERT(result);
		} ZEND_HASH_FOREACH_END();
	} else {
		teds_stricthashmap_entries_set_empty_entry_list(array);
	}
}

void teds_stricthashmap_entries_init_from_traversable(teds_stricthashmap_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_stricthashmap_entries_set_empty_entry_list(array);
	zval tmp_obj;
	ZVAL_OBJ(&tmp_obj, obj);
	iter = ce->get_iterator(ce, &tmp_obj, 0);

	if (UNEXPECTED(EG(exception))) {
		return;
	}

	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (funcs->rewind) {
		funcs->rewind(iter);
		if (UNEXPECTED(EG(exception))) {
			goto cleanup_iter;
		}
	}

	while (funcs->valid(iter) == SUCCESS) {
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception)) || value == NULL) {
			break;
		}
		zval key;
		if (funcs->get_current_key) {
			funcs->get_current_key(iter, &key);
			if (UNEXPECTED(EG(exception))) {
				break;
			}
		} else {
			ZVAL_NULL(&key);
		}

		ZVAL_DEREF(value);
		/* The key's reference count was already increased by get_current_key. We need to free it after attempting to update the entry */
		const bool created_new_entry = teds_stricthashmap_entries_insert(array, &key, value, false);
		zval_ptr_dtor(&key);
		if (!created_new_entry) {
			if (UNEXPECTED(EG(exception))) {
				break;
			}
		}

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
	}

cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
	if (UNEXPECTED(EG(exception))) {
		teds_stricthashmap_entries_clear(array);
	}
}

/* Based on zend_hash_rehash. */
static void teds_stricthashmap_entries_rehash_inplace(teds_stricthashmap_entries *ht)
{
	//fprintf(stderr, "Rehash inplace %d %d\n", (int)ht->nNumOfElements, (int)ht->nTableSize);
	ZEND_ASSERT(ht->nNumOfElements > 0);

	HT_HASH_RESET(ht);
	uint32_t i = 0;
	teds_stricthashmap_entry *p = ht->arData;
	do {
		if (Z_TYPE(p->key) == IS_UNDEF) {
			uint32_t j = i;
			teds_stricthashmap_entry *q = p;

			while (++i < ht->nNumUsed) {
				p++;
				if (EXPECTED(Z_TYPE_INFO(p->key) != IS_UNDEF)) {
					const uint32_t h = TEDS_STRICTHASHMAP_IT_HASH(p);
					const uint32_t nIndex = h | ht->nTableMask;
					ZVAL_COPY_VALUE(&q->key, &p->key);
					TEDS_STRICTHASHMAP_IT_NEXT(q) = HT_HASH(ht, nIndex);
					ZVAL_COPY_VALUE(&q->value, &p->value);
					TEDS_STRICTHASHMAP_IT_HASH(q) = h;
					HT_HASH(ht, nIndex) = j;
					q++;
					j++;
				}
			}
			ht->nNumUsed = j;
			break;
		}
		const uint32_t nIndex = TEDS_STRICTHASHMAP_IT_HASH(p) | ht->nTableMask;
		TEDS_STRICTHASHMAP_IT_NEXT(p) = HT_HASH(ht, nIndex);
		HT_HASH(ht, nIndex) = i;
		p++;
	} while (++i < ht->nNumUsed);

	ZEND_ASSERT(ht->nNumUsed == ht->nNumOfElements);
}


static void teds_stricthashmap_entries_grow(teds_stricthashmap_entries *array)
{
	if (teds_stricthashmap_entries_empty_capacity(array)) {
		array->arData = teds_stricthashmap_alloc_entries(TEDS_STRICTHASHMAP_MIN_CAPACITY);
		array->nTableSize = TEDS_STRICTHASHMAP_MIN_CAPACITY;
		array->nTableMask = TEDS_STRICTHASHMAP_SIZE_TO_MASK(TEDS_STRICTHASHMAP_MIN_CAPACITY);
		return;
	}

	ZEND_ASSERT(array->nNumUsed >= array->nNumOfElements);
	if (array->nNumUsed > array->nNumOfElements + (array->nNumOfElements >> 5)) { /* additional term is there to amortize the cost of compaction */
		teds_stricthashmap_entries_rehash_inplace(array);
		return;
	}
	ZEND_ASSERT(teds_is_pow2(array->nTableSize));
	if (UNEXPECTED(array->nTableSize >= TEDS_MAX_ZVAL_PAIR_COUNT / 2)) {
		teds_error_noreturn_max_stricthashmap_capacity();
		ZEND_UNREACHABLE();
	}

	const uint32_t new_capacity = array->nTableSize * 2;
	teds_stricthashmap_entry *const new_entries = teds_stricthashmap_alloc_entries(new_capacity);
	teds_stricthashmap_entry *old_entry;
	teds_stricthashmap_entry *it = new_entries;
	ZEND_ASSERT(array->nNumOfElements <= array->nNumUsed);
	uint32_t i = 0;
	const uint32_t new_mask = TEDS_STRICTHASHMAP_SIZE_TO_MASK(new_capacity);
	TEDS_STRICTHASHMAP_FOREACH_BUCKET(array, old_entry) {
		/* TODO: This can be optimized based on zend_hash_rehash's special cases, e.g. HT_IS_WITHOUT_HOLES */
		const uint32_t h = TEDS_STRICTHASHMAP_IT_HASH(old_entry);
		const uint32_t nIndex = h | new_mask;
		ZVAL_COPY_VALUE(&it->key, &old_entry->key);
		//fprintf(stderr, "Copying %x %d to i=%d type=%d val=%s\n", (int)h, (int)-nIndex, i, (int)Z_TYPE(it->key), (Z_TYPE(it->key) != IS_STRING ? "()" : Z_STRVAL(it->key)));
		TEDS_STRICTHASHMAP_IT_NEXT(it) = HT_HASH_EX(new_entries, nIndex);
		ZVAL_COPY_VALUE(&it->value, &old_entry->value);
		TEDS_STRICTHASHMAP_IT_HASH(it) = h;
		HT_HASH_EX(new_entries, nIndex) = i;
		it++;
		i++;
	} TEDS_STRICTHASHMAP_FOREACH_END();
	ZEND_ASSERT(it - new_entries == array->nNumOfElements);
	array->nNumUsed = array->nNumOfElements;
	teds_stricthashmap_free_entries(array->arData, array->nTableSize);
	array->arData = new_entries;

	array->nTableSize = new_capacity;
	array->nTableMask = new_mask;
}

static void teds_stricthashmap_entries_set_capacity(teds_stricthashmap_entries *array, uint32_t new_capacity)
{
	ZEND_ASSERT(array->nTableSize == 0);
	ZEND_ASSERT(new_capacity > array->nTableSize);
	ZEND_ASSERT(teds_is_pow2(new_capacity));

	array->arData = teds_stricthashmap_alloc_entries(new_capacity);
	array->nTableSize = new_capacity;
	array->nTableMask = TEDS_STRICTHASHMAP_SIZE_TO_MASK(new_capacity);
}

static void teds_stricthashmap_entries_copy_ctor(teds_stricthashmap_entries *to, teds_stricthashmap_entries *from)
{
	const uint32_t size = from->nNumOfElements;
	if (!size) {
		teds_stricthashmap_entries_set_empty_entry_list(to);
		return;
	}

	const uint32_t capacity = from->nTableSize;
	teds_stricthashmap_entries_set_capacity(to, capacity);

	/* TODO optimize this */
	zval *key, *val;
	TEDS_STRICTHASHMAP_FOREACH_KEY_VAL(from, key, val) {
		teds_stricthashmap_entries_insert(to, key, val, true);
	} TEDS_STRICTHASHMAP_FOREACH_END();
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
void teds_stricthashmap_entries_dtor(teds_stricthashmap_entries *array)
{
	if (teds_stricthashmap_entries_empty_capacity(array)) {
		return;
	}
	zval *key, *val;
	TEDS_STRICTHASHMAP_FOREACH_KEY_VAL(array, key, val) {
		zval_ptr_dtor(key);
		zval_ptr_dtor(val);
	} TEDS_STRICTHASHMAP_FOREACH_END();
	teds_stricthashmap_entries_release(array);
}

static HashTable* teds_stricthashmap_get_gc(zend_object *obj, zval **table, int *table_count)
{
	teds_stricthashmap *intern = teds_stricthashmap_from_object(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (intern->array.nNumOfElements > 0) {
		zval *key, *val;

		TEDS_STRICTHASHMAP_FOREACH_KEY_VAL(&intern->array, key, val) {
			zend_get_gc_buffer_add_zval(gc_buffer, key);
			zend_get_gc_buffer_add_zval(gc_buffer, val);
		} TEDS_STRICTHASHMAP_FOREACH_END();
	}
	zend_get_gc_buffer_use(gc_buffer, table, table_count);

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_stricthashmap_get_properties(zend_object *obj)
{
	teds_stricthashmap *intern = teds_stricthashmap_from_object(obj);
	const uint32_t len = intern->array.nNumOfElements;
	HashTable *ht = zend_std_get_properties(obj);
	uint32_t old_length = zend_hash_num_elements(ht);
	/* Initialize properties array */
	uint32_t i = 0;
	zval *key, *value;
	ZEND_ASSERT(len <= intern->array.nNumUsed);
	TEDS_STRICTHASHMAP_FOREACH_KEY_VAL(&intern->array, key, value) {
		zval tmp;
		Z_TRY_ADDREF_P(key);
		Z_TRY_ADDREF_P(value);
		ZVAL_ARR(&tmp, zend_new_pair(key, value));
		zend_hash_index_update(ht, i, &tmp);
		i++;
	} TEDS_STRICTHASHMAP_FOREACH_END();

	ZEND_ASSERT(i == len);

	for (uint32_t i = len; i < old_length; i++) {
		zend_hash_index_del(ht, i);
	}

	return ht;
}

static void teds_stricthashmap_free_storage(zend_object *object)
{
	teds_stricthashmap *intern = teds_stricthashmap_from_object(object);
	teds_stricthashmap_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_stricthashmap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_stricthashmap *intern;

	intern = zend_object_alloc(sizeof(teds_stricthashmap), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StrictHashMap);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_StrictHashMap;

	if (orig && clone_orig) {
		teds_stricthashmap *other = teds_stricthashmap_from_object(orig);
		teds_stricthashmap_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.arData = NULL;
	}

	return &intern->std;
}

static zend_object *teds_stricthashmap_new(zend_class_entry *class_type)
{
	return teds_stricthashmap_new_ex(class_type, NULL, 0);
}


static zend_object *teds_stricthashmap_clone(zend_object *old_object)
{
	zend_object *new_object = teds_stricthashmap_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_stricthashmap_count_elements(zend_object *object, zend_long *count)
{
	teds_stricthashmap *intern;

	intern = teds_stricthashmap_from_object(object);
	*count = intern->array.nNumOfElements;
	return SUCCESS;
}

/* Get number of entries in this StrictHashMap */
PHP_METHOD(Teds_StrictHashMap, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(object);
	RETURN_LONG(intern->array.nNumOfElements);
}

/* Get whether this StrictHashMap is empty */
PHP_METHOD(Teds_StrictHashMap, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(object);
	RETURN_BOOL(intern->array.nNumOfElements == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_StrictHashMap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_stricthashmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictHashMap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		teds_stricthashmap_entries_set_empty_entry_list(&intern->array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_stricthashmap_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_stricthashmap_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StrictHashMap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_stricthashmap_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_stricthashmap_it_rewind(zend_object_iterator *iter)
{
	((teds_stricthashmap_it*)iter)->current = 0;
}

static int teds_stricthashmap_it_valid(zend_object_iterator *iter)
{
	teds_stricthashmap_it     *iterator = (teds_stricthashmap_it*)iter;
	teds_stricthashmap *object   = Z_STRICTHASHMAP_P(&iter->data);

	while (1) {
		if (iterator->current >= object->array.nNumUsed) {
			return FAILURE;
		}
		zval *v = &object->array.arData[iterator->current].key;
		if (Z_TYPE_P(v) != IS_UNDEF) {
			ZEND_ASSERT(Z_TYPE(object->array.arData[iterator->current].value) != IS_UNDEF);
			return SUCCESS;
		}
		iterator->current++;
	}
}

static teds_stricthashmap_entry *teds_stricthashmap_it_read_offset_helper(const teds_stricthashmap *intern, teds_stricthashmap_it *iterator)
{
	teds_stricthashmap_entry *const arData = intern->array.arData;
	while (EXPECTED(iterator->current < intern->array.nNumUsed)) {
		const zval *v = &arData[iterator->current].key;
		if (EXPECTED(Z_TYPE_P(v) != IS_UNDEF)) {
			return &arData[iterator->current];
		}
		iterator->current++;
	}
	zend_throw_exception(spl_ce_OutOfBoundsException, "Attempting to access iterator after the end of the Teds\\StrictHashMap", 0);
	return NULL;
}

static zval *teds_stricthashmap_it_get_current_data(zend_object_iterator *iter)
{
	teds_stricthashmap_it     *iterator = (teds_stricthashmap_it*)iter;
	teds_stricthashmap *object   = Z_STRICTHASHMAP_P(&iter->data);

	teds_stricthashmap_entry *data = teds_stricthashmap_it_read_offset_helper(object, iterator);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_stricthashmap_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_stricthashmap_it     *iterator = (teds_stricthashmap_it*)iter;
	teds_stricthashmap *object   = Z_STRICTHASHMAP_P(&iter->data);

	teds_stricthashmap_entry *data = teds_stricthashmap_it_read_offset_helper(object, iterator);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_stricthashmap_it_move_forward(zend_object_iterator *iter)
{
	((teds_stricthashmap_it*)iter)->current++;
	teds_stricthashmap_it_valid(iter);
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_stricthashmap_it_funcs = {
	teds_stricthashmap_it_dtor,
	teds_stricthashmap_it_valid,
	teds_stricthashmap_it_get_current_data,
	teds_stricthashmap_it_get_current_key,
	teds_stricthashmap_it_move_forward,
	teds_stricthashmap_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_stricthashmap_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_stricthashmap_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_stricthashmap_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_stricthashmap_it_funcs;
	(void) ce;

	return &iterator->intern;
}

PHP_METHOD(Teds_StrictHashMap, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	uint32_t raw_size = zend_hash_num_elements(raw_data);
	if (UNEXPECTED(raw_size % 2 != 0)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Odd number of elements", 0);
		RETURN_THROWS();
	}
	teds_stricthashmap *const intern = Z_STRICTHASHMAP_P(ZEND_THIS);
	teds_stricthashmap_entries *const array = &intern->array;
	if (UNEXPECTED(!teds_stricthashmap_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	ZEND_ASSERT(array->arData == NULL);
	if (raw_size == 0) {
		teds_stricthashmap_entries_set_empty_entry_list(array);
		return;
	}

	const uint32_t capacity = teds_stricthashmap_next_pow2_capacity(raw_size);
	teds_stricthashmap_entries_set_capacity(array, capacity);

	zend_string *str;
	zval key;
	bool is_key = true;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_stricthashmap_entries_clear(array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictHashMap::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
		if (!is_key) {
			teds_stricthashmap_entries_insert(array, &key, val, false);
		} else {
			ZVAL_COPY_VALUE(&key, val);
		}
		is_key = !is_key;
	} ZEND_HASH_FOREACH_END();
}

static bool teds_stricthashmap_entries_insert_from_pair(teds_stricthashmap_entries *array, zval *raw_val)
{
	ZVAL_DEREF(raw_val);
	if (UNEXPECTED(Z_TYPE_P(raw_val) != IS_ARRAY)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find pair in array but got non-array", 0);
		return false;
	}
	HashTable *ht = Z_ARRVAL_P(raw_val);
	zval *key = zend_hash_index_find(ht, 0);
	if (UNEXPECTED(!key)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find key at index 0", 0);
		return false;
	}
	zval *value = zend_hash_index_find(ht, 1);
	if (UNEXPECTED(!value)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find value at index 1", 0);
		return false;
	}
	ZVAL_DEREF(key);
	ZVAL_DEREF(value);
	teds_stricthashmap_entries_insert(array, key, value, false);
	return true;
}

static void teds_stricthashmap_entries_init_from_array_pairs(teds_stricthashmap_entries *array, zend_array *raw_data)
{
	uint32_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		teds_stricthashmap_entries_set_empty_entry_list(array);
		return;
	}
	const uint32_t capacity = teds_stricthashmap_next_pow2_capacity(num_entries);
	array->nNumOfElements = 0; /* reset size in case emalloc() fails */
	array->nNumUsed = 0;
	array->nTableSize = 0;
	teds_stricthashmap_entries_set_capacity(array, capacity);
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		if (!teds_stricthashmap_entries_insert_from_pair(array, val)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();
}

static void teds_stricthashmap_entries_init_from_traversable_pairs(teds_stricthashmap_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_stricthashmap_entries_set_empty_entry_list(array);
	zval tmp_obj;
	ZVAL_OBJ(&tmp_obj, obj);
	iter = ce->get_iterator(ce, &tmp_obj, 0);

	if (UNEXPECTED(EG(exception))) {
		return;
	}

	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (funcs->rewind) {
		funcs->rewind(iter);
		if (UNEXPECTED(EG(exception))) {
			return;
		}
	}
	while (funcs->valid(iter) == SUCCESS) {
		if (EG(exception)) {
			break;
		}
		zval *pair = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		if (!teds_stricthashmap_entries_insert_from_pair(array, pair)) {
			break;
		}

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}

	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static zend_object* create_from_pairs(zval *iterable) {
	zend_object *object = teds_stricthashmap_new(teds_ce_StrictHashMap);
	teds_stricthashmap *intern = teds_stricthashmap_from_object(object);
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_stricthashmap_entries_init_from_array_pairs(&intern->array, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_stricthashmap_entries_init_from_traversable_pairs(&intern->array, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_StrictHashMap, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_StrictHashMap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_stricthashmap_new(teds_ce_StrictHashMap);
	teds_stricthashmap *intern = teds_stricthashmap_from_object(object);
	teds_stricthashmap_entries_init_from_array_pairs(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_StrictHashMap, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);

	uint32_t len = intern->array.nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	zval *key, *value;
	TEDS_STRICTHASHMAP_FOREACH_KEY_VAL(&intern->array, key, value) {
		Z_TRY_ADDREF_P(key);
		zend_hash_next_index_insert(flat_entries_array, key);
		Z_TRY_ADDREF_P(value);
		zend_hash_next_index_insert(flat_entries_array, value);
	} TEDS_STRICTHASHMAP_FOREACH_END();

	RETURN_ARR(flat_entries_array);
}

PHP_METHOD(Teds_StrictHashMap, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);
	uint32_t len = intern->array.nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *keys = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(keys);

	/* Go through keys and add values to the return array */
	ZEND_HASH_FILL_PACKED(keys) {
		zval *key;
		TEDS_STRICTHASHMAP_FOREACH_KEY(&intern->array, key) {
			Z_TRY_ADDREF_P(key);
			ZEND_HASH_FILL_ADD(key);
		} TEDS_STRICTHASHMAP_FOREACH_END();
	} ZEND_HASH_FILL_END();
	RETURN_ARR(keys);
}

PHP_METHOD(Teds_StrictHashMap, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);
	const uint32_t len = intern->array.nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		zval *value;
		TEDS_STRICTHASHMAP_FOREACH_VAL(&intern->array, value) {
			Z_TRY_ADDREF_P(value);
			ZEND_HASH_FILL_ADD(value);
		} TEDS_STRICTHASHMAP_FOREACH_END();
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

static teds_stricthashmap_entry *teds_stricthashmap_entries_find_value(const teds_stricthashmap_entries *array, zval *value)
{
	teds_stricthashmap_entry *it;
	TEDS_STRICTHASHMAP_FOREACH_BUCKET(array, it) {
		if (teds_is_identical_inline(value, &it->value)) {
			return it;
		}
	} TEDS_STRICTHASHMAP_FOREACH_END();
	return NULL;
}

static bool teds_stricthashmap_entries_remove_key(teds_stricthashmap_entries *array, zval *key)
{
	if (array->nNumOfElements == 0) {
		return false;
	}
	teds_stricthashmap_entry *const entries = array->arData;
	const uint32_t h = teds_strict_hash_uint32_t(key);
	teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_bucket(array, key, h);
	if (!entry) {
		return false;
	}
	ZEND_ASSERT(TEDS_STRICTHASHMAP_IT_HASH(entry) == h);

	const uint32_t nIndex = h | array->nTableMask;
	uint32_t i = HT_HASH(array, nIndex);
	teds_stricthashmap_entry *prev = NULL;

	const uint32_t idx = entry - entries;
	if (i != idx) {
		prev = &array->arData[i];
		while (TEDS_STRICTHASHMAP_IT_NEXT(prev) != idx) {
			i = TEDS_STRICTHASHMAP_IT_NEXT(prev);
			prev = &array->arData[i];
		}
	}
	if (prev) {
		TEDS_STRICTHASHMAP_IT_NEXT(prev) = TEDS_STRICTHASHMAP_IT_NEXT(entry);
	} else {
		HT_HASH(array, nIndex) = TEDS_STRICTHASHMAP_IT_NEXT(entry);
	}

	array->nNumOfElements--;
	if (array->nNumUsed - 1 == idx) {
		do {
			array->nNumUsed--;
		} while (array->nNumUsed > 0 && (UNEXPECTED(Z_TYPE(array->arData[array->nNumUsed-1].key) == IS_UNDEF)));
	}
	zval old_key;
	zval old_value;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	ZVAL_COPY_VALUE(&old_value, &entry->value);
	ZEND_ASSERT(array->nNumOfElements <= array->nNumUsed);
	ZVAL_UNDEF(&entry->key);
	ZVAL_UNDEF(&entry->value);

	/* Destructors may have side effects, run those last */
	zval_ptr_dtor(&old_value);
	zval_ptr_dtor(&old_key);
	return true;
}

static bool teds_stricthashmap_entries_offset_exists_and_not_null(const teds_stricthashmap_entries *array, zval *key)
{
	ZEND_ASSERT(Z_TYPE_P(key) != IS_REFERENCE);
	ZEND_ASSERT(Z_TYPE_P(key) != IS_UNDEF);

	if (array->nNumOfElements > 0) {
		teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_bucket_computing_hash(array, key);
		if (entry) {
			return Z_TYPE(entry->value) != IS_NULL;
		}
	}
	return false;
}

PHP_METHOD(Teds_StrictHashMap, offsetExists)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stricthashmap_entries *array = Z_STRICTHASHMAP_ENTRIES_P(ZEND_THIS);
	RETURN_BOOL(teds_stricthashmap_entries_offset_exists_and_not_null(array, key));
}

PHP_METHOD(Teds_StrictHashMap, offsetGet)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);
	if (intern->array.nNumOfElements > 0) {
		teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_bucket_computing_hash(&intern->array, key);
		if (entry) {
			RETURN_COPY(&entry->value);
		}
	}
	zend_throw_exception(spl_ce_OutOfBoundsException, "Key not found", 0);
	RETURN_THROWS();
}

PHP_METHOD(Teds_StrictHashMap, get)
{
	zval *key;
	zval *default_zv = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_zv)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);
	if (intern->array.nNumOfElements > 0) {
		teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_bucket_computing_hash(&intern->array, key);
		if (entry) {
			RETURN_COPY(&entry->value);
		}
	}
	if (default_zv != NULL) {
		RETURN_COPY(default_zv);
	}
	zend_throw_exception(spl_ce_OutOfBoundsException, "Key not found", 0);
	RETURN_THROWS();
}

PHP_METHOD(Teds_StrictHashMap, offsetSet)
{
	zval *key;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);
	teds_stricthashmap_entries_insert(&intern->array, key, value, false);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_StrictHashMap, offsetUnset)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_stricthashmap_entries_remove_key(Z_STRICTHASHMAP_ENTRIES_P(ZEND_THIS), key);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_StrictHashMap, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stricthashmap_entries *array = Z_STRICTHASHMAP_ENTRIES_P(ZEND_THIS);
	if (array->nTableSize > 0) {
		teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_value(array, value);
		RETURN_BOOL(entry != NULL);
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_StrictHashMap, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stricthashmap_entries *array = Z_STRICTHASHMAP_ENTRIES_P(ZEND_THIS);
	if (array->nNumOfElements > 0) {
		teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_bucket_computing_hash(array, key);
		RETURN_BOOL(entry != NULL);
	}
	RETURN_FALSE;
}

static void teds_stricthashmap_entries_return_pairs(teds_stricthashmap_entries *array, zval *return_value)
{
	uint32_t len = array->nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		zval *key, *val;
		TEDS_STRICTHASHMAP_FOREACH_KEY_VAL(array, key, val) {
			zval tmp;
			Z_TRY_ADDREF_P(key);
			Z_TRY_ADDREF_P(val);
			ZVAL_ARR(&tmp, zend_new_pair(key, val));
			ZEND_HASH_FILL_ADD(&tmp);
		} TEDS_STRICTHASHMAP_FOREACH_END();
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_StrictHashMap, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricthashmap_entries_return_pairs(Z_STRICTHASHMAP_ENTRIES_P(ZEND_THIS), return_value);
}

PHP_METHOD(Teds_StrictHashMap, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricthashmap_entries *const array = Z_STRICTHASHMAP_ENTRIES_P(ZEND_THIS);
	if (!array->nNumOfElements) {
		RETURN_EMPTY_ARRAY();
	}

	zend_array *values = zend_new_array(array->nNumOfElements);
	zval *key, *val;
	/* TODO: Check for side effects of notices */
	TEDS_STRICTHASHMAP_FOREACH_KEY_VAL(array, key, val) {
		Z_TRY_ADDREF_P(val);
		array_set_zval_key(values, key, val);
		zval_ptr_dtor_nogc(val);
		if (UNEXPECTED(EG(exception))) {
			zend_array_destroy(values);
			RETURN_THROWS();
		}
	} TEDS_STRICTHASHMAP_FOREACH_END();
	RETURN_ARR(values);
}

static void teds_stricthashmap_entries_clear(teds_stricthashmap_entries *array) {
	if (teds_stricthashmap_entries_empty_capacity(array)) {
		return;
	}
	teds_stricthashmap_entries array_copy = *array;

	teds_stricthashmap_entries_set_empty_entry_list(array);

	teds_stricthashmap_entries_dtor(&array_copy);
	/* Could call teds_stricthashmap_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StrictHashMap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricthashmap *intern = Z_STRICTHASHMAP_P(ZEND_THIS);
	teds_stricthashmap_entries_clear(&intern->array);
	TEDS_RETURN_VOID();
}

static void teds_stricthashmap_write_dimension(zend_object *object, zval *offset_zv, zval *value)
{
	teds_stricthashmap_entries *array = teds_stricthashmap_entries_from_object(object);
	if (UNEXPECTED(!offset_zv || Z_TYPE_P(offset_zv) == IS_UNDEF)) {
		zend_throw_exception(spl_ce_RuntimeException, "Teds\\StrictHashMap does not support appending with []=", 0);
		return;
	}

	ZVAL_DEREF(offset_zv);
	ZVAL_DEREF(value);
	teds_stricthashmap_entries_insert(array, offset_zv, value, false);
}

static void teds_stricthashmap_unset_dimension(zend_object *object, zval *offset)
{
	teds_stricthashmap_entries *array = teds_stricthashmap_entries_from_object(object);

	ZVAL_DEREF(offset);
	teds_stricthashmap_entries_remove_key(array, offset);
}

static zval *teds_stricthashmap_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (UNEXPECTED(!offset_zv || Z_ISUNDEF_P(offset_zv))) {
handle_missing_key:
		if (type != BP_VAR_IS) {
			zend_throw_exception(spl_ce_OutOfBoundsException, "Key not found", 0);
			return NULL;
		}
		return &EG(uninitialized_zval);
	}

	const teds_stricthashmap_entries *array = teds_stricthashmap_entries_from_object(object);
	ZVAL_DEREF(offset_zv);

	(void)rv;

	if (array->nNumOfElements > 0) {
		teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_bucket_computing_hash(array, offset_zv);
		if (entry) {
			return &entry->value;
		}
	}
	goto handle_missing_key;
}

static int teds_stricthashmap_has_dimension(zend_object *object, zval *offset_zv, int check_empty)
{
	ZVAL_DEREF(offset_zv);
	const teds_stricthashmap_entries *array = teds_stricthashmap_entries_from_object(object);
	if (array->nNumOfElements > 0) {
		teds_stricthashmap_entry *entry = teds_stricthashmap_entries_find_bucket_computing_hash(array, offset_zv);
		if (entry) {
			return teds_has_dimension_helper(&entry->value, check_empty);
		}
	}
	return teds_stricthashmap_entries_offset_exists_and_not_null(array, offset_zv);
}

PHP_MINIT_FUNCTION(teds_stricthashmap)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_StrictHashMap = register_class_Teds_StrictHashMap(zend_ce_aggregate, teds_ce_Map, php_json_serializable_ce);
	teds_ce_StrictHashMap->create_object = teds_stricthashmap_new;

	memcpy(&teds_handler_StrictHashMap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictHashMap.offset          = XtOffsetOf(teds_stricthashmap, std);
	teds_handler_StrictHashMap.clone_obj       = teds_stricthashmap_clone;
	teds_handler_StrictHashMap.count_elements  = teds_stricthashmap_count_elements;
	teds_handler_StrictHashMap.get_properties  = teds_stricthashmap_get_properties;
	teds_handler_StrictHashMap.get_gc          = teds_stricthashmap_get_gc;
	teds_handler_StrictHashMap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictHashMap.free_obj        = teds_stricthashmap_free_storage;
	teds_handler_StrictHashMap.write_dimension = teds_stricthashmap_write_dimension;
	teds_handler_StrictHashMap.has_dimension   = teds_stricthashmap_has_dimension;
	teds_handler_StrictHashMap.read_dimension  = teds_stricthashmap_read_dimension;
	teds_handler_StrictHashMap.unset_dimension = teds_stricthashmap_unset_dimension;

	teds_ce_StrictHashMap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StrictHashMap->get_iterator = teds_stricthashmap_get_iterator;

	return SUCCESS;
}
