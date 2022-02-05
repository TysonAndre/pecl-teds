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
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"

#include <stdbool.h>

/* Though rare, it is possible to have 64-bit zend_longs and a 32-bit size_t. */
#define MAX_ZVAL_COUNT ((SIZE_MAX / sizeof(zval)) - 1)
#define MAX_VALID_OFFSET ((size_t)(MAX_ZVAL_COUNT > ZEND_LONG_MAX ? ZEND_LONG_MAX : MAX_ZVAL_COUNT))

zend_object_handlers teds_handler_LowMemoryVector;
zend_class_entry *teds_ce_LowMemoryVector;

/* This is a placeholder value to distinguish between empty and uninitialized LowMemoryVector instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const zval empty_entry_list[1];

#define LMV_TYPE_UNINITIALIZED  0
#define LMV_TYPE_BOOL_OR_NULL   1
#define LMV_TYPE_INT8           2
#define LMV_TYPE_INT32          3
#if SIZEOF_ZEND_LONG > 4
#define LMV_TYPE_INT64          4
#define LMV_TYPE_LAST_NONREFCOUNTED LMV_TYPE_INT64
#else
#define LMV_TYPE_LAST_NONREFCOUNTED LMV_TYPE_INT32
#endif

#define LMV_TYPE_LAST_GC_NO_SIDE_EFFECTS LMV_TYPE_LAST_NONREFCOUNTED

#define LMV_TYPE_ZVAL           5
#define LMV_TYPE_COUNT          LMV_TYPE_ZVAL + 1

#ifdef LMV_TYPE_INT64
#if LMV_TYPE_LAST_NONREFCOUNTED + 1 != LMV_TYPE_ZVAL
#error need to update tables
#endif
#endif
static const uint8_t teds_lmv_memory_per_element[LMV_TYPE_ZVAL + 1] = {
	0,
	sizeof(int8_t),  /* LMV_TYPE_BOOL_OR_NULL */
	sizeof(int8_t),  /* LMV_TYPE_INT8 */
	sizeof(int32_t), /* LMV_TYPE_INT32 */
	sizeof(int64_t), /* LMV_TYPE_INT64 */
	sizeof(zval),    /* LMV_TYPE_ZVAL */
};

