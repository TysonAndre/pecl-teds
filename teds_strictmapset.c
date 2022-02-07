/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on teds_immutablekeyvaluesequence.c.
 * Instead of a C array of zvals, this is based on a C array of pairs of zvals for key-value entries */
/*
 * Design plan for refactoring:
 * - Buckets: Keys (zval, uint32_t hash) and values (zval) placed based on hash. Similar to HashTable Data Layout as described in Zend_types
 * - Hashes: hash and index, chaining.
 *
 * iteration:
 * - sequence of HashPosition
 * - TODO: associate StrictMap with linked list of iterators
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_strictmap_arginfo.h"
#include "teds_strictmap.h"
#include "teds_util.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_StrictMap;
zend_class_entry *teds_ce_StrictMap;

#undef HT_HASH_TO_BUCKET_EX
#undef HT_IDX_TO_HASH
#undef HT_HASH_TO_IDX

#define TEDS_ENTRY_HASH(entry) ((entry)->h)

/* Need at least 2 for iteration base cases */
const uint32_t teds_strictmap_empty_bucket_list[2] = { TEDS_STRICTMAP_INVALID_INDEX, TEDS_STRICTMAP_INVALID_INDEX };

static void teds_strictmap_entries_grow(teds_strictmap_entries *array);
static void teds_strictmap_entries_set_capacity(teds_strictmap_entries *array, size_t new_capacity);

static zend_always_inline teds_strictmap_entry *teds_strictmap_entries_find_bucket(const teds_strictmap_entries *ht, zval *key, const uint32_t h)
{
	teds_strictmap_entry *p;

	teds_strictmap_entry *const arData = ht->arData;
	const uint32_t nIndex = h | ht->nTableMask;
	uint32_t idx = HT_HASH_EX(arData, nIndex);
	//fprintf(stderr, "Lookup %x nIndex=%d idx=%d\n", (int)h, (int)nIndex, (int)idx);
	while (idx != TEDS_STRICTMAP_INVALID_INDEX) {
		ZEND_ASSERT(idx < ht->nTableSize);
		p = arData + idx;
		//fprintf(stderr, "Lookup %x idx=%d p->h=%x type=%d\n", (int)h, (int)idx, (int)p->h, Z_TYPE(p->key));
		if ((p->h == h) && zend_is_identical(&p->key, key)) {
			return p;
		}
		//fprintf(stderr, "Lookup not identical\n");
		idx = TEDS_STRICTMAP_IT_NEXT(p);
	}
	return NULL;
}
/* Returns true if a new entry was added to the map. Based on _zend_hash_add_or_update_i. */
static zend_always_inline bool teds_strictmap_entries_insert(teds_strictmap_entries *array, zval *key, bool add_new)
{
	teds_strictmap_entry *p;
	ZEND_ASSERT(Z_TYPE_P(key) != IS_UNDEF);

	const zend_ulong h = teds_strict_hash_uint32_t(key);

	if (UNEXPECTED(array->nTableSize == 0)) {
		teds_strictmap_entries_set_capacity(array, TEDS_STRICTMAP_MIN_CAPACITY);
		goto add_to_hash;
	} else if (!add_new || ZEND_DEBUG) {
		p = teds_strictmap_entries_find_bucket(array, key, h);

		if (p) {
			ZEND_ASSERT(!add_new);
			return false;
		}
	}

	/* If the Hash table is full, resize it */
	if (UNEXPECTED(array->nNumUsed >= array->nTableSize)) {
		teds_strictmap_entries_grow(array);
	}

add_to_hash:
	;
	const uint32_t idx = array->nNumUsed;
	teds_strictmap_entry *const arData = array->arData;
	p = arData + idx;
	const uint32_t nIndex = h | array->nTableMask;
	array->nNumUsed++;
	array->nNumOfElements++;
	p->h = h;
	TEDS_STRICTMAP_IT_NEXT(p) = HT_HASH_EX(arData, nIndex);
	HT_HASH_EX(arData, nIndex) = idx;
	ZVAL_COPY(&p->key, key);

	return true;
}


static void teds_strictmap_entries_clear(teds_strictmap_entries *array);

