/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c but has lower overhead (when size is known) and is more efficient to push and remove elements from the end of the list */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds.h"
#include "teds_lowmemoryvector_arginfo.h"
#include "teds_lowmemoryvector.h"
#include "teds_interfaces.h"
#include "teds_exceptions.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"
#include "teds_serialize_util.h"

#include <stdbool.h>

/* Though rare, it is possible to have 64-bit zend_longs and a 32-bit size_t. */
#define MAX_INT_COUNT ((SIZE_MAX / 2) - 1)
#define MAX_VALID_OFFSET ((size_t)(MAX_INT_COUNT > ZEND_LONG_MAX ? ZEND_LONG_MAX : MAX_INT_COUNT))

zend_object_handlers teds_handler_LowMemoryVector;
zend_class_entry *teds_ce_LowMemoryVector;

/* This is a placeholder value to distinguish between empty and uninitialized LowMemoryVector instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const zval empty_entry_list[1];

#define _LMV_TYPE_BOOL_BITSET   0
#define LMV_TYPE_UNINITIALIZED  0
#define LMV_TYPE_BOOL_OR_NULL   1
#define LMV_TYPE_INT8           2
#define LMV_TYPE_INT16          3
#define LMV_TYPE_INT32          4
#if SIZEOF_ZEND_LONG > 4
#define LMV_TYPE_INT64          5
#else
#endif
#define LMV_TYPE_DOUBLE         6
#define LMV_TYPE_LAST_NONREFCOUNTED LMV_TYPE_DOUBLE
#define LMV_TYPE_LAST_GC_NO_SIDE_EFFECTS LMV_TYPE_LAST_NONREFCOUNTED

#define LMV_TYPE_ZVAL           7
#define LMV_TYPE_COUNT          LMV_TYPE_ZVAL + 1

#ifdef LMV_TYPE_INT64
#if LMV_TYPE_LAST_NONREFCOUNTED + 1 != LMV_TYPE_ZVAL
#error need to update tables
#endif
#endif
static const uint8_t teds_lmv_memory_per_element[LMV_TYPE_COUNT] = {
	0,
	sizeof(int8_t),  /* LMV_TYPE_BOOL_OR_NULL */
	sizeof(int8_t),  /* LMV_TYPE_INT8 */
	sizeof(int16_t), /* LMV_TYPE_INT16 */
	sizeof(int32_t), /* LMV_TYPE_INT32 */
	sizeof(int64_t), /* LMV_TYPE_INT64 */
	sizeof(double),  /* LMV_TYPE_DOUBLE */
	sizeof(zval),    /* LMV_TYPE_ZVAL */
};
static const uint8_t teds_lmv_shift_for_element[LMV_TYPE_COUNT] = {
	0,
	0, /* LMV_TYPE_BOOL_OR_NULL */
	0, /* LMV_TYPE_INT8 */
	1, /* LMV_TYPE_INT16 */
	2, /* LMV_TYPE_INT32 */
	3, /* LMV_TYPE_INT64 */
	3, /* LMV_TYPE_DOUBLE, 8 bytes = 1 << 3 */
	4, /* LMV_TYPE_ZVAL, 16 bytes = 1 << 4, but not even used */
};

typedef struct _teds_lowmemoryvector_entries {
	uint32_t size;
	uint32_t capacity;
	union {
		uint8_t     *entries_uint8;
		int8_t      *entries_int8;
		int16_t     *entries_int16;
		int32_t     *entries_int32;
#ifdef LMV_TYPE_INT64
		int64_t     *entries_int64;
#endif
		double      *entries_double;
		/*zend_string *entries_string; */
		zval        *entries_zval;
		void        *entries_raw;
	};
	int8_t type_tag;
} teds_lowmemoryvector_entries;

typedef struct _teds_lowmemoryvector {
	teds_lowmemoryvector_entries		array;
	zend_object				std;
} teds_lowmemoryvector;

/* Used by InternalIterator returned by LowMemoryVector->getIterator() */
typedef struct _teds_lowmemoryvector_it {
	zend_object_iterator intern;
	size_t               current;
	/* Temporary memory location to store the most recent get_current_value() result */
	zval                 tmp;
} teds_lowmemoryvector_it;

static void teds_lowmemoryvector_entries_raise_capacity(teds_lowmemoryvector_entries *intern, const size_t new_capacity);
static zend_always_inline void teds_lowmemoryvector_entries_push(teds_lowmemoryvector_entries *array, const zval *value, const bool check_capacity);
static zend_always_inline void teds_lowmemoryvector_entries_update_type_tag(teds_lowmemoryvector_entries *array, const zval *val);
static zend_always_inline void teds_lowmemoryvector_entries_copy_offset(const teds_lowmemoryvector_entries *array, const size_t offset, zval *dst, bool increase_refcount, bool pop);
static zend_always_inline zval *teds_lowmemoryvector_entries_read_offset(const teds_lowmemoryvector_entries *intern, const zend_ulong offset, zval *tmp);
static void teds_lowmemoryvector_entries_init_from_array_values(teds_lowmemoryvector_entries *array, zend_array *raw_data);
static zend_array *teds_lowmemoryvector_entries_to_refcounted_array(const teds_lowmemoryvector_entries *array);

#ifdef LMV_TYPE_INT64
#define LMV_GENERATE_INT_CASES() \
	LMV_INT_CODEGEN(LMV_TYPE_INT8, int8_t,   entries_int8) \
	LMV_INT_CODEGEN(LMV_TYPE_INT16, int16_t, entries_int16) \
	LMV_INT_CODEGEN(LMV_TYPE_INT32, int32_t, entries_int32) \
	LMV_INT_CODEGEN(LMV_TYPE_INT64, int64_t, entries_int64)
#else
#define LMV_GENERATE_INT_CASES() \
	LMV_INT_CODEGEN(LMV_TYPE_INT8, int8_t,   entries_int8) \
	LMV_INT_CODEGEN(LMV_TYPE_INT16, int16_t, entries_int16) \
	LMV_INT_CODEGEN(LMV_TYPE_INT32, int32_t, entries_int32)
#endif


static zend_always_inline void teds_lowmemoryvector_entries_set_type(zval *dst, const uint8_t type) {
	ZEND_ASSERT(type >= IS_NULL && type <= IS_TRUE);
	Z_TYPE_INFO_P(dst) = type;
}

static zend_always_inline uint8_t teds_lowmemoryvector_entries_validate_type(const uint8_t type) {
	ZEND_ASSERT(type >= IS_NULL && type <= IS_TRUE);
	return type;
}

static zend_always_inline uint8_t teds_lowmemoryvector_entries_get_type(const zval *dst) {
	const uint8_t type = Z_TYPE_P(dst);
	ZEND_ASSERT(type >= IS_NULL && type <= IS_TRUE);
	return type;
}

/*
 * If a size this large is encountered, assume the allocation will likely fail or
 * future changes to the capacity will overflow.
 */
static ZEND_COLD void teds_error_noreturn_max_lowmemoryvector_capacity()
{
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\LowMemoryVector capacity");
}

static zend_always_inline teds_lowmemoryvector *teds_lowmemoryvector_from_object(zend_object *obj)
{
	return (teds_lowmemoryvector*)((char*)(obj) - XtOffsetOf(teds_lowmemoryvector, std));
}

#define Z_LOWMEMORYVECTOR_P(zv)  (teds_lowmemoryvector_from_object(Z_OBJ_P((zv))))
#define Z_LOWMEMORYVECTOR_ENTRIES_P(zv)  (&(Z_LOWMEMORYVECTOR_P((zv))->array))

static zend_always_inline bool teds_lowmemoryvector_entries_empty_capacity(const teds_lowmemoryvector_entries *array)
{
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries_zval != empty_entry_list);
		return false;
	}
	// This lowmemoryvector may have reserved capacity.
	return true;
}