typedef struct _teds_lowmemoryvector_entries {
	size_t size;
	size_t capacity;
	union {
		uint8_t     *entries_uint8;
		int8_t      *entries_int8;
		int32_t     *entries_int32;
#ifdef LMV_TYPE_INT64
		int64_t     *entries_int64;
#endif
		/*double      *entries_double; */
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
static zend_always_inline void teds_lowmemoryvector_entries_copy_offset(const teds_lowmemoryvector_entries *array, const zend_ulong offset, zval *dst, bool increase_refcount, bool pop);
static zend_always_inline zval *teds_lowmemoryvector_entries_read_offset(const teds_lowmemoryvector_entries *intern, const zend_ulong offset, zval *tmp);
static void teds_lowmemoryvector_entries_init_from_array_values(teds_lowmemoryvector_entries *array, zend_array *raw_data);
static zend_array *teds_lowmemoryvector_entries_to_refcounted_array(const teds_lowmemoryvector_entries *array);

#ifdef LMV_TYPE_INT64
#define LMV_GENERATE_INT_CASES() \
	LMV_INT_CODEGEN(LMV_TYPE_INT8, int8_t,   entries_int8) \
	LMV_INT_CODEGEN(LMV_TYPE_INT32, int32_t, entries_int32) \
	LMV_INT_CODEGEN(LMV_TYPE_INT64, int64_t, entries_int64)
#else
#define LMV_GENERATE_INT_CASES() \
	LMV_INT_CODEGEN(LMV_TYPE_INT8, int8_t,   entries_int8) \
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

static inline void teds_lowmemoryvector_entries_shrink_capacity(teds_lowmemoryvector_entries *array, size_t size, size_t capacity, void *old_entries_raw) {
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

/* Initializes the range [from, to) to null. Does not dtor existing entries. */
/* TODO: Delete if this isn't used in the final version
static void teds_lowmemoryvector_entries_init_elems(teds_lowmemoryvector_entries *array, zend_long from, zend_long to)
{
	ZEND_ASSERT(from <= to);
	zval *begin = &array->entries[from];
	zval *end = &array->entries[to];

	while (begin != end) {
		ZVAL_NULL(begin++);
	}
}
*/

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
static void teds_lowmemoryvector_copy_range(teds_lowmemoryvector_entries *array, const int8_t *const start, const size_t n, const uint8_t bytes_per_element)
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
		zval *entries = array->entries_zval;
		if (array->type_tag > LMV_TYPE_LAST_NONREFCOUNTED) {
			ZEND_ASSERT(array->type_tag == LMV_TYPE_ZVAL);
			zval *it = entries;
			array->entries_zval = NULL;
			for (zval *end = it + array->size; it < end; it++) {
				zval_ptr_dtor(it);
			}
		}
		efree(entries);
	}
}

static HashTable* teds_lowmemoryvector_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(obj)->array;

	/* TODO: This won't work with zvals and sizeof size_t > sizeof int with 4 billion values*/
	size_t len = array->size;
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
	 */
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

/* Get number of entries in this lowmemoryvector */
PHP_METHOD(Teds_LowMemoryVector, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_lowmemoryvector *intern = Z_LOWMEMORYVECTOR_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get number of entries in this lowmemoryvector */
PHP_METHOD(Teds_LowMemoryVector, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_lowmemoryvector *intern = Z_LOWMEMORYVECTOR_P(object);
	RETURN_BOOL(intern->array.size == 0);
}

/* Get capacity of this lowmemoryvector */
PHP_METHOD(Teds_LowMemoryVector, capacity)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_lowmemoryvector *intern = Z_LOWMEMORYVECTOR_P(object);
	RETURN_LONG(intern->array.capacity);
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
	teds_lowmemoryvector_it     *iterator = (teds_lowmemoryvector_it*)iter;
	teds_lowmemoryvector *object   = Z_LOWMEMORYVECTOR_P(&iter->data);

	if (iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
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
	zend_throw_exception(spl_ce_RuntimeException, "LowMemoryVector __unserialize not yet implemented", 0);
}

static zend_always_inline HashTable *teds_create_serialize_pair(const uint8_t type, zval *data) {
	zval type_zv;
	ZVAL_LONG(&type_zv, type);
	return zend_new_pair(&type_zv, data);
}

static zend_always_inline zend_string *teds_create_string_from_entries_int8(const char *raw, const size_t len) {
	return zend_string_init(raw, len, 0);
}

/*
static zend_always_inline zend_string *teds_create_string_from_entries_int16(const char *raw, const size_t len) {
#if WORDS_BIGENDIAN
	zend_string *const result = zend_string_alloc(len * sizeof(int16_t), 0);
	uint16_t *dst = (uint16_t *)ZSTR_VAL(result);
	const uint16_t *src = (const uint16_t *)raw;
	const uint16_t *const end = src + len;
	for (;src < end; src++, dst++) {
		const uint16_t v = *src;
		*dst = (v >> 8) | (uint16_t)(v << 8);
	}
	*(char *)dst = '\0';
	return result;
#else
	return zend_string_init((const char*) array->entries_int8, len * sizeof(int16_t), 0);
#endif
}
*/

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

#ifdef LMV_TYPE_INT64
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
#endif

PHP_METHOD(Teds_LowMemoryVector, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_lowmemoryvector_entries *const array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t len = array->size;
	if (len == 0) {
		RETURN_EMPTY_ARRAY();
	}
	php_error_docref(NULL, E_WARNING, "LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented");
	zval tmp;
	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: {
			const uint8_t *src = array->entries_uint8;
#if ZEND_DEBUG
			for (size_t i = 0; i < len; i++) {
				teds_lowmemoryvector_entries_validate_type(src[i]);
			}
#endif
			ZVAL_STR(&tmp, zend_string_init((const char*) src, len, 0));
			RETURN_ARR(teds_create_serialize_pair(LMV_TYPE_BOOL_OR_NULL, &tmp));
		}

#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
		case LMV_TYPE_X: \
			ZVAL_STR(&tmp, teds_create_string_from_##entries_intx((const char *)array->entries_int8, len)); \
			RETURN_ARR(teds_create_serialize_pair(LMV_TYPE_X, &tmp));
LMV_GENERATE_INT_CASES()
#undef LMV_INT_CODEGEN

		case LMV_TYPE_ZVAL: {
			zend_array *values = zend_new_array(len);
			/* Initialize return array */
			zend_hash_real_init_packed(values);
			ZEND_HASH_FILL_PACKED(values) {
				zval *entries = array->entries_zval;
				for (size_t i = 0; i < len; i++) {
					zval *tmp = &entries[i];
					Z_TRY_ADDREF_P(tmp);
					ZEND_HASH_FILL_ADD(tmp);
				}
			} ZEND_HASH_FILL_END();
			zval tmp;
			ZVAL_ARR(&tmp, values);
			RETURN_ARR(teds_create_serialize_pair(LMV_TYPE_ZVAL, &tmp));
		}
		default:
			ZEND_UNREACHABLE();

	}
}

static void teds_lowmemoryvector_entries_init_from_array_values(teds_lowmemoryvector_entries *array, zend_array *raw_data)
{
	size_t num_entries = zend_hash_num_elements(raw_data);
	teds_lowmemoryvector_entries_set_empty_list(array);
	if (num_entries == 0) {
		return;
	}
	/* TODO: Can probably precompute capacity to avoid reallocations */
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
	size_t len = array->size;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		switch (array->type_tag) {
			case LMV_TYPE_BOOL_OR_NULL: {
				const uint8_t *const entries = array->entries_uint8;
				for (size_t i = 0; i < len; i++) {
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
				for (size_t i = 0; i < len; i++) { \
					ZEND_HASH_FILL_SET_LONG(entries[i]); \
					ZEND_HASH_FILL_NEXT(); \
				} \
				break; \
			}
LMV_GENERATE_INT_CASES()
#undef LMV_INT_CODEGEN

			case LMV_TYPE_ZVAL: {
				zval *entries = array->entries_zval;
				for (size_t i = 0; i < len; i++) {
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
	size_t len = array->size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_lowmemoryvector_entries_to_refcounted_array(array));
}

static zend_always_inline void teds_lowmemoryvector_get_value_at_offset(zval *return_value, const zval *zval_this, zend_long offset)
{
	teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(zval_this);
	size_t len = array->size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
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

	const teds_lowmemoryvector *intern = Z_LOWMEMORYVECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	RETURN_BOOL((zend_ulong) offset < len);
}

PHP_METHOD(Teds_LowMemoryVector, indexOf)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_lowmemoryvector_entries *array = Z_LOWMEMORYVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t len = array->size;
	if (len > 0) {
		switch (array->type_tag) {
			case LMV_TYPE_BOOL_OR_NULL: {
				const uint8_t type = Z_TYPE_P(value);
				if (type > IS_TRUE) {
					RETURN_NULL();
				}
				const uint8_t *start = array->entries_uint8;
				const uint8_t *it = start;
				for (const uint8_t *end = it + len; it < end; it++) {
					if (type == *it) {
						RETURN_LONG(it - start);
					}
				}
				break;
			}
			case LMV_TYPE_INT8: {
				if (Z_TYPE_P(value) != IS_LONG) {
					RETURN_NULL();
				}
				const int8_t v = (int8_t) Z_LVAL_P(value);
#if SIZEOF_ZEND_LONG > 4
				if (v != Z_LVAL_P(value)) { RETURN_NULL(); }
#endif
				const int8_t *start = array->entries_int8;
				const int8_t *it = start;
				for (const int8_t *end = it + len; it < end; it++) {
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
			case LMV_TYPE_ZVAL: {
				zval *start = array->entries_zval;
				zval *it = start;
				for (zval *end = it + len; it < end; it++) {
					if (zend_is_identical(value, it)) {
						RETURN_LONG(it - start);
					}
				}
				break;
			}
			default:
				ZEND_UNREACHABLE();
		}
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
	const size_t len = array->size;
	if (len > 0) {
		switch (array->type_tag) {
			case LMV_TYPE_BOOL_OR_NULL: {
				const uint8_t type = Z_TYPE_P(value);
				if (type > IS_TRUE) {
					RETURN_FALSE;
				}
				const uint8_t *start = array->entries_uint8;
				const uint8_t *it = start;
				for (const uint8_t *end = it + len; it < end; it++) {
					if (type == *it) {
						RETURN_TRUE;
					}
				}
				break;
			}
			case LMV_TYPE_INT8: {
				if (Z_TYPE_P(value) != IS_LONG) {
					RETURN_FALSE;
				}
				const int8_t v = (int8_t) Z_LVAL_P(value);
#if SIZEOF_ZEND_LONG > 4
				if (v != Z_LVAL_P(value)) { RETURN_NULL(); }
#endif
				const int8_t *start = array->entries_int8;
				const int8_t *it = start;
				for (const int8_t *end = it + len; it < end; it++) {
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
			case LMV_TYPE_ZVAL: {
				zval *it = array->entries_zval;
				for (zval *end = it + len; it < end; it++) {
					if (zend_is_identical(value, it)) {
						RETURN_TRUE;
					}
				}
				break;
			}
			default:
				ZEND_UNREACHABLE();
		}
	}
	RETURN_FALSE;
}

static zend_always_inline void teds_lowmemoryvector_set_value_at_offset(zend_object *object, zend_long offset, zval *value) {
	teds_lowmemoryvector_entries *array = &teds_lowmemoryvector_from_object(object)->array;
	teds_lowmemoryvector_entries_update_type_tag(array, value);
	size_t len = array->size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
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

		case LMV_TYPE_ZVAL: {
			zval tmp;
			zval *dst = &array->entries_zval[offset];
			ZVAL_COPY_VALUE(&tmp, dst);
			ZVAL_COPY(dst, value);
			/* Garbage collect original value after modifying to avoid writing to invalid location due to side effects of destructor */
			zval_ptr_dtor(&tmp);
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

	teds_lowmemoryvector_set_value_at_offset(Z_OBJ_P(ZEND_THIS), offset, value);
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

	teds_lowmemoryvector_set_value_at_offset(Z_OBJ_P(ZEND_THIS), offset, value);
	TEDS_RETURN_VOID();
}

static zend_always_inline void teds_lowmemoryvector_entries_copy_offset(const teds_lowmemoryvector_entries *array, const zend_ulong offset, zval *dst, bool increase_refcount, bool pop)
{
	ZEND_ASSERT(pop ? array->size < array->capacity : array->size <= array->capacity);
	ZEND_ASSERT(pop ? offset == array->size : offset < array->size);
	ZEND_ASSERT(dst != NULL);

	switch (array->type_tag) {
		case LMV_TYPE_BOOL_OR_NULL: teds_lowmemoryvector_entries_set_type(dst, array->entries_uint8[offset]); return;

#define LMV_INT_CODEGEN(LMV_TYPE_X, intx_t, entries_intx) \
		case LMV_TYPE_X: ZVAL_LONG(dst, array->entries_intx[offset]); return;
LMV_GENERATE_INT_CASES();
#undef LMV_INT_CODEGEN

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

		case LMV_TYPE_ZVAL:
			return &array->entries_zval[offset];
		default:
			ZEND_UNREACHABLE();
	}
}

static zend_always_inline void teds_lowmemoryvector_entries_update_type_tag(teds_lowmemoryvector_entries *array, const zval *val)
{

#define TEDS_PROMOTE_INT_TYPE(int_smaller, entries_smaller, int_larger, entries_larger, LMV_TYPE_LARGER) \
	array->type_tag = LMV_TYPE_LARGER; \
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
	}

#define TEDS_CHECK_PROMOTE_INT_TO_ZVAL(intx_t, entries_intx) \
	if (Z_TYPE_P(val) != IS_LONG) { \
		array->type_tag = LMV_TYPE_ZVAL; \
		intx_t *const original_entries = array->entries_intx; \
		const intx_t *src = original_entries; \
		const size_t size = array->size; \
		const size_t capacity = size >= 2 ? size * 2 : 4; \
		array->capacity = capacity; \
		zval *const entries_zval = safe_emalloc(capacity, sizeof(zval), 0); \
		const zval *const end = entries_zval + size; \
		zval *dst = entries_zval; \
		array->entries_zval = entries_zval; \
		for (; dst < end; dst++, src++) { \
			ZVAL_LONG(dst, *src); \
		} \
		if (array->capacity > 0) { \
			efree(original_entries); \
		} \
		return; \
	}


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
						array->type_tag = LMV_TYPE_INT32;
						return;
					}
					array->type_tag = LMV_TYPE_INT8;
					return;
				}
				default:
					array->type_tag = LMV_TYPE_ZVAL;
					return;
			}
		case LMV_TYPE_BOOL_OR_NULL:
			if (Z_TYPE_P(val) > IS_TRUE) {
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

			TEDS_CHECK_PROMOTE_INT_TO_ZVAL(int8_t, entries_int8)

			if (Z_LVAL_P(val) != (int8_t) Z_LVAL_P(val)) {
#ifdef LMV_TYPE_INT64
				if (Z_LVAL_P(val) != (int32_t) Z_LVAL_P(val)) {
					TEDS_PROMOTE_INT_TYPE(int8_t, entries_int8, int64_t, entries_int64, LMV_TYPE_INT64);
					return;
				}
#endif
				TEDS_PROMOTE_INT_TYPE(int8_t, entries_int8, int32_t, entries_int32, LMV_TYPE_INT32);
			}
			return;
		case LMV_TYPE_INT32:
			TEDS_CHECK_PROMOTE_INT_TO_ZVAL(int32_t, entries_int32)

#ifdef LMV_TYPE_INT64
			if (Z_LVAL_P(val) != (int32_t) Z_LVAL_P(val)) {
				TEDS_PROMOTE_INT_TYPE(int32_t, entries_int32, int64_t, entries_int64, LMV_TYPE_INT64);
			}
#endif
			return;
#ifdef LMV_TYPE_INT64
		case LMV_TYPE_INT64:
			TEDS_CHECK_PROMOTE_INT_TO_ZVAL(int64_t, entries_int64)
			return;
#endif

#undef TEDS_PROMOTE_INT_TYPE
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
	const size_t old_size = array->size;
	const size_t new_size = old_size + argc;
	/* The compiler will type check but eliminate dead code on platforms where size_t is 32 bits (4 bytes) */
	if (SIZEOF_SIZE_T < 8 && UNEXPECTED(new_size > MAX_VALID_OFFSET + 1 || new_size < old_size)) {
		teds_error_noreturn_max_lowmemoryvector_capacity();
		ZEND_UNREACHABLE();
	}

	for (uint32_t i = 0; i < argc; i++) {
		teds_lowmemoryvector_entries_push(array, &args[i], true);
	}
	array->size = new_size;
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

PHP_METHOD(Teds_LowMemoryVector, offsetUnset)
{
	zval                  *offset_zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &offset_zv) == FAILURE) {
		RETURN_THROWS();
	}
	zend_throw_exception(spl_ce_RuntimeException, "Teds\\LowMemoryVector does not support offsetUnset - elements must be set to null or removed by resizing", 0);
	RETURN_THROWS();
}

static void teds_lowmemoryvector_write_dimension(zend_object *object, zval *offset_zv, zval *value)
{
	teds_lowmemoryvector *intern = teds_lowmemoryvector_from_object(object);
	if (!offset_zv) {
		teds_lowmemoryvector_entries_push(&intern->array, value, true);
		return;
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	if (offset < 0 || (zend_ulong) offset >= intern->array.size) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index invalid or out of range", 0);
		return;
	}
	ZVAL_DEREF(value);
	teds_lowmemoryvector_set_value_at_offset(object, offset, value);
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

	/* TODO can optimize */
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
	teds_ce_LowMemoryVector = register_class_Teds_LowMemoryVector(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
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