/* Used by InternalIterator returned by StrictMap->getIterator() */
typedef struct _teds_strictmap_it {
	zend_object_iterator intern;
	zend_ulong           current;
} teds_strictmap_it;

static zend_always_inline teds_strictmap *teds_strictmap_from_obj(zend_object *obj)
{
	return (teds_strictmap*)((char*)(obj) - XtOffsetOf(teds_strictmap, std));
}

#define Z_STRICTMAP_P(zv)  teds_strictmap_from_obj(Z_OBJ_P((zv)))

static zend_always_inline void teds_strictmap_entry_compute_and_store_hash(teds_strictmap_entry *entry) {
	TEDS_ENTRY_HASH(entry) = teds_strict_hash_uint32_t(&entry->key);
}

static zend_always_inline bool teds_strictmap_entries_uninitialized(teds_strictmap_entries *array)
{
	ZEND_ASSERT(array->nNumOfElements <= array->nTableSize);
	ZEND_ASSERT(array->nNumOfElements <= array->nNumUsed);
	if (array->arData == NULL) {
		ZEND_ASSERT(array->nTableSize == 0);
		return true;
	}
	ZEND_ASSERT(array->nTableMask > 0);
	ZEND_ASSERT(array->nTableMask <= TEDS_STRICTMAP_MIN_MASK);
#if ZEND_DEBUG
	if (array->arData == teds_strictmap_empty_entry_list) {
		ZEND_ASSERT(array->nTableSize == 0 && array->nTableMask == HT_MIN_MASK);
	} else {
		ZEND_ASSERT(array->nTableSize > 0);
		ZEND_ASSERT(array->nTableMask < HT_MIN_MASK);
	}
#endif
	return false;
}

static zend_always_inline void teds_strictmap_entries_set_empty_entry_list(teds_strictmap_entries *array)
{
	array->nNumOfElements = 0;
	array->nNumUsed = 0;
	array->nTableSize = 0;
	array->arData = teds_strictmap_empty_entry_list;
	array->nTableMask = TEDS_STRICTMAP_MIN_MASK;
}

static zend_always_inline size_t teds_strictmap_offset_bytes_for_capacity(size_t capacity) {
	return (capacity * sizeof(uint32_t) * 2);
}

static teds_strictmap_entry *teds_strictmap_alloc_entries(size_t capacity) {
	uint8_t *ptr = safe_emalloc(capacity, TEDS_STRICTMAP_MEMORY_PER_ENTRY, 0);
	const size_t buckets_byte_count = teds_strictmap_offset_bytes_for_capacity(capacity);
	memset(ptr, TEDS_STRICTMAP_INVALID_INDEX, buckets_byte_count);
	return (void *)(ptr + buckets_byte_count);
}

static zend_always_inline void teds_strictmap_free_entries(teds_strictmap_entry *old_entries, size_t old_capacity) {
	void * old_ptr = ((uint8_t *) old_entries) - teds_strictmap_offset_bytes_for_capacity(old_capacity);
	efree(old_ptr);
}

void teds_strictmap_entries_init_from_array(teds_strictmap_entries *array, zend_array *values)
{
	uint32_t size = zend_hash_num_elements(values);
	if (size > 0) {
		zval *val;
		/* Avoid allocating too much if there are duplicates? */
		zend_long capacity = TEDS_STRICTMAP_MIN_CAPACITY;

		array->nNumOfElements = 0; /* reset size in case emalloc() fails */
		array->nNumUsed = 0;
		array->nTableSize = 0;
		teds_strictmap_entries_set_capacity(array, capacity);
		/* NOTE: Unlike StrictMap's init_from_array, where keys are unique, this is creating a StrictMap from the values of the array */
		ZEND_HASH_FOREACH_VAL(values, val)  {
			teds_strictmap_entries_insert(array, val, false);
		} ZEND_HASH_FOREACH_END();
	} else {
		teds_strictmap_entries_set_empty_entry_list(array);
	}
}

void teds_strictmap_entries_init_from_traversable(teds_strictmap_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_strictmap_entries_set_empty_entry_list(array);
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
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception)) || value == NULL) {
			break;
		}

		ZVAL_DEREF(value);
		teds_strictmap_entries_insert(array, value, false);

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
	}

	if (iter) {
		zend_iterator_dtor(iter);
	}
	if (UNEXPECTED(EG(exception))) {
		teds_strictmap_entries_clear(array);
	}
}