static zend_always_inline bool teds_lowmemoryvector_entries_uninitialized(const teds_lowmemoryvector_entries *array)
{
	if (array->entries_raw == NULL) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		ZEND_ASSERT(array->type_tag == LMV_TYPE_UNINITIALIZED);
		return true;
	}
	ZEND_ASSERT((array->entries_zval == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static uint8_t teds_lowmemoryvector_entries_compute_memory_per_element(const teds_lowmemoryvector_entries *array) {
	ZEND_ASSERT(array->type_tag != LMV_TYPE_UNINITIALIZED);
	ZEND_ASSERT(array->type_tag < LMV_TYPE_COUNT);
	return teds_lmv_memory_per_element[array->type_tag];
}

static void teds_lowmemoryvector_entries_raise_capacity(teds_lowmemoryvector_entries *array, const size_t new_capacity) {
	ZEND_ASSERT(new_capacity > array->capacity);
	const uint8_t memory_per_element = teds_lowmemoryvector_entries_compute_memory_per_element(array);
	if (array->capacity == 0) {
		array->entries_raw = safe_emalloc(new_capacity, memory_per_element, 0);
	} else {
		array->entries_raw = safe_erealloc(array->entries_raw, new_capacity, memory_per_element, 0);
	}
	array->capacity = new_capacity;
	ZEND_ASSERT(array->entries_raw != NULL);
}

static inline void teds_lowmemoryvector_entries_shrink_capacity(teds_lowmemoryvector_entries *array, uint32_t size, uint32_t capacity, void *old_entries_raw) {
	ZEND_ASSERT(size <= capacity);
	ZEND_ASSERT(size == array->size);
	ZEND_ASSERT(capacity > 0);
	ZEND_ASSERT(capacity < array->capacity);
	ZEND_ASSERT(old_entries_raw == array->entries_raw);
	const uint8_t memory_per_element = teds_lowmemoryvector_entries_compute_memory_per_element(array);
	array->capacity = capacity;
	array->entries_raw = erealloc2(old_entries_raw, capacity * memory_per_element, size * memory_per_element);
	ZEND_ASSERT(array->entries_raw != NULL);
}

static zend_always_inline void teds_lowmemoryvector_entries_set_empty_list(teds_lowmemoryvector_entries *array) {
	array->size = 0;
	array->capacity = 0;
	array->type_tag = LMV_TYPE_UNINITIALIZED;
	array->entries_raw = (void *)empty_entry_list;
}

static void teds_lowmemoryvector_entries_init_from_traversable(teds_lowmemoryvector_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_lowmemoryvector_entries_set_empty_list(array);
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

	/* Reindex keys from 0. */
	while (funcs->valid(iter) == SUCCESS) {
		if (EG(exception)) {
			break;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		teds_lowmemoryvector_entries_push(array, value, true);

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
	}

	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [0, n) into the lowmemoryvector, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_lowmemoryvector_copy_range(teds_lowmemoryvector_entries *array, const int8_t *const start, const uint32_t n, const uint8_t bytes_per_element)
{
	if (array->type_tag <= LMV_TYPE_LAST_NONREFCOUNTED) {
		memcpy(array->entries_int8, start, bytes_per_element * n);
		return;
	}
	ZEND_ASSERT(array->type_tag == LMV_TYPE_ZVAL);
	zval *dst = array->entries_zval;
	const zval *src = (zval *)start;
	for (zval *end = dst + n; dst < end; dst++, src++) {
		ZVAL_COPY(dst, src);
	}
}

static void teds_lowmemoryvector_entries_copy_ctor(teds_lowmemoryvector_entries *to, const teds_lowmemoryvector_entries *from)
{
	zend_long size = from->size;
	if (!size) {
		teds_lowmemoryvector_entries_set_empty_list(to);
		return;
	}

	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0;
	to->type_tag = from->type_tag;
	const uint8_t bytes_per_element = teds_lowmemoryvector_entries_compute_memory_per_element(from);
	to->entries_int8 = safe_emalloc(size, bytes_per_element, 0);
	to->size = size;
	to->capacity = size;

	teds_lowmemoryvector_copy_range(to, from->entries_int8, size, bytes_per_element);
}

/* Destructs and frees contents but not the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_lowmemoryvector_entries_dtor(teds_lowmemoryvector_entries *array)
{
	if (!teds_lowmemoryvector_entries_empty_capacity(array)) {
		/* Get state before replacing entries with empty entry list in case of side effects of destructors */
		zval *const entries = array->entries_zval;
		const uint8_t type_tag = array->type_tag;
		const size_t size = array->size;

		/* Replace entries with empty entry list */
		teds_lowmemoryvector_entries_set_empty_list(array);

		/* Free the elements */
		if (type_tag > LMV_TYPE_LAST_NONREFCOUNTED) {
			ZEND_ASSERT(type_tag == LMV_TYPE_ZVAL);

			zval *it = entries;
			array->entries_zval = NULL;
			for (zval *const end = it + size; it < end; it++) {
				zval_ptr_dtor(it);
			}
		}
		efree(entries);
	}
}

static HashTable* teds_lowmemoryvector_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(obj)->array;

	uint32_t len = array->size;
	if (len == 0 || array->type_tag <= LMV_TYPE_LAST_GC_NO_SIDE_EFFECTS) {
		*n = 0;
		/* Deliberately return null if obj->properties has not yet been instantiated */
		return obj->properties;
	}
	ZEND_ASSERT(array->type_tag == LMV_TYPE_ZVAL);

	*n = (int)len;
	*table = array->entries_zval;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return obj->properties;
}

static HashTable* teds_lowmemoryvector_get_properties(zend_object *obj)
{
	teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(obj)->array;
	if (!array->size && !obj->properties) {
		/* Similar to ext/ffi/ffi.c zend_fake_get_properties */
		return (HashTable*)&zend_empty_array;
	}
	HashTable *ht = zend_std_get_properties(obj);

	/* XXX Here, we deliberately don't support leaking properties to var_export. The entire point is low memory. */
	/* Related to https://github.com/php/php-src/issues/8044 */

	/*
	 * Re-initialize properties array.
	 *
	 * Note that destructors may mutate the original array,
	 * so we fetch the size and circular buffer (and type tag) each time to avoid invalid memory accesses.
	 *
	 * json_encode needs distinct results of `*get_properties` (Z_OBJPROP) to detect infinite recursion.
	 * var_export needs `*get_properties_for` to return the same array every time to detect infinite recursion.
	 * var_dump is doing yet another different thing, and calling `Z_PROTECT_RECURSION(zval *struct)` to set the GC_FLAGS
	 * on the object?
	 *
	 * Garbage collection needs to get this property array to detect reference cycles because this also has references.
	 * But the point is to reduce memory usage, anyway.
	 *
	 * There's the option of allocating a **different** table for var_export to use, exclusively,
	 * and letting var_dump/var_export use their hash table detection?
	for (uint32_t i = 0; i < array->size; i++) {
		zval tmp;
		zval *elem = teds_lowmemoryvector_entries_read_offset(array, i, &tmp);
		Z_TRY_ADDREF_P(elem);
		zend_hash_index_update(ht, i, elem);
	}
	uint32_t properties_size = zend_hash_num_elements(ht);
	if (UNEXPECTED(properties_size > array->size)) {
		for (uint32_t i = array->size; i < properties_size; i++) {
			zend_hash_index_del(ht, i);
		}
	}
	 */

	return ht;
}

static HashTable* teds_lowmemoryvector_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(obj)->array;
	if (!array->size && !obj->properties) {
		/* Similar to ext/ffi/ffi.c zend_fake_get_properties */
		return (HashTable*)&zend_empty_array;
	}
	switch (purpose) {
		case ZEND_PROP_PURPOSE_DEBUG:
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
		case ZEND_PROP_PURPOSE_SERIALIZE:
		case ZEND_PROP_PURPOSE_JSON:
			return teds_lowmemoryvector_entries_to_refcounted_array(array);
		case ZEND_PROP_PURPOSE_VAR_EXPORT: {
			/* var_export uses get_properties_for for infinite recursion detection rather than get_properties(Z_OBJPROP).
			 * or checking for recursion on the object itself (php_var_dump) */
			HashTable *ht = teds_lowmemoryvector_get_properties(obj);
			GC_TRY_ADDREF(ht);
			return ht;
	    }
		default:
			ZEND_UNREACHABLE();
			return NULL;
	}
}

static void teds_lowmemoryvector_free_storage(zend_object *object)
{
	teds_lowmemoryvector *intern = teds_lowmemoryvector_from_object(object);
	teds_lowmemoryvector_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_lowmemoryvector_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_lowmemoryvector *intern;

	intern = zend_object_alloc(sizeof(teds_lowmemoryvector), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_LowMemoryVector);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_LowMemoryVector;

	if (orig && clone_orig) {
		teds_lowmemoryvector *other = teds_lowmemoryvector_from_object(orig);
		teds_lowmemoryvector_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries_raw = NULL;
		intern->array.type_tag = LMV_TYPE_UNINITIALIZED;
	}

	return &intern->std;
}

static zend_object *teds_lowmemoryvector_new(zend_class_entry *class_type)
{
	return teds_lowmemoryvector_new_ex(class_type, NULL, 0);
}

static zend_object *teds_lowmemoryvector_clone(zend_object *old_object)
{
	zend_object *new_object = teds_lowmemoryvector_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_lowmemoryvector_count_elements(zend_object *object, zend_long *count)
{
	const teds_lowmemoryvector *intern = teds_lowmemoryvector_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this LowMemoryVector */
PHP_METHOD(Teds_LowMemoryVector, count)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS)->size);
}

/* Check if this LowMemoryVector is empty */
PHP_METHOD(Teds_LowMemoryVector, isEmpty)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_BOOL(!Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS)->size);
}

/* Get capacity of this lowmemoryvector */
PHP_METHOD(Teds_LowMemoryVector, capacity)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS)->capacity);
}

/* Create this from an optional iterable */
PHP_METHOD(Teds_LowMemoryVector, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(object);

	if (UNEXPECTED(!teds_lowmemoryvector_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\LowMemoryVector::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}
	if (!iterable) {
		teds_lowmemoryvector_entries_set_empty_list(array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_lowmemoryvector_entries_init_from_array_values(array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_lowmemoryvector_entries_init_from_traversable(array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

/* Clear this */
PHP_METHOD(Teds_LowMemoryVector, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_lowmemoryvector_entries_dtor(Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS));
}

PHP_METHOD(Teds_LowMemoryVector, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_lowmemoryvector_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_lowmemoryvector_it_rewind(zend_object_iterator *iter)
{
	((teds_lowmemoryvector_it*)iter)->current = 0;
}

static int teds_lowmemoryvector_it_valid(zend_object_iterator *iter)
{
	teds_lowmemoryvector_it *iterator = (teds_lowmemoryvector_it*)iter;

	return iterator->current < Z_LOWMEMORYVECTOR_ENTRIES_P(&iter->data)->size ? SUCCESS : FAILURE;
}


static zval *teds_lowmemoryvector_it_get_current_data(zend_object_iterator *iter)
{
	teds_lowmemoryvector_it *iterator = (teds_lowmemoryvector_it*)iter;
	teds_lowmemoryvector_entries *array   = Z_LOWMEMORYVECTOR_ENTRIES_P(&iter->data);
	if (UNEXPECTED(iterator->current >= array->size)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		return &EG(uninitialized_zval);
	}
	return teds_lowmemoryvector_entries_read_offset(array, iterator->current, &iterator->tmp);
}

static void teds_lowmemoryvector_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_lowmemoryvector_it     *iterator = (teds_lowmemoryvector_it*)iter;
	teds_lowmemoryvector *object   = Z_LOWMEMORYVECTOR_P(&iter->data);

	size_t offset = iterator->current;
	if (offset >= object->array.size) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, offset);
	}
}

static void teds_lowmemoryvector_it_move_forward(zend_object_iterator *iter)
{
	((teds_lowmemoryvector_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_lowmemoryvector_it_funcs = {
	teds_lowmemoryvector_it_dtor,
	teds_lowmemoryvector_it_valid,
	teds_lowmemoryvector_it_get_current_data,
	teds_lowmemoryvector_it_get_current_key,
	teds_lowmemoryvector_it_move_forward,
	teds_lowmemoryvector_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_lowmemoryvector_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	// This is final
	ZEND_ASSERT(ce == teds_ce_LowMemoryVector);
	teds_lowmemoryvector_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_lowmemoryvector_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_lowmemoryvector_it_funcs;

	return &iterator->intern;
}

static void teds_lowmemoryvector_entries_unserialize_from_mixed_zval_array(teds_lowmemoryvector_entries *array, HashTable *ht) {
	const uint32_t num_entries = zend_hash_num_elements(ht);
	zval *entries = emalloc(num_entries * sizeof(zval));
	zval *it = entries;
	zval *val;
	ZEND_HASH_FOREACH_VAL(ht, val) {
		ZVAL_COPY_DEREF(it, val);
		it++;
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it == entries + num_entries);
	array->size = num_entries;
	array->capacity = num_entries;
	array->entries_zval = entries;
}

static void teds_lowmemoryvector_entries_unserialize_from_bitset(teds_lowmemoryvector_entries *array, const uint8_t *bitset_val, const size_t bytes) {
	uint8_t wasted_bytes;
	/* overflow check */
	if (UNEXPECTED(bytes <= 1 || (wasted_bytes = ((uint8_t*)bitset_val)[bytes - 1]) >= 8) || bytes >= TEDS_MAX_ZVAL_COLLECTION_SIZE / 8 - 1) {
		zend_throw_exception(spl_ce_RuntimeException, "Unserializing from invalid bitset data", 0);
		return;
	}
	/* bytes > 0 */
	const uint32_t num_entries = (bytes - 1) * 8 - wasted_bytes;
	ZEND_ASSERT(num_entries <= TEDS_MAX_ZVAL_COLLECTION_SIZE);
	uint8_t *const entries = emalloc(num_entries);
	uint8_t *dst = entries;

	for (const uint8_t *const last = dst + num_entries; dst + 8 <= last; bitset_val++, dst += 8) {
		const uint8_t orig = *bitset_val;
		/* TODO sse may be faster? */
		dst[0] = IS_FALSE + ((orig >> 0) & 1);
		dst[1] = IS_FALSE + ((orig >> 1) & 1);
		dst[2] = IS_FALSE + ((orig >> 2) & 1);
		dst[3] = IS_FALSE + ((orig >> 3) & 1);
		dst[4] = IS_FALSE + ((orig >> 4) & 1);
		dst[5] = IS_FALSE + ((orig >> 5) & 1);
		dst[6] = IS_FALSE + ((orig >> 6) & 1);
		dst[7] = IS_FALSE + ((orig >> 7) & 1);
	}

	if (wasted_bytes > 0) {
		uint32_t remaining = 8 - wasted_bytes;
		ZEND_ASSERT(remaining > 0);
		/* There are 1 to 7 padding bits to decode */
		uint8_t val = *bitset_val;
		do {
			*dst++ = IS_FALSE + (val & 1);
			val >>= 1;
			remaining--;
		} while (remaining > 0);
	}
	//fprintf(stderr, "dst-entries=%d num_entries=%d\n", (int)(dst-entries), (int)num_entries);
	ZEND_ASSERT(dst == entries + num_entries);
	array->size = num_entries;
	array->capacity = num_entries;
	array->entries_uint8 = entries;
	array->type_tag = LMV_TYPE_BOOL_OR_NULL;
}

static zend_always_inline uint8_t teds_lowmemoryvector_entries_compute_type_from_bit_pair(const uint8_t input) {
	return input == 0 ? IS_NULL : (IS_NULL - 1) + input;
}

static void teds_lowmemoryvector_entries_unserialize_from_bool_or_null_set(teds_lowmemoryvector_entries *const array, const uint8_t *bitset_val, const size_t bytes) {
	uint8_t wasted_bit_pairs;
	/* overflow check */
	if (UNEXPECTED(bytes <= 1 || (wasted_bit_pairs = ((uint8_t*)bitset_val)[bytes - 1]) >= 4) || bytes >= TEDS_MAX_ZVAL_COLLECTION_SIZE / 4 - 1) {
		zend_throw_exception(spl_ce_RuntimeException, "Unserializing from invalid nullable boolset data", 0);
		return;
	}
	/* bytes > 0 */
	const uint32_t num_entries = (bytes - 1) * 4 - wasted_bit_pairs;
	uint8_t *const entries = emalloc(num_entries);
	uint8_t *dst = entries;

	for (const uint8_t *const dst_end = dst + num_entries; dst + 4 <= dst_end; bitset_val++, dst += 4) {
		const uint8_t orig = *bitset_val;
		dst[0] = teds_lowmemoryvector_entries_compute_type_from_bit_pair((orig >> 0) & 3);
		dst[1] = teds_lowmemoryvector_entries_compute_type_from_bit_pair((orig >> 2) & 3);
		dst[2] = teds_lowmemoryvector_entries_compute_type_from_bit_pair((orig >> 4) & 3);
		dst[3] = teds_lowmemoryvector_entries_compute_type_from_bit_pair((orig >> 6) & 3);
	}
	if (wasted_bit_pairs > 0) {
		uint8_t remaining = 4 - wasted_bit_pairs;
		ZEND_ASSERT(remaining > 0 && remaining < 4);
		/* There are 1 to 3 padding bit pairs to decode */
		uint8_t val = *bitset_val;
		do {
			uint8_t tmp = val & 3;
			//fprintf(stderr, "tmp=%d bitset-val=%d\n", (int)tmp, (int)*bitset_val);
			*dst++ = teds_lowmemoryvector_entries_compute_type_from_bit_pair(tmp);
			val >>= 2;
			remaining--;
		} while (remaining > 0);
	}
	//fprintf(stderr, "dst-entries=%d num_entries=%d\n", (int)(dst-entries), num_entries);

	ZEND_ASSERT(dst == entries + num_entries);
	array->size = num_entries;
	array->capacity = num_entries;
	array->entries_uint8 = entries;
	array->type_tag = LMV_TYPE_BOOL_OR_NULL;
}

PHP_METHOD(Teds_LowMemoryVector, __unserialize)
{
	HashTable *raw_data;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}
	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!teds_lowmemoryvector_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	if (zend_hash_num_elements(raw_data) == 0) {
		teds_lowmemoryvector_entries_set_empty_list(array);
		return;
	}

	if (UNEXPECTED(zend_hash_num_elements(raw_data) != 2)) {
		zend_throw_exception(spl_ce_RuntimeException, "LowMemoryVector unexpected __unserialize data: expected exactly 2 values", 0);
		RETURN_THROWS();
	}
	const zval *type_tag_zval = zend_hash_index_find(raw_data, 0);
	if (UNEXPECTED(type_tag_zval == NULL || Z_TYPE_P(type_tag_zval) != IS_LONG)) {
		zend_throw_exception(spl_ce_RuntimeException, "LowMemoryVector unserialize got invalid type tag, expected int", 0);
		RETURN_THROWS();
	}
	const zend_ulong type_tag = Z_LVAL_P(type_tag_zval);
	if (UNEXPECTED(type_tag >= LMV_TYPE_COUNT)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "LowMemoryVector unserialize got unsupported type tag %d", (int)type_tag);
		RETURN_THROWS();
	}
	const zval *raw_zval = zend_hash_index_find(raw_data, 1);
	if (UNEXPECTED(raw_zval == NULL)) {
		zend_throw_exception(spl_ce_RuntimeException, "LowMemoryVector missing data to unserialize", 0);
		RETURN_THROWS();
	}
	array->type_tag = type_tag;
	if (type_tag == LMV_TYPE_ZVAL) {
		if (UNEXPECTED(Z_TYPE_P(raw_zval) != IS_ARRAY)) {
			zend_throw_exception(spl_ce_RuntimeException, "LowMemoryVector expected array of values for mixed type representation", 0);
			RETURN_THROWS();
		}
		teds_lowmemoryvector_entries_unserialize_from_mixed_zval_array(array, Z_ARR_P(raw_zval));
		return;
	}
	if (Z_TYPE_P(raw_zval) != IS_STRING) {
		zend_throw_exception(spl_ce_RuntimeException, "LowMemoryVector expected string for binary data", 0);
		RETURN_THROWS();
	}
	const size_t str_byte_length = Z_STRLEN_P(raw_zval);
	const char *strval = Z_STRVAL_P(raw_zval);
	switch (type_tag) {
		case _LMV_TYPE_BOOL_BITSET:
			/* Unserialize bitset to Z_TYPE of IS_TRUE/IS_FALSE */
			teds_lowmemoryvector_entries_unserialize_from_bitset(array, (const uint8_t*)strval, str_byte_length);
			return;
		case LMV_TYPE_BOOL_OR_NULL:
			teds_lowmemoryvector_entries_unserialize_from_bool_or_null_set(array, (const uint8_t*)strval, str_byte_length);
			return;
		case LMV_TYPE_INT32 + 1: /* 32-bit check */
#ifndef LMV_TYPE_INT64
			zend_throw_exception(spl_ce_RuntimeException, "LowMemoryVector unserialize binary not implemented for serialized int64 on 32-bit builds", 0);
			RETURN_THROWS();
#endif
		case LMV_TYPE_DOUBLE:
		case LMV_TYPE_INT32:
		case LMV_TYPE_INT16:
		case LMV_TYPE_INT8: {
			uint8_t shift = teds_lmv_shift_for_element[type_tag];
			const size_t num_elements = str_byte_length >> shift;
			if (UNEXPECTED(num_elements >= TEDS_MAX_ZVAL_COLLECTION_SIZE)) {
				teds_error_noreturn_max_lowmemoryvector_capacity();
				ZEND_UNREACHABLE();
			}
			if (UNEXPECTED((num_elements << shift) != str_byte_length)) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0, "LowMemoryVector Unexpected binary length for type tag, expected multiple of 8 * 2**%d, got %d bytes", (int)shift, (int)str_byte_length);
				RETURN_THROWS();
			}
			char *const values = emalloc(str_byte_length);
			array->entries_int8 = (int8_t *)values;
			array->size = num_elements;
			array->capacity = num_elements;
#if WORDS_BIGENDIAN
			/* TODO: Can probably optimize this with C restrict keyword to indicate memory doesn't overlap? */
			if (type_tag != LMV_TYPE_INT8) {
				if (type_tag == LMV_TYPE_INT64 || type_tag == LMV_TYPE_DOUBLE) {
					uint64_t *dst = (uint64_t *)values;
					const uint64_t *src = (const uint64_t *)strval;
					const uint64_t *const end = src + num_elements;
					for (;src < end; src++, dst++) {
						/* This compiles down to a bswap assembly instruction in optimized builds */
						*dst = bswap_64(*src);
					}
				} else if (type_tag == LMV_TYPE_INT32) {
					uint32_t *dst = (uint32_t *)values;
					const uint32_t *src = (const uint32_t *)strval;
					const uint32_t *const end = src + num_elements;
					for (;src < end; src++, dst++) {
						/* This compiles down to a bswap assembly instruction in optimized builds */
						*dst = bswap_32(*src);
					}
				} else if (type_tag == LMV_TYPE_INT16) {
					uint16_t *dst = (uint16_t *)values;
					const uint16_t *src = (const uint16_t *)strval;
					const uint16_t *const end = src + num_elements;
					for (;src < end; src++, dst++) {
						/* This compiles down to a bswap assembly instruction in optimized builds */
						*dst = teds_bswap_16(*src);
					}
				} else {
					ZEND_UNREACHABLE();
				}
				return;
			}
#endif
			/* Little-endian and int8_t are just a memcpy */
			memcpy(values, strval, str_byte_length);
			break;
		}
		default:
			ZEND_UNREACHABLE();
	}
}

static zend_always_inline zend_string *teds_create_string_from_entries_int8(const char *raw, const size_t len) {
	return zend_string_init(raw, len, 0);
}

static zend_always_inline zend_string *teds_create_string_from_entries_int16(const char *raw, const size_t len) {
#if WORDS_BIGENDIAN
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
#if WORDS_BIGENDIAN
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
#if WORDS_BIGENDIAN
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

static zend_always_inline zend_string *teds_create_string_from_entries_double(const char *raw, const size_t len) {
	return teds_create_string_from_entries_int64(raw, len);
}

static zend_string *teds_convert_bool_types_to_bitset(const uint8_t *src, const size_t len)
{
	ZEND_ASSERT(len > 0);
	const size_t num_bytes_for_bits = ((len + 15) >> 3);
	const uint8_t wasted_bits = (8 - len) & 7;
	zend_string *const result = zend_string_alloc(num_bytes_for_bits, 0);
	uint8_t *dst = (uint8_t *)ZSTR_VAL(result);

	for (const uint8_t *const end = src + len; src + 8 <= end; dst++, src += 8) {
		*dst = ((src[0] == IS_TRUE ? 1 : 0) << 0)
		     + ((src[1] == IS_TRUE ? 1 : 0) << 1)
		     + ((src[2] == IS_TRUE ? 1 : 0) << 2)
		     + ((src[3] == IS_TRUE ? 1 : 0) << 3)
		     + ((src[4] == IS_TRUE ? 1 : 0) << 4)
		     + ((src[5] == IS_TRUE ? 1 : 0) << 5)
		     + ((src[6] == IS_TRUE ? 1 : 0) << 6)
		     + ((src[7] == IS_TRUE ? 1 : 0) << 7);
	}
	uint8_t remaining = len & 7;
	//fprintf(stderr, "In bitset len=%d remaining=%d num_bytes_for_bits=%d wasted_bits=%d\n", (int)len, (int)remaining, (int)num_bytes_for_bits, (int)wasted_bits);
	if (remaining > 0) {
		ZEND_ASSERT((size_t)(dst + 2 - (uint8_t *)ZSTR_VAL(result)) == num_bytes_for_bits);
		uint8_t v = 0;
		for (uint8_t shift = 0; remaining > 0; remaining--, shift++, src++) {
			v += ((*src == IS_TRUE ? 1 : 0) << shift);
		}
		//fprintf(stderr, "In bitset len=%d v=%d wasted_bits=%d\n", (int)len, (int)v, (int)wasted_bits);
		dst[0] = v; /* Last bits */
		dst[1] = wasted_bits; /* Number of wasted bit pairs */
		dst[2] = '\0'; /* Trailing null byte */
	} else {
		ZEND_ASSERT((size_t)(dst + 1 - (uint8_t *)ZSTR_VAL(result)) == num_bytes_for_bits);
		dst[0] = '\0'; /* Number of wasted bit pairs */
		dst[1] = '\0'; /* Trailing null byte */
	}

	return result;
}

static zend_string *teds_convert_bool_or_null_types_to_bitset(const uint8_t *src, const uint32_t len)
{
	/*
	 * Layout:
	 * 1. ciel(len/4) bytes of bit pairs (4 bit pairs per byte)
	 * 2. The number of unused bit pairs - (4 - len) & 3, i.e. ((ciel(len/4) * 4) - len)
	 * 3. Trailing null byte required by zend_string after end.
	 */
	ZEND_ASSERT(len > 0);
	const uint32_t num_bytes_for_bit_pairs = ((len + 7) >> 2); /* 4 bit pairs per byte, plus an extra byte. ciel(len/4) + 1 */
	zend_string *const result = zend_string_alloc(num_bytes_for_bit_pairs, 0);
	uint8_t *dst = (uint8_t *)ZSTR_VAL(result);

	for (const uint8_t *const end = src + len; src + 4 <= end; dst++, src += 4) {
		*dst = (((src[0] - IS_NULL + 1) & 3) << 0) |
		       (((src[1] - IS_NULL + 1) & 3) << 2) |
		       (((src[2] - IS_NULL + 1) & 3) << 4) |
		       (((src[3] - IS_NULL + 1) & 3) << 6);
	}
	uint8_t remaining = len & 3;
	if (remaining > 0) {
		uint8_t v = 0;
		for (uint8_t shift = 0; remaining > 0; remaining--, shift += 2, src++) {
			uint8_t value = (*src - IS_NULL + 1) & 3;
			v += value << shift;
		}
		dst[0] = v; /* Last bit pairs */
		dst[1] = (4 - len) & 3; /* Number of wasted bit pairs */
		dst[2] = '\0'; /* Trailing null byte */
		ZEND_ASSERT((size_t)(dst + 2 - (uint8_t *)ZSTR_VAL(result)) == num_bytes_for_bit_pairs);
	} else {
		dst[0] = '\0'; /* Number of wasted bit pairs */
		dst[1] = '\0'; /* Trailing null byte */
		ZEND_ASSERT((size_t)(dst + 1 - (uint8_t *)ZSTR_VAL(result)) == num_bytes_for_bit_pairs);
	}

	return result;
}

PHP_METHOD(Teds_LowMemoryVector, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_lowmemoryvector_entries *const array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		RETURN_EMPTY_ARRAY();
	}
	zval tmp;
	uint8_t type_tag = array->type_tag;
	switch (type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: {
			const uint8_t *src = array->entries_uint8;
#if ZEND_DEBUG
			for (const uint8_t *it = src, *end = it + len; it < end; it++) {
				teds_lowmemoryvector_entries_validate_type(*it);
			}
#endif
			if (memchr(src, IS_NULL, len) != NULL) {
				ZVAL_STR(&tmp, teds_convert_bool_or_null_types_to_bitset(src, len));
			} else {
				type_tag = _LMV_TYPE_BOOL_BITSET;
				ZVAL_STR(&tmp, teds_convert_bool_types_to_bitset(src, len));
			}
			break;
		}

#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
		case LMV_TYPE_X: \
			ZVAL_STR(&tmp, teds_create_string_from_##entries_intx((const char *)array->entries_int8, len)); \
			break;
LMV_GENERATE_INT_CASES()
#undef LMV_INT_CODEGEN

		case LMV_TYPE_DOUBLE:
			ZVAL_STR(&tmp, teds_create_string_from_entries_double((const char *)array->entries_double, len));
			break;

		case LMV_TYPE_ZVAL: {
			zend_array *values = teds_new_array_check_overflow(len);
			/* Initialize return array */
			zend_hash_real_init_packed(values);
			ZEND_HASH_FILL_PACKED(values) {
				zval *entries = array->entries_zval;
				for (uint32_t i = 0; i < len; i++) {
					zval *tmp = &entries[i];
					Z_TRY_ADDREF_P(tmp);
					ZEND_HASH_FILL_ADD(tmp);
				}
			} ZEND_HASH_FILL_END();
			ZVAL_ARR(&tmp, values);
			break;
		}
		default:
			ZEND_UNREACHABLE();
	}
	RETURN_ARR(teds_create_serialize_pair(type_tag, &tmp));
}

static void teds_lowmemoryvector_entries_init_from_array_values(teds_lowmemoryvector_entries *array, zend_array *raw_data)
{
	const uint32_t num_entries = zend_hash_num_elements(raw_data);
	teds_lowmemoryvector_entries_set_empty_list(array);
	if (num_entries == 0) {
		return;
	}
	/* TODO: Can probably precompute capacity to avoid reallocations in common case where type tag doesn't change */
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		teds_lowmemoryvector_entries_push(array, val, 1);
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Teds_LowMemoryVector, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_lowmemoryvector_new(teds_ce_LowMemoryVector);
	teds_lowmemoryvector *intern = teds_lowmemoryvector_from_object(object);
	teds_lowmemoryvector_entries_init_from_array_values(&intern->array, array_ht);

	RETURN_OBJ(object);
}

static zend_array *teds_lowmemoryvector_entries_to_refcounted_array(const teds_lowmemoryvector_entries *array) {
	const uint32_t len = array->size;
	zend_array *values = teds_new_array_check_overflow(array->size);
	/* Postcondition: len < HT_MAX_SIZE */
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		switch (array->type_tag) {
			case LMV_TYPE_BOOL_OR_NULL: {
				const uint8_t *const entries = array->entries_uint8;
				for (uint32_t i = 0; i < len; i++) {
					zval tmp;
					teds_lowmemoryvector_entries_set_type(&tmp, entries[i]);
					ZEND_HASH_FILL_SET(&tmp);
					ZEND_HASH_FILL_NEXT();
				}
				break;
			}
#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
			case LMV_TYPE_X: { \
				const intx_t *const entries = array->entries_intx; \
				for (uint32_t i = 0; i < len; i++) { \
					ZEND_HASH_FILL_SET_LONG(entries[i]); \
					ZEND_HASH_FILL_NEXT(); \
				} \
				break; \
			}
LMV_GENERATE_INT_CASES()
#undef LMV_INT_CODEGEN
			case LMV_TYPE_DOUBLE: {
				const double *const entries = array->entries_double;
				for (uint32_t i = 0; i < len; i++) {
					ZEND_HASH_FILL_SET_DOUBLE(entries[i]);
					ZEND_HASH_FILL_NEXT();
				}
				break;
			}

			case LMV_TYPE_ZVAL: {
				zval *entries = array->entries_zval;
				for (uint32_t i = 0; i < len; i++) {
					zval *tmp = &entries[i];
					Z_TRY_ADDREF_P(tmp);
					ZEND_HASH_FILL_ADD(tmp);
				}
				break;
			}
			default:
				ZEND_UNREACHABLE();
		}
	} ZEND_HASH_FILL_END();
	return values;
}

PHP_METHOD(Teds_LowMemoryVector, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	const teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	if (!array->size) {
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_lowmemoryvector_entries_to_refcounted_array(array));
}

static zend_always_inline void teds_lowmemoryvector_get_value_at_offset(zval *return_value, const zval *zval_this, zend_long offset)
{
	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(zval_this);
	if (UNEXPECTED(!teds_offset_within_size_t(offset, array->size))) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		RETURN_THROWS();
	}
	teds_lowmemoryvector_entries_copy_offset(array, offset, return_value, true, false);
}

PHP_METHOD(Teds_LowMemoryVector, get)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_lowmemoryvector_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_LowMemoryVector, offsetGet)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	teds_lowmemoryvector_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_LowMemoryVector, offsetExists)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	const teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	if ((zend_ulong) offset >= array->size) {
		RETURN_FALSE;
	}
	ZEND_ASSERT(offset >= 0);
	zval tmp;
	teds_lowmemoryvector_get_value_at_offset(&tmp, ZEND_THIS, offset);
	RETURN_BOOL(Z_TYPE(tmp) != IS_NULL);
}