/* Based on zend_hash_rehash. */
static void teds_strictmap_entries_rehash_inplace(teds_strictmap_entries *ht)
{
	//fprintf(stderr, "Rehash inplace %d %d\n", (int)ht->nNumOfElements, (int)ht->nTableSize);
	ZEND_ASSERT(ht->nNumOfElements > 0);

	HT_HASH_RESET(ht);
	uint32_t i = 0;
	teds_strictmap_entry *p = ht->arData;
	do {
		if (Z_TYPE(p->key) == IS_UNDEF) {
			uint32_t j = i;
			teds_strictmap_entry *q = p;

			while (++i < ht->nNumUsed) {
				p++;
				if (EXPECTED(Z_TYPE_INFO(p->key) != IS_UNDEF)) {
					ZVAL_COPY_VALUE(&q->key, &p->key);
					q->h = p->h;
					const uint32_t nIndex = q->h | ht->nTableMask;
					TEDS_STRICTMAP_IT_NEXT(q) = HT_HASH(ht, nIndex);
					HT_HASH(ht, nIndex) = j;
					q++;
					j++;
				}
			}
			ht->nNumUsed = j;
			break;
		}
		const uint32_t nIndex = p->h | ht->nTableMask;
		TEDS_STRICTMAP_IT_NEXT(p) = HT_HASH(ht, nIndex);
		HT_HASH(ht, nIndex) = i;
		p++;
	} while (++i < ht->nNumUsed);

	ZEND_ASSERT(ht->nNumUsed == ht->nNumOfElements);
}


static void teds_strictmap_entries_grow(teds_strictmap_entries *array)
{
	if (teds_strictmap_entries_empty_capacity(array)) {
		array->arData = teds_strictmap_alloc_entries(TEDS_STRICTMAP_MIN_CAPACITY);
		array->nTableSize = TEDS_STRICTMAP_MIN_CAPACITY;
		array->nTableMask = TEDS_STRICTMAP_SIZE_TO_MASK(TEDS_STRICTMAP_MIN_CAPACITY);
		return;
	}

	ZEND_ASSERT(array->nNumUsed >= array->nNumOfElements);
	if (array->nNumUsed > array->nNumOfElements + (array->nNumOfElements >> 5)) { /* additional term is there to amortize the cost of compaction */
		teds_strictmap_entries_rehash_inplace(array);
		return;
	}
	ZEND_ASSERT(teds_is_pow2(array->nTableSize));

	const size_t new_capacity = array->nTableSize * 2;
	teds_strictmap_entry *const new_entries = teds_strictmap_alloc_entries(new_capacity);
	teds_strictmap_entry *old_entry;
	teds_strictmap_entry *it = new_entries;
	ZEND_ASSERT(array->nNumOfElements <= array->nNumUsed);
	uint32_t i = 0;
	const uint32_t new_mask = TEDS_STRICTMAP_SIZE_TO_MASK(new_capacity);
	TEDS_STRICTMAP_FOREACH_BUCKET(array, old_entry) {
		/* TODO: This can be optimized based on zend_hash_rehash's special cases, e.g. HT_IS_WITHOUT_HOLES */
		const uint32_t h = TEDS_ENTRY_HASH(old_entry);
		const uint32_t nIndex = h | new_mask;
		ZVAL_COPY_VALUE(&it->key, &old_entry->key);
		//fprintf(stderr, "Copying %x %d to i=%d type=%d val=%s\n", (int)h, (int)-nIndex, i, (int)Z_TYPE(it->key), (Z_TYPE(it->key) != IS_STRING ? "()" : Z_STRVAL(it->key)));
		TEDS_ENTRY_HASH(it) = h;
		TEDS_STRICTMAP_IT_NEXT(it) = HT_HASH_EX(new_entries, nIndex);
		HT_HASH_EX(new_entries, nIndex) = i;
		it++;
		i++;
	} TEDS_STRICTMAP_FOREACH_END();
	ZEND_ASSERT(it - new_entries == array->nNumOfElements);
	array->nNumUsed = array->nNumOfElements;
	teds_strictmap_free_entries(array->arData, array->nTableSize);
	array->arData = new_entries;

	array->nTableSize = new_capacity;
	array->nTableMask = new_mask;
}