PHP_METHOD(Teds_LowMemoryVector, containsKey)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset_zv) != IS_LONG) {
		RETURN_FALSE;
	}
	const zend_long offset = Z_LVAL_P(offset_zv);

	const teds_lowmemoryvector *intern = Z_LOWMEMORYVECTOR_P(ZEND_THIS);
	RETURN_BOOL(teds_offset_within_size_t(offset, intern->array.size));
}

PHP_METHOD(Teds_LowMemoryVector, indexOf)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		RETURN_NULL();
	}
	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: {
			const uint8_t type = Z_TYPE_P(value);
			if (type > IS_TRUE) {
				RETURN_NULL();
			}
			const uint8_t *start = array->entries_uint8;
			const uint8_t *offset = memchr(start, type, len);

			if (offset) {
				RETURN_LONG(offset - start);
			}
			break;
		}
		case LMV_TYPE_INT8: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_NULL();
			}
			const int8_t v = (int8_t) Z_LVAL_P(value);
			if (v != Z_LVAL_P(value)) { RETURN_NULL(); }
			const uint8_t *start = array->entries_uint8;
			const uint8_t *offset = memchr(start, (uint8_t)v, len);
			if (offset) {
				RETURN_LONG(offset - start);
			}
			break;
		}
		case LMV_TYPE_INT16: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_NULL();
			}
			const int16_t v = (int16_t) Z_LVAL_P(value);
			if (v != Z_LVAL_P(value)) { RETURN_NULL(); }
			const int16_t *start = array->entries_int16;
			const int16_t *it = start;
			for (const int16_t *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_LONG(it - start);
				}
			}
			break;
		}
		case LMV_TYPE_INT32: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_NULL();
			}
			const int32_t v = (int32_t) Z_LVAL_P(value);
#if SIZEOF_ZEND_LONG > 4
			if (v != Z_LVAL_P(value)) { RETURN_NULL(); }
#endif
			const int32_t *start = array->entries_int32;
			const int32_t *it = start;
			for (const int32_t *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_LONG(it - start);
				}
			}
			break;
		}
#ifdef LMV_TYPE_INT64
		case LMV_TYPE_INT64: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_NULL();
			}
			const zend_long v = Z_LVAL_P(value);
			const int64_t *start = array->entries_int64;
			const int64_t *it = start;
			for (const int64_t *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_LONG(it - start);
				}
			}
			break;
		}
#endif
		case LMV_TYPE_DOUBLE: {
			if (Z_TYPE_P(value) != IS_DOUBLE) {
				RETURN_NULL();
			}
			const double v = Z_DVAL_P(value);
			const double *start = array->entries_double;
			const double *it = start;
			for (const double *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_LONG(it - start);
				}
			}
			break;
		}
		case LMV_TYPE_ZVAL:
			teds_zval_range_zval_indexof(return_value, value, array->entries_zval, len);
			return;
		default:
			ZEND_UNREACHABLE();
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_LowMemoryVector, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	const uint32_t len = array->size;
	if (len == 0) {
		RETURN_FALSE;
	}
	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: {
			const uint8_t type = Z_TYPE_P(value);
			if (type <= IS_TRUE) {
				const uint8_t *start = array->entries_uint8;
				RETURN_BOOL(memchr((uint8_t *)start, type, len));
			}
			break;
		}
		case LMV_TYPE_INT8: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_FALSE;
			}
			const int8_t v = (int8_t) Z_LVAL_P(value);
			if (v != Z_LVAL_P(value)) { RETURN_FALSE; }
			const int8_t *start = array->entries_int8;
			RETURN_BOOL(memchr((uint8_t *)start, (uint8_t)v, len));
		}
		case LMV_TYPE_INT16: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_FALSE;
			}
			const int16_t v = (int16_t) Z_LVAL_P(value);
			if (v != Z_LVAL_P(value)) { RETURN_FALSE; }
			const int16_t *start = array->entries_int16;
			const int16_t *it = start;
			for (const int16_t *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_TRUE;
				}
			}
			break;
		}
		case LMV_TYPE_INT32: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_FALSE;
			}
			const int32_t v = (int32_t) Z_LVAL_P(value);