/* TODO teds_strictmap_do_resize */

static void teds_strictmap_entries_set_capacity(teds_strictmap_entries *array, size_t new_capacity)
{
	ZEND_ASSERT(array->nTableSize == 0);
	ZEND_ASSERT(new_capacity > array->nTableSize);
	ZEND_ASSERT(teds_is_pow2(new_capacity));

	array->arData = teds_strictmap_alloc_entries(new_capacity);
	array->nTableSize = new_capacity;
	array->nTableMask = TEDS_STRICTMAP_SIZE_TO_MASK(new_capacity);
}

static void teds_strictmap_entries_copy_ctor(teds_strictmap_entries *to, teds_strictmap_entries *from)
{
	const zend_long size = from->size;
	if (!size) {
		teds_strictmap_entries_set_empty_entry_list(to);
		return;
	}

	const size_t capacity = from->nTableSize;
	teds_strictmap_entries_set_capacity(to, capacity);

	/* TODO optimize this */
	zval *key, *val;
	TEDS_STRICTMAP_FOREACH_KEY_VAL(from, key, val) {
		teds_strictmap_entries_insert(to, key, val, true);
	} TEDS_STRICTMAP_FOREACH_END();
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
void teds_strictmap_entries_dtor(teds_strictmap_entries *array)
{
	if (teds_strictmap_entries_empty_capacity(array)) {
		return;
	}
	zval *tmp;
	TEDS_STRICTMAP_FOREACH_VAL(array, tmp) {
		zval_ptr_dtor(tmp);
	} TEDS_STRICTMAP_FOREACH_END();
	teds_strictmap_entries_release(array);
}

static HashTable* teds_strictmap_get_gc(zend_object *obj, zval **table, int *table_count)
{
	teds_strictmap *intern = teds_strictmap_from_obj(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (intern->array.nNumOfElements > 0) {
		zval *val;

		TEDS_STRICTMAP_FOREACH_VAL(&intern->array, val) {
			zend_get_gc_buffer_add_zval(gc_buffer, val);
		} TEDS_STRICTMAP_FOREACH_END();
	}
	zend_get_gc_buffer_use(gc_buffer, table, table_count);

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_strictmap_get_properties(zend_object *obj)
{
	teds_strictmap *intern = teds_strictmap_from_obj(obj);
	uint32_t len = intern->array.nNumOfElements;
	HashTable *ht = zend_std_get_properties(obj);
	size_t old_length = zend_hash_num_elements(ht);
	/* Initialize properties array */
	size_t i = 0;
	zval *val;
	ZEND_ASSERT(len <= intern->array.nNumUsed);
	TEDS_STRICTMAP_FOREACH_VAL(&intern->array, val) {
		Z_TRY_ADDREF_P(val);
		zend_hash_index_update(ht, i, val);
		i++;
	} TEDS_STRICTMAP_FOREACH_END();

	ZEND_ASSERT(i == len);

	for (size_t i = len; i < old_length; i++) {
		zend_hash_index_del(ht, i);
	}

	return ht;
}

static void teds_strictmap_free_storage(zend_object *object)
{
	teds_strictmap *intern = teds_strictmap_from_obj(object);
	teds_strictmap_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_strictmap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_strictmap *intern;

	intern = zend_object_alloc(sizeof(teds_strictmap), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StrictMap);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_StrictMap;

	if (orig && clone_orig) {
		teds_strictmap *other = teds_strictmap_from_obj(orig);
		teds_strictmap_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.arData = NULL;
	}

	return &intern->std;
}

static zend_object *teds_strictmap_new(zend_class_entry *class_type)
{
	return teds_strictmap_new_ex(class_type, NULL, 0);
}


static zend_object *teds_strictmap_clone(zend_object *old_object)
{
	zend_object *new_object = teds_strictmap_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_strictmap_count_elements(zend_object *object, zend_long *count)
{
	teds_strictmap *intern;

	intern = teds_strictmap_from_obj(object);
	*count = intern->array.nNumOfElements;
	return SUCCESS;
}

/* Get number of entries in this StrictMap */
PHP_METHOD(Teds_StrictMap, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictmap *intern = Z_STRICTMAP_P(object);
	RETURN_LONG(intern->array.nNumOfElements);
}

/* Get whether this StrictMap is empty */
PHP_METHOD(Teds_StrictMap, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_strictmap *intern = Z_STRICTMAP_P(object);
	RETURN_BOOL(intern->array.nNumOfElements == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_StrictMap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_strictmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictMap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		teds_strictmap_entries_set_empty_entry_list(&intern->array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_strictmap_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_strictmap_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StrictMap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_strictmap_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_strictmap_it_rewind(zend_object_iterator *iter)
{
	((teds_strictmap_it*)iter)->current = 0;
}

static int teds_strictmap_it_valid(zend_object_iterator *iter)
{
	teds_strictmap_it *iterator = (teds_strictmap_it*)iter;
	teds_strictmap *object = Z_STRICTMAP_P(&iter->data);

	while (1) {
		if (iterator->current >= object->array.nNumUsed) {
			return FAILURE;
		}
		zval *v = &object->array.arData[iterator->current].key;
		if (Z_TYPE_P(v) != IS_UNDEF) {
			return SUCCESS;
		}
		iterator->current++;
	}
}

static teds_strictmap_entry *teds_strictmap_read_offset_helper(teds_strictmap *intern, size_t offset)
{
	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (UNEXPECTED(offset >= intern->array.nNumOfElements)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		return NULL;
	} else {
		return &intern->array.arData[offset];
	}
}

static zval *teds_strictmap_it_get_current_data(zend_object_iterator *iter)
{
	teds_strictmap_it     *iterator = (teds_strictmap_it*)iter;
	teds_strictmap *object   = Z_STRICTMAP_P(&iter->data);

	teds_strictmap_entry *data = teds_strictmap_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return &data->value;
	}
}

static void teds_strictmap_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_strictmap_it     *iterator = (teds_strictmap_it*)iter;
	teds_strictmap *object   = Z_STRICTMAP_P(&iter->data);

	teds_strictmap_entry *data = teds_strictmap_read_offset_helper(object, iterator->current);

	if (data == NULL) {
		ZVAL_NULL(key);
	} else {
		ZVAL_COPY(key, &data->key);
	}
}

static void teds_strictmap_it_move_forward(zend_object_iterator *iter)
{
	((teds_strictmap_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_strictmap_it_funcs = {
	teds_strictmap_it_dtor,
	teds_strictmap_it_valid,
	teds_strictmap_it_get_current_data,
	teds_strictmap_it_get_current_key,
	teds_strictmap_it_move_forward,
	teds_strictmap_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_strictmap_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_strictmap_it *iterator;
	(void) ce;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_strictmap_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_strictmap_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_StrictMap, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	size_t raw_size = zend_hash_num_elements(raw_data);
	teds_strictmap *const intern = Z_STRICTMAP_P(ZEND_THIS);
	if (UNEXPECTED(!teds_strictmap_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	teds_strictmap_entries *const array = &intern->array;
	ZEND_ASSERT(array->arData == NULL);
	if (raw_size == 0) {
		teds_strictmap_entries_set_empty_entry_list(array);
		return;
	}

	const size_t capacity = teds_strictmap_next_pow2_capacity(raw_size);
	teds_strictmap_entries_set_capacity(array, capacity);

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_strictmap_entries_clear(&intern->array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictMap::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}

		teds_strictmap_entries_insert(&intern->array, val, false);
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Teds_StrictMap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_strictmap_new(teds_ce_StrictMap);
	teds_strictmap *intern = teds_strictmap_from_obj(object);
	teds_strictmap_entries_init_from_array(&intern->array, array_ht);

	RETURN_OBJ(object);
}

static zend_array *teds_strictmap_create_array_copy(teds_strictmap *intern) {
	const size_t len = intern->array.nNumOfElements;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		zval *tmp;
		TEDS_STRICTMAP_FOREACH_VAL(&intern->array, tmp) {
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		} TEDS_STRICTMAP_FOREACH_END();
	} ZEND_HASH_FILL_END();
	return values;
}

PHP_METHOD(Teds_StrictMap, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	size_t len = intern->array.nNumOfElements;
	if (!len) {
		/* NOTE: This macro sets immutable gc flags, differently from RETURN_ARR */
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_strictmap_create_array_copy(intern));
}

static bool teds_strictmap_entries_remove_key(teds_strictmap_entries *array, zval *key)
{
	if (array->nNumOfElements == 0) {
		return false;
	}
	teds_strictmap_entry *const entries = array->arData;
	const uint32_t h = teds_strict_hash_uint32_t(key);
	teds_strictmap_entry *entry = teds_strictmap_entries_find_bucket(array, key, h);
	if (!entry) {
		return false;
	}
	ZEND_ASSERT(entry->h == h);

	const uint32_t nIndex = h | array->nTableMask;
	uint32_t i = HT_HASH(array, nIndex);
	teds_strictmap_entry *prev = NULL;

	const uint32_t idx = entry - entries;
	if (i != idx) {
		prev = &array->arData[i];
		while (TEDS_STRICTMAP_IT_NEXT(prev) != idx) {
			i = TEDS_STRICTMAP_IT_NEXT(prev);
			prev = &array->arData[i];
		}
	}
	if (prev) {
		TEDS_STRICTMAP_IT_NEXT(prev) = TEDS_STRICTMAP_IT_NEXT(entry);
	} else {
		HT_HASH(array, nIndex) = TEDS_STRICTMAP_IT_NEXT(entry);
	}

	array->nNumOfElements--;
	if (array->nNumUsed - 1 == idx) {
		do {
			array->nNumUsed--;
		} while (array->nNumUsed > 0 && (UNEXPECTED(Z_TYPE(array->arData[array->nNumUsed-1].key) == IS_UNDEF)));
	}
	zval old_key;
	ZVAL_COPY_VALUE(&old_key, &entry->key);
	ZEND_ASSERT(array->nNumOfElements <= array->nNumUsed);
	ZVAL_UNDEF(&entry->key);

	/* Destructors may have side effects, run those last */
	zval_ptr_dtor(&old_key);
	return true;
}

PHP_METHOD(Teds_StrictMap, remove)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	RETURN_BOOL(teds_strictmap_entries_remove_key(&intern->array, value));
}

PHP_METHOD(Teds_StrictMap, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	RETURN_BOOL(teds_strictmap_entries_insert(&intern->array, value, false));
}

PHP_METHOD(Teds_StrictMap, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	if (intern->array.nTableSize == 0) {
		RETURN_FALSE;
	}

	teds_strictmap_entry *entry = teds_strictmap_entries_find_bucket(&intern->array, value, teds_strict_hash_uint32_t(value));
	RETURN_BOOL(entry != NULL);
}

static void teds_strictmap_entries_clear(teds_strictmap_entries *array) {
	if (teds_strictmap_entries_empty_capacity(array)) {
		return;
	}
	teds_strictmap_entries array_copy = *array;

	teds_strictmap_entries_set_empty_entry_list(array);

	teds_strictmap_entries_dtor(&array_copy);
	/* Could call teds_strictmap_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StrictMap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_strictmap *intern = Z_STRICTMAP_P(ZEND_THIS);
	teds_strictmap_entries_clear(&intern->array);

	(void) return_value;
}

PHP_MINIT_FUNCTION(teds_strictmap)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_StrictMap = register_class_Teds_StrictMap(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce);
	teds_ce_StrictMap->create_object = teds_strictmap_new;

	memcpy(&teds_handler_StrictMap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictMap.offset          = XtOffsetOf(teds_strictmap, std);
	teds_handler_StrictMap.clone_obj       = teds_strictmap_clone;
	teds_handler_StrictMap.count_elements  = teds_strictmap_count_elements;
	teds_handler_StrictMap.get_properties  = teds_strictmap_get_properties;
	teds_handler_StrictMap.get_gc          = teds_strictmap_get_gc;
	teds_handler_StrictMap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictMap.free_obj        = teds_strictmap_free_storage;

	teds_ce_StrictMap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StrictMap->get_iterator = teds_strictmap_get_iterator;

	return SUCCESS;
}