#if SIZEOF_ZEND_LONG > 4
			if (v != Z_LVAL_P(value)) { RETURN_FALSE; }
#endif
			const int32_t *start = array->entries_int32;
			const int32_t *it = start;
			for (const int32_t *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_TRUE;
				}
			}
			break;
		}
#ifdef LMV_TYPE_INT64
		case LMV_TYPE_INT64: {
			if (Z_TYPE_P(value) != IS_LONG) {
				RETURN_FALSE;
			}
			const zend_long v = Z_LVAL_P(value);
			const int64_t *it = array->entries_int64;
			for (const int64_t *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_TRUE;
				}
			}
			break;
		}
#endif
		case LMV_TYPE_DOUBLE: {
			if (Z_TYPE_P(value) != IS_DOUBLE) {
				RETURN_FALSE;
			}
			const double v = Z_DVAL_P(value);
			const double *it = array->entries_double;
			for (const double *end = it + len; it < end; it++) {
				if (v == *it) {
					RETURN_TRUE;
				}
			}
			break;
		}
		case LMV_TYPE_ZVAL:
			RETURN_BOOL(teds_zval_range_contains(value, array->entries_zval, len));
		default:
			ZEND_UNREACHABLE();
	}
	RETURN_FALSE;
}

static zend_always_inline void teds_lowmemoryvector_entries_set_value_at_offset(teds_lowmemoryvector_entries *array, zend_long offset, const zval *value, bool is_update) {
	if (is_update) {
		teds_lowmemoryvector_entries_update_type_tag(array, value);
	}

	if (UNEXPECTED(!teds_offset_within_size_t(offset, array->size))) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		return;
	}
	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: {
			array->entries_uint8[offset] = teds_lowmemoryvector_entries_get_type(value);
			break;
		}
#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
		case LMV_TYPE_X: \
			ZEND_ASSERT(Z_TYPE_P(value) == IS_LONG); \
			array->entries_intx[offset] = Z_LVAL_P(value); \
			break;
LMV_GENERATE_INT_CASES()
#undef LMV_INT_CODEGEN

		case LMV_TYPE_DOUBLE:
			ZEND_ASSERT(Z_TYPE_P(value) == IS_DOUBLE);
			array->entries_double[offset] = Z_DVAL_P(value); \
			break;
		case LMV_TYPE_ZVAL: {
			zval *dst = &array->entries_zval[offset];
			if (is_update) {
				zval tmp;
				ZVAL_COPY_VALUE(&tmp, dst);
				ZVAL_COPY(dst, value);
				/* Garbage collect original value after modifying to avoid writing to invalid location due to side effects of destructor */
				zval_ptr_dtor(&tmp);
			} else {
				ZVAL_COPY(dst, value);
			}
			break;
		}
		default:
			ZEND_UNREACHABLE();
	}
}

PHP_METHOD(Teds_LowMemoryVector, set)
{
	zend_long offset;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_lowmemoryvector_entries_set_value_at_offset(Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS), offset, value, true);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_LowMemoryVector, offsetSet)
{
	zval                  *offset_zv, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(offset_zv)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	teds_lowmemoryvector_entries_set_value_at_offset(Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS), offset, value, true);
	TEDS_RETURN_VOID();
}

static zend_always_inline void teds_lowmemoryvector_entries_copy_offset(const teds_lowmemoryvector_entries *array, const size_t offset, zval *dst, const bool increase_refcount, const bool remove)
{
	ZEND_ASSERT(remove ? array->size < array->capacity : array->size <= array->capacity);
	ZEND_ASSERT(remove ? offset <= array->size : offset < array->size);
	ZEND_ASSERT(dst != NULL);

	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: teds_lowmemoryvector_entries_set_type(dst, array->entries_uint8[offset]); return;

#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
		case LMV_TYPE_X: ZVAL_LONG(dst, array->entries_intx[offset]); return;
LMV_GENERATE_INT_CASES();
#undef LMV_INT_CODEGEN
		case LMV_TYPE_DOUBLE:
			ZVAL_DOUBLE(dst, array->entries_double[offset]);
			return;
		case LMV_TYPE_ZVAL:
			if (increase_refcount) {
				ZVAL_COPY(dst, &array->entries_zval[offset]);
			} else {
				ZVAL_COPY_VALUE(dst, &array->entries_zval[offset]);
			}
			return;
		default:
			ZEND_UNREACHABLE();
	}
}

static zend_always_inline zval *teds_lowmemoryvector_entries_read_offset(const teds_lowmemoryvector_entries *const array, const zend_ulong offset, zval *const tmp)
{
	ZEND_ASSERT(array->size <= array->capacity);
	ZEND_ASSERT(offset < array->size);
	ZEND_ASSERT(tmp != NULL);

	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: teds_lowmemoryvector_entries_set_type(tmp, array->entries_uint8[offset]); return tmp;

#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
		case LMV_TYPE_X: ZVAL_LONG(tmp, array->entries_intx[offset]); return tmp;
LMV_GENERATE_INT_CASES();
#undef LMV_INT_CODEGEN

		case LMV_TYPE_DOUBLE:
			ZVAL_DOUBLE(tmp, array->entries_double[offset]);
			return tmp;
		case LMV_TYPE_ZVAL:
			return &array->entries_zval[offset];
		default:
			ZEND_UNREACHABLE();
	}
}

static zend_always_inline void teds_lowmemoryvector_entries_update_type_tag(teds_lowmemoryvector_entries *array, const zval *val)
{

#define TEDS_PROMOTE_INT_TYPE_AND_RETURN(int_smaller, entries_smaller, int_larger, entries_larger, LMV_TYPE_LARGER) do { \
		ZEND_ASSERT(sizeof(int_smaller) < sizeof(int_larger)); \
		array->type_tag = LMV_TYPE_LARGER; \
		int_smaller *const original_entries = array->entries_smaller; \
		const int_smaller *src = original_entries; \
		const uint32_t size = array->size; \
		const uint32_t old_capacity = array->capacity; \
		const uint32_t capacity = old_capacity >= 4 ? old_capacity : 4; \
		int_larger *const entries_larger = safe_emalloc(capacity, sizeof(int_larger), 0); \
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

#define TEDS_RETURN_IF_LVAL_FITS_IN_TYPE(intx_t) do {  \
		if (EXPECTED(Z_LVAL_P(val) == (intx_t) Z_LVAL_P(val))) { return; } \
	} while (0)

#define TEDS_CHECK_PROMOTE_INT_TO_ZVAL_AND_RETURN(intx_t, entries_intx) do {\
		if (UNEXPECTED(Z_TYPE_P(val) != IS_LONG)) { \
			array->type_tag = LMV_TYPE_ZVAL; \
			intx_t *const original_entries = array->entries_intx; \
			const intx_t *src = original_entries; \
			const uint32_t size = array->size; \
			const uint32_t old_capacity = array->capacity; \
			const uint32_t capacity = old_capacity >= 4 ? old_capacity : 4; \
			zval *const entries_zval = safe_emalloc(capacity, sizeof(zval), 0); \
			if (old_capacity > 0) { \
				const zval *const end = entries_zval + size; \
				zval *dst = entries_zval; \
				array->entries_zval = entries_zval; \
				for (; dst < end; dst++, src++) { \
					ZVAL_LONG(dst, *src); \
				} \
				efree(original_entries); \
			} \
			return; \
		} \
	} while (0)


	ZEND_ASSERT(Z_TYPE_P(val) >= IS_NULL && Z_TYPE_P(val) <= IS_RESOURCE);
	switch (array->type_tag) {
		case LMV_TYPE_UNINITIALIZED:
			ZEND_ASSERT(teds_lowmemoryvector_entries_empty_capacity(array));
			switch (Z_TYPE_P(val)) {
				case IS_NULL:
				case IS_FALSE:
				case IS_TRUE:
					array->type_tag = LMV_TYPE_BOOL_OR_NULL;
					return;
				case IS_LONG: {
					if (Z_LVAL_P(val) != (int8_t)Z_LVAL_P(val)) {
#ifdef LMV_TYPE_INT64
						if (Z_LVAL_P(val) != (int32_t)Z_LVAL_P(val)) {
							array->type_tag = LMV_TYPE_INT64;
							return;
						}
#endif
						if (Z_LVAL_P(val) != (int16_t)Z_LVAL_P(val)) {
							array->type_tag = LMV_TYPE_INT32;
							return;
						}
						array->type_tag = LMV_TYPE_INT16;
						return;
					}
					array->type_tag = LMV_TYPE_INT8;
					return;
				}
				case IS_DOUBLE:
					array->type_tag = LMV_TYPE_DOUBLE;
					return;
				default:
					array->type_tag = LMV_TYPE_ZVAL;
					return;
			}
		case LMV_TYPE_BOOL_OR_NULL:
			if (UNEXPECTED(Z_TYPE_P(val) > IS_TRUE)) {
				array->type_tag = LMV_TYPE_ZVAL;
				uint8_t *const original_types = array->entries_uint8;
				const uint8_t *src = original_types;
				const size_t size = array->size;
				const size_t capacity = size >= 2 ? size * 2 : 4;
				array->capacity = capacity;
				zval *const entries_zval = safe_emalloc(capacity, sizeof(zval), 0);
				const zval *const end = entries_zval + size;
				zval *dst = entries_zval;
				array->entries_zval = entries_zval;
				for (; dst < end; dst++, src++) {
					teds_lowmemoryvector_entries_set_type(dst, *src);
				}
				if (array->capacity > 0) {
					efree(original_types);
				}
				return;
			}
			return;
		case LMV_TYPE_INT8:
			TEDS_CHECK_PROMOTE_INT_TO_ZVAL_AND_RETURN(int8_t, entries_int8);
			TEDS_RETURN_IF_LVAL_FITS_IN_TYPE(int8_t);

#ifdef LMV_TYPE_INT64
			if (Z_LVAL_P(val) != (int32_t) Z_LVAL_P(val)) {
				TEDS_PROMOTE_INT_TYPE_AND_RETURN(int8_t, entries_int8, int64_t, entries_int64, LMV_TYPE_INT64);
				return;
			}
#endif
			if (Z_LVAL_P(val) != (int16_t) Z_LVAL_P(val)) {
				TEDS_PROMOTE_INT_TYPE_AND_RETURN(int8_t, entries_int8, int32_t, entries_int32, LMV_TYPE_INT32);
				return;
			}
			TEDS_PROMOTE_INT_TYPE_AND_RETURN(int8_t, entries_int8, int16_t, entries_int16, LMV_TYPE_INT16);
			return;
		case LMV_TYPE_INT16:
			TEDS_CHECK_PROMOTE_INT_TO_ZVAL_AND_RETURN(int16_t, entries_int16);
			TEDS_RETURN_IF_LVAL_FITS_IN_TYPE(int16_t);

#ifdef LMV_TYPE_INT64
			if (Z_LVAL_P(val) != (int32_t) Z_LVAL_P(val)) {
				TEDS_PROMOTE_INT_TYPE_AND_RETURN(int16_t, entries_int16, int64_t, entries_int64, LMV_TYPE_INT64);
			}
#endif
			TEDS_PROMOTE_INT_TYPE_AND_RETURN(int16_t, entries_int16, int32_t, entries_int32, LMV_TYPE_INT32);
			return;
		case LMV_TYPE_INT32:
			TEDS_CHECK_PROMOTE_INT_TO_ZVAL_AND_RETURN(int32_t, entries_int32);

#ifdef LMV_TYPE_INT64
			if (UNEXPECTED(Z_LVAL_P(val) != (int32_t) Z_LVAL_P(val))) {
				TEDS_PROMOTE_INT_TYPE_AND_RETURN(int32_t, entries_int32, int64_t, entries_int64, LMV_TYPE_INT64);
			}
#endif
			return;
#ifdef LMV_TYPE_INT64
		case LMV_TYPE_INT64:
			TEDS_CHECK_PROMOTE_INT_TO_ZVAL_AND_RETURN(int64_t, entries_int64);
			return;
#endif

#undef TEDS_PROMOTE_INT_TYPE_AND_RETURN
		case LMV_TYPE_DOUBLE:
			if (UNEXPECTED(Z_TYPE_P(val) != IS_DOUBLE)) {
				array->type_tag = LMV_TYPE_ZVAL;
				double *const original_entries = array->entries_double;
				const double *src = original_entries;
				const size_t size = array->size;
				const size_t capacity = size >= 2 ? size * 2 : 4;
				array->capacity = capacity;
				zval *const entries_zval = safe_emalloc(capacity, sizeof(zval), 0);
				const zval *const end = entries_zval + size;
				zval *dst = entries_zval;
				array->entries_zval = entries_zval;
				for (; dst < end; dst++, src++) {
					ZVAL_DOUBLE(dst, *src);
				}
				if (array->capacity > 0) {
					efree(original_entries);
				}
			}
			return;
		case LMV_TYPE_ZVAL:
			/* This can hold any type */
			return;
		default:
			ZEND_UNREACHABLE();
	}
}

static zend_always_inline void teds_lowmemoryvector_entries_push(teds_lowmemoryvector_entries *array, const zval *value, const bool check_capacity)
{
	const size_t old_size = array->size;
	teds_lowmemoryvector_entries_update_type_tag(array, value);
	/* update_type_tag may raise the capacity */
	const size_t old_capacity = array->capacity;

	if (check_capacity) {
		if (old_size >= old_capacity) {
			/* The compiler will type check but eliminate dead code on platforms where size_t is 32 bits (4 bytes) */
			if (SIZEOF_SIZE_T < 8 && UNEXPECTED(old_size > SIZE_MAX / 4)) {
				teds_error_noreturn_max_lowmemoryvector_capacity();
				ZEND_UNREACHABLE();
			}
			ZEND_ASSERT(old_size == old_capacity);
			teds_lowmemoryvector_entries_raise_capacity(array, old_size > 2 ? old_size * 2 : 4);
		}
	} else {
		ZEND_ASSERT(old_size < old_capacity);
	}
	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: \
			array->entries_uint8[old_size] = teds_lowmemoryvector_entries_get_type(value);
			break;
#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
		case LMV_TYPE_X: \
			ZEND_ASSERT(Z_TYPE_P(value) == IS_LONG); \
			array->entries_intx[old_size] = Z_LVAL_P(value); \
			break;
LMV_GENERATE_INT_CASES()
#undef LMV_INT_CODEGEN

		case LMV_TYPE_DOUBLE: \
			ZEND_ASSERT(Z_TYPE_P(value) == IS_DOUBLE); \
			array->entries_double[old_size] = Z_DVAL_P(value); \
			break;
		case LMV_TYPE_ZVAL:
			ZVAL_COPY(&array->entries_zval[old_size], value);
			break;
		default:
			ZEND_UNREACHABLE();
	}
	array->size++;
}

/* Based on array_push */
PHP_METHOD(Teds_LowMemoryVector, push)
{
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(argc == 0)) {
		return;
	}
	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);

	for (uint32_t i = 0; i < argc; i++) {
		teds_lowmemoryvector_entries_push(array, &args[i], true);
	}
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_LowMemoryVector, unshift)
{
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(argc == 0)) {
		return;
	}
	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);

	for (uint32_t i = 0; i < argc && array->type_tag != LMV_TYPE_ZVAL; i++) {
		teds_lowmemoryvector_entries_update_type_tag(array, &args[i]);
	}
	const uint8_t memory_per_element = teds_lowmemoryvector_entries_compute_memory_per_element(array);
	const size_t old_size = array->size;
	const size_t new_size = ((size_t) array->size) + argc;
	if (new_size >= array->capacity) {
		teds_lowmemoryvector_entries_raise_capacity(array, new_size > 3 ? new_size + (new_size >> 1) : 4);
	}

	for (uint32_t i = 0; i < argc && array->type_tag != LMV_TYPE_ZVAL; i++) {
		teds_lowmemoryvector_entries_update_type_tag(array, &args[i]);
	}
	uint8_t *const raw_bytes = array->entries_uint8;
	memcpy(raw_bytes + argc * (size_t) memory_per_element, raw_bytes, memory_per_element * old_size);
	array->size = new_size;
	for (uint32_t i = 0; i < argc; i++) {
		teds_lowmemoryvector_entries_set_value_at_offset(array, argc - i - 1, &args[i], false);
	}

	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_LowMemoryVector, pop)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty Teds\\LowMemoryVector", 0);
		RETURN_THROWS();
	}
	const size_t old_capacity = array->capacity;
	array->size--;
	teds_lowmemoryvector_entries_copy_offset(array, array->size, return_value, false, true);
	if (old_size * 4 < old_capacity) {
		/* Shrink the storage if only a quarter of the capacity is used  */
		const size_t size = old_size - 1;
		const size_t capacity = size > 2 ? size * 2 : 4;
		if (capacity < old_capacity) {
			teds_lowmemoryvector_entries_shrink_capacity(array, size, capacity, array->entries_raw);
		}
	}
}

PHP_METHOD(Teds_LowMemoryVector, shift)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot shift from empty Teds\\LowMemoryVector", 0);
		RETURN_THROWS();
	}
	const size_t old_capacity = array->capacity;
	uint8_t *const entries_uint8 = array->entries_uint8;
	const uint8_t bytes_per_element = teds_lowmemoryvector_entries_compute_memory_per_element(array);
	teds_lowmemoryvector_entries_copy_offset(array, 0, return_value, false, true);
	array->size--;
	memmove(entries_uint8, entries_uint8 + bytes_per_element, bytes_per_element * (old_size - 1));
	if (old_size * 4 < old_capacity) {
		/* Shrink the storage if only a quarter of the capacity is used  */
		const size_t size = old_size - 1;
		const size_t capacity = size > 2 ? size * 2 : 4;
		if (capacity < old_capacity) {
			teds_lowmemoryvector_entries_shrink_capacity(array, size, capacity, entries_uint8);
		}
	}
}

ZEND_COLD PHP_METHOD(Teds_LowMemoryVector, offsetUnset)
{
	zval                  *offset_zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &offset_zv) == FAILURE) {
		RETURN_THROWS();
	}
	TEDS_THROW_UNSUPPORTEDOPERATIONEXCEPTION("Teds\\LowMemoryVector does not support offsetUnset - elements must be set to null or removed by resizing");
}

static void teds_lowmemoryvector_write_dimension(zend_object *object, zval *offset_zv, zval *value)
{
	teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(object)->array;
	if (!offset_zv) {
		teds_lowmemoryvector_entries_push(array, value, true);
		return;
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	if (offset < 0 || (zend_ulong) offset >= array->size) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index invalid or out of range", 0);
		return;
	}
	ZVAL_DEREF(value);
	teds_lowmemoryvector_entries_set_value_at_offset(array, offset, value, true);
}

static zval *teds_lowmemoryvector_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (UNEXPECTED(!offset_zv || Z_ISUNDEF_P(offset_zv))) {
		return &EG(uninitialized_zval);
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW_RETURN_NULLPTR(offset, offset_zv);

	const teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(object)->array;

	if (UNEXPECTED(offset < 0 || (zend_ulong) offset >= array->size)) {
		if (type != BP_VAR_IS) {
			zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		}
		return NULL;
	} else {
		return teds_lowmemoryvector_entries_read_offset(array, offset, rv);
	}
}

static int teds_lowmemoryvector_has_dimension(zend_object *object, zval *offset_zv, int check_empty)
{
	zend_long offset;
	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) {
		offset = teds_get_offset(offset_zv);
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
	} else {
		offset = Z_LVAL_P(offset_zv);
	}

	const teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(object)->array;

	if (UNEXPECTED(((zend_ulong) offset) >= array->size || offset < 0)) {
		return 0;
	}

	/* TODO can optimize based on type tag, probably not necessary */
	zval tmp;
	zval *val = teds_lowmemoryvector_entries_read_offset(array, offset, &tmp);
	if (check_empty) {
		return zend_is_true(val);
	}
	return Z_TYPE_P(val) != IS_NULL;
}

PHP_MINIT_FUNCTION(teds_lowmemoryvector)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_LowMemoryVector = register_class_Teds_LowMemoryVector(zend_ce_aggregate, teds_ce_Sequence, php_json_serializable_ce);
	teds_ce_LowMemoryVector->create_object = teds_lowmemoryvector_new;

	memcpy(&teds_handler_LowMemoryVector, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_LowMemoryVector.offset          = XtOffsetOf(teds_lowmemoryvector, std);
	teds_handler_LowMemoryVector.clone_obj       = teds_lowmemoryvector_clone;
	teds_handler_LowMemoryVector.count_elements  = teds_lowmemoryvector_count_elements;
	teds_handler_LowMemoryVector.get_properties  = teds_lowmemoryvector_get_properties;
	teds_handler_LowMemoryVector.get_properties_for = teds_lowmemoryvector_get_properties_for;
	teds_handler_LowMemoryVector.get_gc          = teds_lowmemoryvector_get_gc;
	teds_handler_LowMemoryVector.free_obj        = teds_lowmemoryvector_free_storage;

	teds_handler_LowMemoryVector.read_dimension  = teds_lowmemoryvector_read_dimension;
	teds_handler_LowMemoryVector.write_dimension = teds_lowmemoryvector_write_dimension;
	teds_handler_LowMemoryVector.has_dimension   = teds_lowmemoryvector_has_dimension;

	teds_ce_LowMemoryVector->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_LowMemoryVector->get_iterator = teds_lowmemoryvector_get_iterator;

	return SUCCESS;
}
