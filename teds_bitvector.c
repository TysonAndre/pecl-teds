/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c but has lower overhead (when count(bit_size) is known) and is more efficient to push and remove elements from the end of the list */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds.h"
#include "teds_bitvector_arginfo.h"
#include "teds_bitvector.h"
#include "teds_interfaces.h"
#include "teds_exceptions.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"
#include "teds_serialize_util.h"

#include <stdbool.h>

#define BYTE_OF_BIT_POSITION(n) ((n) >> 3)
#define BYTES_FOR_BIT_SIZE(n) BYTE_OF_BIT_POSITION((n) + 7)

/* zend_alloc.c allocations are at least 8 bytes. */
#define TEDS_MIN_BITVECTOR_BITS (8 * 8)
/* Avoid overflow */
#if SIZEOF_ZEND_LONG > SIZEOF_SIZE_T
#error expected SIZEOF_ZEND_LONG <= SIZEOF_SIZE_T
#endif

/* Returns a positive integer that is a multiple of TEDS_MIN_BITVECTOR_BITS */
static size_t teds_bitvector_compute_next_valid_capacity(size_t size) {
	return (size + TEDS_MIN_BITVECTOR_BITS) & ~(TEDS_MIN_BITVECTOR_BITS - 1);
}

/* Low enough to check for overflow */
#define TEDS_MAX_BITVECTOR_BITS (((size_t) 1) << (SIZEOF_ZEND_LONG * 8 - 2))

static zend_always_inline bool teds_bitvector_is_bool(const zval *offset) {
	//return ((uint8_t)(Z_TYPE_P(offset) - IS_FALSE)) <= 1;
	return EXPECTED(Z_TYPE_P(offset) == IS_FALSE || Z_TYPE_P(offset) == IS_TRUE);
}

#define TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(value, value_zv) do { \
	const zval *_tmp = (value_zv); \
	if (UNEXPECTED(!teds_bitvector_is_bool(_tmp))) { \
		if (EXPECTED(Z_TYPE_P(_tmp) == IS_REFERENCE)) { \
			_tmp = Z_REFVAL_P(_tmp); \
			if (EXPECTED(teds_bitvector_is_bool(_tmp))) { \
				(value) = Z_TYPE_P(_tmp) != IS_FALSE; \
				break; \
			} \
		} \
		zend_type_error("Illegal Teds\\BitVector value type %s", zend_zval_type_name(_tmp)); \
		return; \
	} else { \
		(value) = Z_TYPE_P(_tmp) != IS_FALSE; \
	} \
} while (0)


/* Though rare, it is possible to have 64-bit zend_longs and a 32-bit size_t. */
#define MAX_ZVAL_COUNT ((SIZE_MAX / sizeof(zval)) - 1)
#define MAX_VALID_OFFSET ((size_t)(MAX_ZVAL_COUNT > ZEND_LONG_MAX ? ZEND_LONG_MAX : MAX_ZVAL_COUNT))

zend_object_handlers teds_handler_BitVector;
zend_class_entry *teds_ce_BitVector;

typedef struct _teds_bitvector_entries {
	uint8_t *entries_bits;
	/* This is deliberately a size_t instead of an uint32_t.
	 * This is memory efficient enough that it's more likely to be used in practice for more than 4 billion values,
	 * and garbage collection isn't a problem.
	 * The memory usage in bytes is 1/8th of the bit_capacity (BYTE_OF_BIT_POSITION(bit_capacity)). */
	size_t bit_size;
	size_t bit_capacity;
} teds_bitvector_entries;

typedef struct _teds_bitvector {
	teds_bitvector_entries		array;
	zend_object				std;
} teds_bitvector;

/* Used by InternalIterator returned by BitVector->getIterator() */
typedef struct _teds_bitvector_it {
	zend_object_iterator intern;
	size_t               current;
	/* Temporary memory location to store the most recent get_current_value() result */
	zval                 tmp;
} teds_bitvector_it;

static void teds_bitvector_entries_raise_capacity(teds_bitvector_entries *intern, const size_t new_capacity);
static zend_always_inline void teds_bitvector_entries_push(teds_bitvector_entries *array, const bool value, const bool check_capacity);
static zend_always_inline void teds_bitvector_entries_copy_offset(const teds_bitvector_entries *array, const size_t offset, zval *dst, bool pop);
static zend_always_inline zval *teds_bitvector_entries_read_offset(const teds_bitvector_entries *intern, const size_t offset, zval *tmp);
static void teds_bitvector_entries_init_from_array_values(teds_bitvector_entries *array, zend_array *raw_data);
static zend_array *teds_bitvector_entries_to_refcounted_array(const teds_bitvector_entries *array);

/*
 * If a bit_size this large is encountered, assume the allocation will likely fail or
 * future changes to the bit_capacity will overflow.
 */
static ZEND_COLD void teds_error_noreturn_max_bitvector_capacity()
{
	zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\BitVector bit_capacity");
}

static zend_always_inline teds_bitvector *teds_bitvector_from_object(zend_object *obj)
{
	return (teds_bitvector*)((char*)(obj) - XtOffsetOf(teds_bitvector, std));
}

#define Z_BITVECTOR_P(zv)  (teds_bitvector_from_object(Z_OBJ_P((zv))))
#define Z_BITVECTOR_ENTRIES_P(zv)  (&(Z_BITVECTOR_P((zv))->array))

static zend_always_inline bool teds_bitvector_entries_empty_capacity(const teds_bitvector_entries *array)
{
	if (array->bit_capacity > 0) {
		ZEND_ASSERT(array->entries_bits != (void *)empty_entry_list);
		return false;
	}
	// This bitvector may have reserved bit_capacity.
	return true;
}

static zend_always_inline bool teds_bitvector_entries_uninitialized(const teds_bitvector_entries *array)
{
	if (array->entries_bits == NULL) {
		ZEND_ASSERT(array->bit_size == 0);
		ZEND_ASSERT(array->bit_capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries_bits == (void *)empty_entry_list && array->bit_capacity == 0) || array->bit_capacity > 0);
	return false;
}

static void teds_bitvector_entries_raise_capacity(teds_bitvector_entries *array, const size_t new_capacity) {
	ZEND_ASSERT((array->bit_capacity & 7) == 0);
	ZEND_ASSERT((new_capacity & (TEDS_MIN_BITVECTOR_BITS - 1)) == 0);
	ZEND_ASSERT(new_capacity > array->bit_capacity);
	const size_t bytes_needed = BYTES_FOR_BIT_SIZE(new_capacity);
	if (UNEXPECTED(new_capacity >= TEDS_MAX_BITVECTOR_BITS)) {
		teds_error_noreturn_max_bitvector_capacity();
		return;
	}
	//fprintf(stderr, "reallocate %d bytes %d bits\n", (int)bytes_needed, (int)new_capacity);
	if (array->bit_capacity == 0) {
		array->entries_bits = emalloc(bytes_needed);
	} else {
		array->entries_bits = erealloc(array->entries_bits, bytes_needed);
	}
	array->bit_capacity = new_capacity;
	ZEND_ASSERT(array->entries_bits != NULL);
}

static inline void teds_bitvector_entries_shrink_capacity(teds_bitvector_entries *array, const size_t bit_size, const size_t bit_capacity, void *old_entries_bits) {
	ZEND_ASSERT(bit_size <= bit_capacity);
	ZEND_ASSERT(bit_size == array->bit_size);
	ZEND_ASSERT(bit_capacity > 0);
	ZEND_ASSERT(bit_capacity < array->bit_capacity);
	ZEND_ASSERT(old_entries_bits == array->entries_bits);
	ZEND_ASSERT((bit_capacity & 7) == 0);
	array->bit_capacity = bit_capacity;
	array->entries_bits = erealloc2(old_entries_bits, bit_capacity >> 3, BYTES_FOR_BIT_SIZE(bit_size));
	ZEND_ASSERT(array->entries_bits != NULL);
}

static zend_always_inline void teds_bitvector_entries_set_empty_list(teds_bitvector_entries *array) {
	array->bit_size = 0;
	array->bit_capacity = 0;
	array->entries_bits = (void *)empty_entry_list;
}

static void teds_bitvector_entries_init_from_traversable(teds_bitvector_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_bitvector_entries_set_empty_list(array);
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

	/* Reindex keys from 0. */
	while (funcs->valid(iter) == SUCCESS) {
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		zval *value_zv = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		bool value;
		if (UNEXPECTED(!teds_bitvector_is_bool(value_zv))) {
			if (EXPECTED(Z_TYPE_P(value_zv) == IS_REFERENCE)) {
				value_zv = Z_REFVAL_P(value_zv);
				if (EXPECTED(teds_bitvector_is_bool(value_zv))) {
					value = Z_TYPE_P(value_zv) != IS_FALSE;
					break;
				}
			}
			zend_type_error("Illegal Teds\\BitVector value type %s", zend_zval_type_name(value_zv));
			break;
		} else {
			value = Z_TYPE_P(value_zv) != IS_FALSE;
		}

		teds_bitvector_entries_push(array, value, true);

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
}

static void teds_bitvector_entries_copy_ctor(teds_bitvector_entries *to, const teds_bitvector_entries *from)
{
	zend_long bit_size = from->bit_size;
	if (!bit_size) {
		teds_bitvector_entries_set_empty_list(to);
		return;
	}

	to->bit_size = 0; /* reset bit_size in case emalloc() fails */
	to->bit_capacity = 0;
	const size_t bytes_needed = BYTES_FOR_BIT_SIZE(bit_size);
	to->entries_bits = safe_emalloc(bit_size, bytes_needed, 0);
	to->bit_size = bit_size;
	to->bit_capacity = bytes_needed * 8;

	memcpy(to->entries_bits, from->entries_bits, BYTES_FOR_BIT_SIZE(bit_size));
}

static HashTable* teds_bitvector_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	(void)purpose;
	teds_bitvector_entries *array = &teds_bitvector_from_object(obj)->array;
	if (!array->bit_size) {
		/* debug_zval_dump DEBUG purpose requires null or a refcounted array. */
		return NULL;
	}
	/* var_export uses get_properties_for for infinite recursion detection rather than get_properties(Z_OBJPROP).
	 * or checking for recursion on the object itself (php_var_dump).
	 * However, BitVector can only contain integers, making infinite recursion impossible, so it's safe to return new arrays. */
	return teds_bitvector_entries_to_refcounted_array(array);
}

static void teds_bitvector_free_storage(zend_object *object)
{
	teds_bitvector *intern = teds_bitvector_from_object(object);
	if (!teds_bitvector_entries_empty_capacity(&intern->array)) {
		efree(intern->array.entries_bits);
	}
	zend_object_std_dtor(object);
}

static zend_object *teds_bitvector_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_bitvector *intern = zend_object_alloc(sizeof(teds_bitvector), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_BitVector);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_BitVector;

	if (orig && clone_orig) {
		teds_bitvector *other = teds_bitvector_from_object(orig);
		teds_bitvector_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries_bits = NULL;
	}

	return &intern->std;
}

static zend_object *teds_bitvector_new(zend_class_entry *class_type)
{
	return teds_bitvector_new_ex(class_type, NULL, 0);
}

static zend_object *teds_bitvector_clone(zend_object *old_object)
{
	zend_object *new_object = teds_bitvector_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_bitvector_count_elements(zend_object *object, zend_long *count)
{
	const teds_bitvector *intern = teds_bitvector_from_object(object);
	*count = intern->array.bit_size;
	return SUCCESS;
}

/* Get number of entries in this bitvector */
PHP_METHOD(Teds_BitVector, count)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(Z_BITVECTOR_ENTRIES_P(ZEND_THIS)->bit_size);
}

/* Get number of entries in this bitvector */
PHP_METHOD(Teds_BitVector, capacity)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(Z_BITVECTOR_ENTRIES_P(ZEND_THIS)->bit_capacity);
}

/* Check if the bitvector is empty. */
PHP_METHOD(Teds_BitVector, isEmpty)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_BOOL(Z_BITVECTOR_ENTRIES_P(ZEND_THIS)->bit_size == 0);
}

static void teds_bitvector_set_range(teds_bitvector_entries *const array, const size_t start_bit, const size_t end_bit, const bool value_bool) {
	ZEND_ASSERT(start_bit < end_bit);
	ZEND_ASSERT(end_bit < array->bit_capacity);
	uint8_t *const entries_bits = array->entries_bits;
	ZEND_ASSERT(entries_bits != NULL);

	const int8_t value = value_bool ? -1 : 0;
	const size_t start_byte_offset = (start_bit + 7) >> 3;
	const size_t end_byte_offset = (end_bit + 7) >> 3;

	const uint8_t start_mask = (1 << (start_bit & 7)) - 1;
	// fprintf(stderr, "start_bit=%d end_bit=%d start_byte_offset=%d end_byte_offset=%d value=%d start_mask=%d\n", (int)start_bit, (int)end_bit, (int)start_byte_offset, (int)end_byte_offset, (int)value, (int)start_mask);
	if (start_bit & 7) {
		/* Set/clear upper bits */
		if (value) {
			entries_bits[start_bit >> 3] |= 0xff ^ start_mask;
		} else {
			entries_bits[start_bit >> 3] &= start_mask;
		}
	}
	if (start_byte_offset < end_byte_offset) {
		memset(&entries_bits[start_byte_offset], value_bool ? -1 : 0, end_byte_offset - start_byte_offset);
	}
}

/* Set size in bits of this BitVector */
PHP_METHOD(Teds_BitVector, setSize)
{
	zend_long size_signed;
	bool default_bool = false;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(size_signed)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(default_bool)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(size_signed < 0)) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}
	const zend_ulong size = size_signed;
	teds_bitvector_entries *const array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	if (size <= old_size) {
		array->bit_size = size;
		const size_t capacity = teds_bitvector_compute_next_valid_capacity(size);
		if (UNEXPECTED(capacity < array->bit_capacity)) {
			teds_bitvector_entries_shrink_capacity(array, size, capacity, array->entries_bits);
		}
		TEDS_RETURN_VOID();
	}
	/* Raise the capacity as needed and fill the space with nulls */
	if (UNEXPECTED(size > array->bit_capacity)) {
		teds_bitvector_entries_raise_capacity(array, teds_bitvector_compute_next_valid_capacity(size + (size >> 2)));
	}
	teds_bitvector_set_range(array, old_size, size, default_bool);
	array->bit_size = size;
	TEDS_RETURN_VOID();
}

/* Create this from an optional iterable */
PHP_METHOD(Teds_BitVector, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(object);

	if (UNEXPECTED(!teds_bitvector_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\BitVector::__construct twice", 0);
		RETURN_THROWS();
	}
	if (!iterable) {
		teds_bitvector_entries_set_empty_list(array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_bitvector_entries_init_from_array_values(array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_bitvector_entries_init_from_traversable(array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_BitVector, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);

	if (!teds_bitvector_entries_empty_capacity(array)) {
		efree(array->entries_bits);
	}
	teds_bitvector_entries_set_empty_list(array);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_BitVector, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_bitvector_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_bitvector_it_rewind(zend_object_iterator *iter)
{
	((teds_bitvector_it*)iter)->current = 0;
}

static int teds_bitvector_it_valid(zend_object_iterator *iter)
{
	teds_bitvector_it     *iterator = (teds_bitvector_it*)iter;
	teds_bitvector *object   = Z_BITVECTOR_P(&iter->data);

	if (iterator->current < object->array.bit_size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval *teds_bitvector_it_get_current_data(zend_object_iterator *iter)
{
	teds_bitvector_it *iterator = (teds_bitvector_it*)iter;
	teds_bitvector_entries *array   = Z_BITVECTOR_ENTRIES_P(&iter->data);
	if (UNEXPECTED(iterator->current >= array->bit_size)) {
		teds_throw_invalid_sequence_index_exception();
		return &EG(uninitialized_zval);
	}
	return teds_bitvector_entries_read_offset(array, iterator->current, &iterator->tmp);
}

static void teds_bitvector_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_bitvector_it     *iterator = (teds_bitvector_it*)iter;
	teds_bitvector *object   = Z_BITVECTOR_P(&iter->data);

	size_t offset = iterator->current;
	if (offset >= object->array.bit_size) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, offset);
	}
}

static void teds_bitvector_it_move_forward(zend_object_iterator *iter)
{
	((teds_bitvector_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_bitvector_it_funcs = {
	teds_bitvector_it_dtor,
	teds_bitvector_it_valid,
	teds_bitvector_it_get_current_data,
	teds_bitvector_it_get_current_key,
	teds_bitvector_it_move_forward,
	teds_bitvector_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_bitvector_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	// This is final
	ZEND_ASSERT(ce == teds_ce_BitVector);
	teds_bitvector_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_bitvector_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_bitvector_it_funcs;

	return &iterator->intern;
}

static void teds_bitvector_initialize_from_bytes(teds_bitvector_entries *array, const uint8_t *bytes, const size_t byte_count, const uint8_t modulo_bit_len)
{
	const size_t bit_size = byte_count * 8 - modulo_bit_len;
	ZEND_ASSERT(bit_size > 0);
	const size_t bit_capacity = teds_bitvector_compute_next_valid_capacity(bit_size);
	// fprintf(stderr, "byte_count=%d modulo_bit_len=%d bit_size=%d bit_capacity=%d\n", (int)byte_count, (int)modulo_bit_len, (int)bit_size, (int)bit_capacity);
	uint8_t *const bytes_copy = emalloc(bit_capacity >> 3);
	memcpy(bytes_copy, bytes, byte_count);
	array->entries_bits = bytes_copy;
	array->bit_size = bit_size;
	array->bit_capacity = bit_capacity;
}

PHP_METHOD(Teds_BitVector, __unserialize)
{
	HashTable *raw_data;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}
	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!teds_bitvector_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Teds\\BitVector already unserialized", 0);
		RETURN_THROWS();
	}
	if (zend_hash_num_elements(raw_data) == 0) {
		teds_bitvector_entries_set_empty_list(array);
		return;
	}

	if (UNEXPECTED(zend_hash_num_elements(raw_data) != 1)) {
		zend_throw_exception(spl_ce_RuntimeException, "Teds\\BitVector unexpected __unserialize data: expected exactly 0 or 1 value", 0);
		RETURN_THROWS();
	}
	const zval *const raw_zval = zend_hash_index_find(raw_data, 0);
	if (UNEXPECTED(raw_zval == NULL)) {
		zend_throw_exception(spl_ce_RuntimeException, "Teds\\BitVector missing data to unserialize", 0);
		RETURN_THROWS();
	}
	if (Z_TYPE_P(raw_zval) != IS_STRING) {
		zend_throw_exception(spl_ce_RuntimeException, "Teds\\BitVector expected string for binary data", 0);
		RETURN_THROWS();
	}
	const zend_string *const zstr = Z_STR_P(raw_zval);
	if (ZSTR_LEN(zstr) == 0) {
		// Not expected but allowed
		teds_bitvector_entries_set_empty_list(array);
		return;
	}
	const uint8_t *bytes = (const uint8_t *)ZSTR_VAL(zstr);
	const size_t byte_count = ZSTR_LEN(zstr) - 1;
	const uint8_t modulo_bit_len = (uint8_t) bytes[byte_count];
	if (modulo_bit_len >= 8) {
		zend_throw_exception(spl_ce_RuntimeException, "Teds\\BitVector expected binary data to end with number of wasted bits", 0);
		RETURN_THROWS();
	}
	teds_bitvector_initialize_from_bytes(array, bytes, byte_count, modulo_bit_len);
	TEDS_RETURN_VOID();
}

static zend_always_inline zend_string *teds_create_string_from_entries_int8(const char *raw, const size_t len) {
	return zend_string_init(raw, len, 0);
}

static zend_always_inline zend_string *teds_create_string_from_entries_int16(const char *raw, const size_t len) {
#ifdef WORDS_BIGENDIAN
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
#ifdef WORDS_BIGENDIAN
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
#ifdef WORDS_BIGENDIAN
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

static zend_string *teds_bitvector_create_serialized_string(const teds_bitvector_entries *const array)
{
	const size_t len = array->bit_size;
	const uint8_t removed_bits = ((8 - len) & 7);
	const size_t str_len = BYTES_FOR_BIT_SIZE(len);
	const uint8_t mask = ((1 << (8 - removed_bits)) - 1);
	// fprintf(stderr, "serialize mask=%d str_len=%d len=%d removed=%d\n", (int)mask, (int)str_len, (int)len, (int)removed_bits);
	zend_string *const ret = zend_string_alloc(str_len + 1, 0);
	memcpy(ZSTR_VAL(ret), array->entries_bits, str_len);
	ZSTR_VAL(ret)[str_len - 1] &= mask;
	ZSTR_VAL(ret)[str_len] = removed_bits;
	ZSTR_VAL(ret)[str_len + 1] = '\0';
	return ret;
}

static zend_string *teds_bitvector_create_serialized_string_no_padding(const teds_bitvector_entries *const array)
{
	const size_t len = array->bit_size;
	const uint8_t removed_bits = ((8 - len) & 7);
	const size_t str_len = BYTES_FOR_BIT_SIZE(len);
	const uint8_t mask = ((1 << (8 - removed_bits)) - 1);
	zend_string *const ret = zend_string_alloc(str_len, 0);
	memcpy(ZSTR_VAL(ret), array->entries_bits, str_len);
	ZSTR_VAL(ret)[str_len - 1] &= mask;
	ZSTR_VAL(ret)[str_len] = '\0';
	return ret;
}

PHP_METHOD(Teds_BitVector, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_bitvector_entries *const array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t len = array->bit_size;
	if (len == 0) {
		RETURN_EMPTY_ARRAY();
	}
	/* Represent the bitvector as the bytes, followed by the number of bits % 8 */
	zval tmp;
	ZVAL_STR(&tmp, teds_bitvector_create_serialized_string(array));
	zend_array *result = zend_new_array(1);
	zend_hash_next_index_insert(result, &tmp);
	RETURN_ARR(result);
}

PHP_METHOD(Teds_BitVector, serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();
	const teds_bitvector_entries *const array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t len = array->bit_size;
	if (len == 0) {
		RETURN_EMPTY_STRING();
	}
	RETURN_STR(teds_bitvector_create_serialized_string(array));
}

PHP_METHOD(Teds_BitVector, unserialize)
{
	zend_string *zstr;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zstr)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *new_object = teds_bitvector_new(teds_ce_BitVector);
	teds_bitvector *bitvector = teds_bitvector_from_object(new_object);
	if (ZSTR_LEN(zstr) == 0) {
		teds_bitvector_entries_set_empty_list(&bitvector->array);
	} else {
		const uint8_t *bytes = (const uint8_t *)ZSTR_VAL(zstr);
		const size_t byte_count = ZSTR_LEN(zstr) - 1;
		const uint8_t modulo_bit_len = (uint8_t) bytes[byte_count];
		if (modulo_bit_len >= 8) {
			zend_throw_exception(spl_ce_RuntimeException, "Teds\\BitVector expected binary data to end with number of wasted bits", 0);
			RETURN_THROWS();
		}

		teds_bitvector_initialize_from_bytes(&bitvector->array, bytes, byte_count, modulo_bit_len);
	}
	RETURN_OBJ(new_object);
}

PHP_METHOD(Teds_BitVector, toBinaryString)
{
	ZEND_PARSE_PARAMETERS_NONE();
	const teds_bitvector_entries *const array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t len = array->bit_size;
	if (len == 0) {
		RETURN_EMPTY_STRING();
	}
	RETURN_STR(teds_bitvector_create_serialized_string_no_padding(array));
}

PHP_METHOD(Teds_BitVector, fromBinaryString)
{
	zend_string *zstr;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zstr)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *new_object = teds_bitvector_new(teds_ce_BitVector);
	teds_bitvector *bitvector = teds_bitvector_from_object(new_object);
	if (ZSTR_LEN(zstr) == 0) {
		teds_bitvector_entries_set_empty_list(&bitvector->array);
	} else {
		const uint8_t *bytes = (const uint8_t *)ZSTR_VAL(zstr);
		const size_t byte_count = ZSTR_LEN(zstr);

		teds_bitvector_initialize_from_bytes(&bitvector->array, bytes, byte_count, 0);
	}
	RETURN_OBJ(new_object);
}

static void teds_bitvector_entries_init_from_array_values(teds_bitvector_entries *array, zend_array *raw_data)
{
	const size_t num_entries = zend_hash_num_elements(raw_data);
	teds_bitvector_entries_set_empty_list(array);
	if (num_entries == 0) {
		return;
	}
	/* Precompute bit_capacity to avoid reallocations/branches */
	const size_t bytes_needed = BYTES_FOR_BIT_SIZE(num_entries);
	array->entries_bits = emalloc(bytes_needed);
	array->bit_capacity = bytes_needed * 8;

	zval *val_zv;
	ZEND_HASH_FOREACH_VAL(raw_data, val_zv) {
		zend_long val;
		TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(val, val_zv);
		teds_bitvector_entries_push(array, val, 0);
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Teds_BitVector, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_bitvector_new(teds_ce_BitVector);
	teds_bitvector *intern = teds_bitvector_from_object(object);
	teds_bitvector_entries_init_from_array_values(&intern->array, array_ht);

	RETURN_OBJ(object);
}

static zend_array *teds_bitvector_entries_to_refcounted_array(const teds_bitvector_entries *array) {
	size_t len = array->bit_size;
	const uint8_t *const bit_values = array->entries_bits;
	zend_array *const values = teds_new_array_check_overflow(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (size_t i = 0; i < len; i++) {
			ZVAL_BOOL(TEDS_FILL_VAL, ((bit_values[i >> 3] >> (i & 7)) & 1));
			ZEND_HASH_FILL_NEXT();
		}
	} ZEND_HASH_FILL_END();
	return values;
}

PHP_METHOD(Teds_BitVector, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	const teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	size_t len = array->bit_size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_bitvector_entries_to_refcounted_array(array));
}

static zend_always_inline void teds_bitvector_convert_zval_value_to_long_at_offset(zval *return_value, const zval *zval_this, zend_long offset)
{
	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(zval_this);
	size_t len = array->bit_size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		teds_throw_invalid_sequence_index_exception();
		RETURN_THROWS();
	}
	teds_bitvector_entries_copy_offset(array, offset, return_value, false);
}

PHP_METHOD(Teds_BitVector, get)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_bitvector_convert_zval_value_to_long_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_BitVector, offsetGet)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	teds_bitvector_convert_zval_value_to_long_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_BitVector, offsetExists)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	RETURN_BOOL((zend_ulong) offset < Z_BITVECTOR_ENTRIES_P(ZEND_THIS)->bit_size);
}

/* containsKey is similar to offsetExists but rejects non-integers */
PHP_METHOD(Teds_BitVector, containsKey)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(offset_zv) != IS_LONG) {
		RETURN_FALSE;
	}
	zend_long offset = Z_LVAL_P(offset_zv);

	RETURN_BOOL((zend_ulong) offset < Z_BITVECTOR_ENTRIES_P(ZEND_THIS)->bit_size);
}

PHP_METHOD(Teds_BitVector, indexOf)
{
	bool value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t len = array->bit_size;
	if (len == 0) {
		RETURN_NULL();
	}
	const uint8_t *const entries_bits = array->entries_bits;
	size_t i = 0;
	while (i + 64 <= len) {
		if ((*(uint64_t *)&entries_bits[i >> 3]) + value) {
			break;
		}
		i += 64;
	}
	while (i + 8 <= len) {
		if ((uint8_t)(entries_bits[i >> 3] + value)) {
			break;
		}
		i += 8;
	}
	// TODO: builtin_clz?
	uint8_t v = entries_bits[i >> 3] + value;
	while (i < len) {
		if (v & 1) {
			RETURN_LONG(i);
		}
		i++;
		v >>= 1;
	}
	RETURN_NULL();
}

PHP_METHOD(Teds_BitVector, contains)
{
	zval *value_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value_zv)
	ZEND_PARSE_PARAMETERS_END();

	if (!teds_bitvector_is_bool(value_zv)) {
		RETURN_FALSE;
	}
	const uint8_t value = Z_TYPE_P(value_zv) != IS_FALSE ? 1 : 0;
	const teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const uint8_t *const entries_bits = array->entries_bits;
	const size_t len = array->bit_size;
	if (len == 0) {
		RETURN_FALSE;
	}
	size_t i = 0;
	while (i + 64 <= len) {
		if ((*(uint64_t *)&entries_bits[i >> 3]) + value) {
			break;
		}
		i += 64;
	}
	while (i + 8 <= len) {
		if ((uint8_t)(entries_bits[i >> 3] + value)) {
			RETURN_TRUE;
		}
		i += 8;
	}
	const uint8_t v = entries_bits[i >> 3] + value;
	RETURN_BOOL(v & ((1 << (len & 7)) - 1));
}

static zend_always_inline void teds_bitvector_set_value_at_offset(zend_object *object, const zend_long offset, const bool value) {
	teds_bitvector_entries *array = &teds_bitvector_from_object(object)->array;
	size_t len = array->bit_size;
	if (UNEXPECTED((zend_ulong) offset >= len) || offset < 0) {
		teds_throw_invalid_sequence_index_exception();
		return;
	}
	uint8_t *const ptr = &array->entries_bits[((zend_ulong) offset) >> 3];
	if (value) {
		*ptr |= (1 << (offset & 7));
	} else {
		*ptr &= ~(1 << (offset & 7));
	}
}

PHP_METHOD(Teds_BitVector, set)
{
	zend_long offset;
	zval *value_zv;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(value_zv)
	ZEND_PARSE_PARAMETERS_END();

	bool value;
	TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(value, value_zv);

	teds_bitvector_set_value_at_offset(Z_OBJ_P(ZEND_THIS), offset, value);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_BitVector, setBit)
{
	zend_long offset;
	bool value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_bitvector_set_value_at_offset(Z_OBJ_P(ZEND_THIS), offset, value);
	TEDS_RETURN_VOID();
}


PHP_METHOD(Teds_BitVector, offsetSet)
{
	zval                  *offset_zv, *value_zv;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(offset_zv)
		Z_PARAM_ZVAL(value_zv)
	ZEND_PARSE_PARAMETERS_END();

	bool value;
	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);
	TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(value, value_zv);

	teds_bitvector_set_value_at_offset(Z_OBJ_P(ZEND_THIS), offset, value);
	TEDS_RETURN_VOID();
}

#define teds_bitvector_convert_8(x) (x)
#ifdef WORDS_BIGENDIAN
#define teds_bitvector_convert_16(x) teds_bswap_16((x))
#define teds_bitvector_convert_32(x) teds_bswap_32((x))
#define teds_bitvector_convert_64(x) teds_bswap_64((x))
#else
#define teds_bitvector_convert_16(x) (x)
#define teds_bitvector_convert_32(x) (x)
#define teds_bitvector_convert_64(x) (x)
#endif

#define TEDS_BITVECTOR_DECLARE_INT_GETTER(methodName, cTypeName, cTypeNameUnsigned, errorTypeName, convertName) \
PHP_METHOD(Teds_BitVector, methodName) \
{ \
	zend_long offset_int; \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(offset_int) \
	ZEND_PARSE_PARAMETERS_END(); \
	teds_bitvector_entries *const array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS); \
	if (UNEXPECTED(((zend_ulong)offset_int) >= (array->bit_size / (sizeof(cTypeNameUnsigned) * 8)) || offset_int < 0)) { \
		teds_throw_invalid_sequence_index_exception(); \
		RETURN_THROWS(); \
	} \
	cTypeName result = ((cTypeName)(cTypeNameUnsigned)convertName(((cTypeNameUnsigned *)array->entries_bits)[offset_int])); \
	RETURN_LONG(result); \
}

#define TEDS_BITVECTOR_DECLARE_INT_SETTER(methodName, cTypeName, cTypeNameUnsigned, errorTypeName, convertName) \
PHP_METHOD(Teds_BitVector, methodName) \
{ \
	zend_long offset_int; \
	zend_long offset_value; \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_LONG(offset_int) \
		Z_PARAM_LONG(offset_value) \
	ZEND_PARSE_PARAMETERS_END(); \
	teds_bitvector_entries *const array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS); \
	if (UNEXPECTED(((zend_ulong)offset_int) >= (array->bit_size / (sizeof(cTypeNameUnsigned) * 8)) || offset_int < 0)) { \
		teds_throw_invalid_sequence_index_exception(); \
		RETURN_THROWS(); \
	} \
	((cTypeNameUnsigned *)array->entries_bits)[offset_int] = convertName(offset_value); \
	TEDS_RETURN_VOID(); \
}


TEDS_BITVECTOR_DECLARE_INT_GETTER(getInt8,   int8_t,   uint8_t,  int8, teds_bitvector_convert_8)
TEDS_BITVECTOR_DECLARE_INT_GETTER(getInt16,  int16_t,  uint16_t, int16, teds_bitvector_convert_16)
TEDS_BITVECTOR_DECLARE_INT_GETTER(getInt32,  int32_t,  uint32_t, int32, teds_bitvector_convert_32)
TEDS_BITVECTOR_DECLARE_INT_GETTER(getUInt8,  uint8_t,  uint8_t,  int8, teds_bitvector_convert_8)
TEDS_BITVECTOR_DECLARE_INT_GETTER(getUInt16, uint16_t, uint16_t, int16, teds_bitvector_convert_16)
TEDS_BITVECTOR_DECLARE_INT_GETTER(getUInt32, uint32_t, uint32_t, int32, teds_bitvector_convert_32)
TEDS_BITVECTOR_DECLARE_INT_SETTER(setInt8,   int8_t,   uint8_t,  int8, teds_bitvector_convert_8)
TEDS_BITVECTOR_DECLARE_INT_SETTER(setInt16,  int16_t,  uint16_t, int16, teds_bitvector_convert_16)
TEDS_BITVECTOR_DECLARE_INT_SETTER(setInt32,  int32_t,  uint32_t, int32, teds_bitvector_convert_32)
#if SIZEOF_ZEND_LONG > 4
TEDS_BITVECTOR_DECLARE_INT_GETTER(getInt64,  int64_t,  uint64_t, int64, teds_bitvector_convert_64)
TEDS_BITVECTOR_DECLARE_INT_SETTER(setInt64,  int64_t,  uint64_t, int64, teds_bitvector_convert_64)
#else
ZEND_COLD PHP_METHOD(Teds_BitVector, getInt64)
{
	zend_long offset_int;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset_int)
	ZEND_PARSE_PARAMETERS_END();
	teds_throw_unsupportedoperationexception("Teds\\BitVector 64-bit method not supported on 32-bit builds");
}
ZEND_COLD PHP_METHOD(Teds_BitVector, setInt64)
{
	zend_long offset_int;
	zend_long offset_value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset_int)
		Z_PARAM_LONG(offset_value)
	ZEND_PARSE_PARAMETERS_END();
	teds_throw_unsupportedoperationexception("Teds\\BitVector 64-bit method not supported on 32-bit builds");
}
#endif

static zend_always_inline bool teds_bitvector_entries_read_offset_as_bool(const teds_bitvector_entries *const array, const size_t offset)
{
	ZEND_ASSERT(array->bit_size <= array->bit_capacity);
	ZEND_ASSERT(offset <= array->bit_size);

	return (array->entries_bits[offset >> 3] >> (offset & 7)) & 1;
}

static zend_always_inline void teds_bitvector_entries_copy_offset(const teds_bitvector_entries *array, const zend_ulong offset, zval *dst, bool pop)
{
	ZEND_ASSERT(pop ? array->bit_size < array->bit_capacity : array->bit_size <= array->bit_capacity);
	ZEND_ASSERT(pop ? offset == array->bit_size : offset < array->bit_size);
	ZEND_ASSERT(dst != NULL);

	ZVAL_BOOL(dst, teds_bitvector_entries_read_offset_as_bool(array, offset));
}

static zend_always_inline zval *teds_bitvector_entries_read_offset(const teds_bitvector_entries *const array, const size_t offset, zval *const tmp)
{
	ZVAL_BOOL(tmp, teds_bitvector_entries_read_offset_as_bool(array, offset));
	return tmp;
}

static zend_always_inline void teds_bitvector_entries_push(teds_bitvector_entries *array, const bool value, const bool check_capacity)
{
	const size_t old_size = array->bit_size;
	if (check_capacity) {
		const size_t old_capacity = array->bit_capacity;

		ZEND_ASSERT((old_capacity & 7) == 0);

		if (UNEXPECTED(old_size >= old_capacity)) {
			ZEND_ASSERT(old_size == old_capacity);
			const size_t new_capacity = teds_bitvector_compute_next_valid_capacity(old_size + (old_size >> 1));
			ZEND_ASSERT(new_capacity > old_capacity);
			teds_bitvector_entries_raise_capacity(array, new_capacity);
		}
	} else {
		ZEND_ASSERT(old_size < array->bit_capacity);
	}
	const size_t position = old_size >> 3;
	const uint8_t bit = 1 << (old_size & 7);
	// fprintf(stderr, "size=%d capacity=%d check_capacity=%d position=%d bit=%d\n", (int)array->bit_size, (int)array->bit_capacity, (int)check_capacity, (int)position, (int) bit);
	if (value) {
		array->entries_bits[position] |= bit;
	} else {
		array->entries_bits[position] &= ~bit;
	}
	array->bit_size++;
}

/* Based on array_push */
PHP_METHOD(Teds_BitVector, push)
{
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(argc == 0)) {
		return;
	}
	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	const size_t new_size = old_size + argc;
	/* The compiler will type check but eliminate dead code on platforms where size_t is 32 bits (4 bytes) */
	if (SIZEOF_SIZE_T < 8 && UNEXPECTED(new_size > MAX_VALID_OFFSET + 1 || new_size < old_size)) {
		teds_error_noreturn_max_bitvector_capacity();
		ZEND_UNREACHABLE();
	}

	for (uint32_t i = 0; i < argc; i++) {
		zend_long new_value;
		TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(new_value, (&args[i]));
		teds_bitvector_entries_push(array, new_value, true);
	}
	TEDS_RETURN_VOID();
}

static zend_always_inline void teds_bitvector_entries_unshift_single(teds_bitvector_entries *array, const bool value)
{
	const size_t old_size = array->bit_size;
	const size_t old_capacity = array->bit_capacity;

	ZEND_ASSERT((old_capacity & 7) == 0);

	if (UNEXPECTED(old_size >= old_capacity)) {
		ZEND_ASSERT(old_size == old_capacity);
		const size_t new_capacity = teds_bitvector_compute_next_valid_capacity(old_size + (old_size >> 1));
		ZEND_ASSERT(new_capacity > old_capacity);
		teds_bitvector_entries_raise_capacity(array, new_capacity);
	}

	array->bit_size++;
	ZEND_ASSERT(array->bit_size <= array->bit_capacity);
	size_t n = old_size >> 3;
	uint8_t *const entries_bits = array->entries_bits;
#ifdef WORDS_BIGENDIAN
	for (; n > 0; n--) {
		entries_bits[n] = (entries_bits[n] << 1) | (entries_bits[n - 1] >> 7);
	}
#else
	for (; n & 7; n--) {
		entries_bits[n] = (entries_bits[n] << 1) | (entries_bits[n - 1] >> 7);
	}
	if (n >= 8) {
		entries_bits[n] = (entries_bits[n] << 1) | (entries_bits[n - 1] >> 7);
		while (n > 8) {
			n -= 8;
			*((uint64_t *)(&entries_bits[n])) = (*((uint64_t *)(&entries_bits[n])) << 1) | (entries_bits[n - 1] >> 7);
		}
		*((uint64_t *)(&entries_bits[0])) = (*((uint64_t *)(&entries_bits[0])) << 1) | (value ? 1 : 0);
		return;
	}
	ZEND_ASSERT(n == 0);
#endif
	entries_bits[0] = (entries_bits[0] << 1) | (value ? 1 : 0);
}

static zend_always_inline void teds_bitvector_entries_insert_single(teds_bitvector_entries *array, const size_t offset, const bool value)
{
	const size_t old_size = array->bit_size;
	const size_t old_capacity = array->bit_capacity;

	ZEND_ASSERT((old_capacity & 7) == 0);

	if (UNEXPECTED(old_size >= old_capacity)) {
		ZEND_ASSERT(old_size == old_capacity);
		const size_t new_capacity = teds_bitvector_compute_next_valid_capacity(old_size + (old_size >> 1));
		ZEND_ASSERT(new_capacity > old_capacity);
		teds_bitvector_entries_raise_capacity(array, new_capacity);
	}

	array->bit_size++;
	ZEND_ASSERT(array->bit_size <= array->bit_capacity);
	size_t n = old_size >> 3;
	const size_t start = offset >> 3;
	uint8_t *const entries_bits = array->entries_bits;
	for (; n > start; n--) {
		entries_bits[n] = (entries_bits[n] << 1) | (entries_bits[n - 1] >> 7);
	}
	const uint8_t bitpos = offset & 7;
	const uint8_t insert_mask = (1 << (bitpos)) - 1;
	const uint8_t old = entries_bits[n];
	/* Insert the bit after least insignificant bits and move up the most significant bits */
	entries_bits[n] = (old & insert_mask) | ((value ? 1 : 0) << bitpos) | ((old & ~insert_mask) << 1);
}

PHP_METHOD(Teds_BitVector, unshift)
{
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(argc == 0)) {
		return;
	}
	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	const size_t new_size = old_size + argc;
	/* The compiler will type check but eliminate dead code on platforms where size_t is 32 bits (4 bytes) */
	if (SIZEOF_SIZE_T < 8 && UNEXPECTED(new_size > MAX_VALID_OFFSET + 1 || new_size < old_size)) {
		teds_error_noreturn_max_bitvector_capacity();
		ZEND_UNREACHABLE();
	}

	/* TODO: Optimize this for large varargs case */
	for (uint32_t i = 0; i < argc; i++) {
		zend_long new_value;
		TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(new_value, (&args[i]));
		teds_bitvector_entries_unshift_single(array, new_value);
	}
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_BitVector, insert)
{
	zend_long offset;
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_LONG(offset);
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	if (UNEXPECTED((zend_ulong) offset > array->bit_size || offset < 0)) {
		teds_throw_invalid_sequence_index_exception();
		return;
	}
	if (UNEXPECTED(argc == 0)) {
		return;
	}
	const size_t new_size = old_size + argc;
	/* The compiler will type check but eliminate dead code on platforms where size_t is 32 bits (4 bytes) */
	if (SIZEOF_SIZE_T < 8 && UNEXPECTED(new_size > MAX_VALID_OFFSET + 1 || new_size < old_size)) {
		teds_error_noreturn_max_bitvector_capacity();
		ZEND_UNREACHABLE();
	}

	/* TODO: Optimize this for large varargs case */
	for (uint32_t i = 0; i < argc; i++) {
		zend_long new_value;
		TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(new_value, (&args[i]));
		teds_bitvector_entries_insert_single(array, offset + i, new_value);
	}
	TEDS_RETURN_VOID();
}

/* Based on array_push */
PHP_METHOD(Teds_BitVector, pushBits)
{
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(argc == 0)) {
		return;
	}
	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	const size_t new_size = old_size + argc;
	/* The compiler will type check but eliminate dead code on platforms where size_t is 32 bits (4 bytes) */
	if (SIZEOF_SIZE_T < 8 && UNEXPECTED(new_size > MAX_VALID_OFFSET + 1 || new_size < old_size)) {
		teds_error_noreturn_max_bitvector_capacity();
		ZEND_UNREACHABLE();
	}

	const size_t new_capacity = teds_bitvector_compute_next_valid_capacity(new_size);
	if (UNEXPECTED(new_capacity > array->bit_capacity)) {
		teds_bitvector_entries_raise_capacity(array, teds_bitvector_compute_next_valid_capacity(new_size + (new_size >> 1)));
	}

	uint32_t i = 0;
	do {
		bool new_value;
		TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(new_value, (&args[i]));
		teds_bitvector_entries_push(array, new_value, false);
		i++;
	} while (i < argc);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_BitVector, pop)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty Teds\\BitVector", 0);
		RETURN_THROWS();
	}
	array->bit_size--;
	teds_bitvector_entries_copy_offset(array, array->bit_size, return_value, true);

	const size_t capacity = teds_bitvector_compute_next_valid_capacity(old_size);
	if (UNEXPECTED(capacity < array->bit_capacity)) {
		teds_bitvector_entries_shrink_capacity(array, old_size - 1, capacity, array->entries_bits);
	}
}

PHP_METHOD(Teds_BitVector, first)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read first bit of empty Teds\\BitVector", 0);
		RETURN_THROWS();
	}
	teds_bitvector_entries_copy_offset(array, 0, return_value, false);
}

PHP_METHOD(Teds_BitVector, last)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read last bit of empty Teds\\BitVector", 0);
		RETURN_THROWS();
	}
	teds_bitvector_entries_copy_offset(array, old_size - 1, return_value, false);
}

static zend_always_inline void teds_bitvector_entries_shift(teds_bitvector_entries *array) {
	uint8_t *const entries_bits = array->entries_bits;
	ZEND_ASSERT(array->bit_size <= array->bit_capacity);
	ZEND_ASSERT(array->bit_size <= array->bit_capacity);
	array->bit_size--;
	/* Offset of the last byte *after* removing a bit */
	const size_t last_byte_offset = array->bit_size >> 3;
	/* Remove the least significant bit and replace it */
	/* TODO: Optimize */
	size_t i = 0;
#ifndef WORDS_BIGENDIAN
	for (; i + 8 <= last_byte_offset; i += 8) {
		*((uint64_t *)(&entries_bits[i])) = (*((uint64_t *)(&entries_bits[i])) >> 1) | (((uint64_t)entries_bits[i + 8]) << 63);
	}
#endif
	for (; i < last_byte_offset; i++) {
		entries_bits[i] = (entries_bits[i] >> 1) | (entries_bits[i + 1] << 7);
	}
	/* E.g. if there were originally 64 bits, and then 63, replace the 7th byte with the 7th byte shifted by 1, taking nothing from the next byte (last_byte_offset = 7) */
	entries_bits[i] = (entries_bits[i] >> 1);
}

static zend_always_inline uint8_t remove_bit_from_byte(const uint8_t old, const uint8_t bitpos) {
	ZEND_ASSERT(bitpos < 8);
	const uint8_t keep_mask = (1 << (bitpos)) - 1;
	return (old & keep_mask) | ((old & ~((1 << (bitpos + 1)) - 1)) >> 1);
}

static zend_always_inline void teds_bitvector_entries_unset_offset(teds_bitvector_entries *array, size_t offset_bit) {
	uint8_t *const entries_bits = array->entries_bits;
	ZEND_ASSERT(array->bit_size <= array->bit_capacity);
	ZEND_ASSERT(offset_bit < array->bit_size);
	array->bit_size--;
	/* Number of bytes *after* removing the last byte, minus 1 */
	const size_t last_byte_offset = array->bit_size >> 3;
	size_t it_byte_offset = offset_bit >> 3;
	const uint8_t bitpos = offset_bit & 7;
	const uint8_t old = entries_bits[it_byte_offset];

	ZEND_ASSERT(it_byte_offset <= last_byte_offset);
	if (it_byte_offset == last_byte_offset) {
		entries_bits[it_byte_offset] = remove_bit_from_byte(old, bitpos);
		return;
	}

	entries_bits[it_byte_offset] = remove_bit_from_byte(old, bitpos) | (entries_bits[it_byte_offset + 1] << 7);
	it_byte_offset++;
	/* E.g. if there were originally 64 bits, and then 63, replace the 7th byte with the 7th byte shifted by 1, taking nothing from the next byte (last_byte_offset = 7) */
	while (it_byte_offset < last_byte_offset) {
		entries_bits[it_byte_offset] = (entries_bits[it_byte_offset] >> 1) | (entries_bits[it_byte_offset + 1] << 7);
		it_byte_offset++;
	}
	entries_bits[it_byte_offset] = entries_bits[it_byte_offset] >> 1;
}

PHP_METHOD(Teds_BitVector, shift)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty Teds\\BitVector", 0);
		RETURN_THROWS();
	}
	/* Remove the least significant bit and replace it */
	ZVAL_BOOL(return_value, array->entries_bits[0] & 1);
	teds_bitvector_entries_shift(array);

	const size_t capacity = teds_bitvector_compute_next_valid_capacity(old_size);
	if (UNEXPECTED(capacity < array->bit_capacity)) {
		teds_bitvector_entries_shrink_capacity(array, old_size - 1, capacity, array->entries_bits);
	}
}

PHP_METHOD(Teds_BitVector, offsetUnset)
{
	zval                  *offset_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv);
	ZEND_PARSE_PARAMETERS_END();

	teds_bitvector_entries *array = Z_BITVECTOR_ENTRIES_P(ZEND_THIS);
	const size_t old_size = array->bit_size;
	zend_long offset;

	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	if (UNEXPECTED(!teds_offset_within_size_t(offset, array->bit_size))) {
		TEDS_THROW_INVALID_SEQUENCE_INDEX_EXCEPTION();
	}

	teds_bitvector_entries_unset_offset(array, offset);
	const size_t capacity = teds_bitvector_compute_next_valid_capacity(old_size);
	if (UNEXPECTED(capacity < array->bit_capacity)) {
		teds_bitvector_entries_shrink_capacity(array, old_size - 1, capacity, array->entries_bits);
	}
}

static void teds_bitvector_write_dimension(zend_object *object, zval *offset_zv, zval *value_zv)
{
	teds_bitvector *intern = teds_bitvector_from_object(object);

	bool v;
	TEDS_BITVECTOR_VALUE_TO_BOOL_OR_THROW(v, value_zv);

	if (!offset_zv) {
		teds_bitvector_entries_push(&intern->array, v, true);
		return;
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	if (UNEXPECTED(!teds_offset_within_size_t(offset, intern->array.bit_size))) {
		teds_throw_invalid_sequence_index_exception();
		return;
	}
	teds_bitvector_set_value_at_offset(object, offset, v);
}

static zval *teds_bitvector_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (UNEXPECTED(!offset_zv || Z_ISUNDEF_P(offset_zv))) {
handle_missing_key:
		if (type != BP_VAR_IS) {
			teds_throw_invalid_sequence_index_exception();
			return NULL;
		}
		return &EG(uninitialized_zval);
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW_RETURN_NULLPTR(offset, offset_zv);

	const teds_bitvector_entries *array = &teds_bitvector_from_object(object)->array;

	if (UNEXPECTED(!teds_offset_within_size_t(offset, array->bit_size))) {
		goto handle_missing_key;
	} else {
		return teds_bitvector_entries_read_offset(array, (zend_ulong) offset, rv);
	}
}

static int teds_bitvector_has_dimension(zend_object *object, zval *offset_zv, int check_empty)
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

	const teds_bitvector_entries *array = &teds_bitvector_from_object(object)->array;

	if (UNEXPECTED(!teds_offset_within_size_t(offset, array->bit_size))) {
		return 0;
	}

	/* TODO can optimize */
	if (check_empty) {
		return teds_bitvector_entries_read_offset_as_bool(array, offset);
	}
	return true; /* int !== null */
}

PHP_MINIT_FUNCTION(teds_bitvector)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_BitVector = register_class_Teds_BitVector(zend_ce_aggregate, teds_ce_Sequence, php_json_serializable_ce);
	teds_ce_BitVector->create_object = teds_bitvector_new;

	memcpy(&teds_handler_BitVector, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_BitVector.offset          = XtOffsetOf(teds_bitvector, std);
	teds_handler_BitVector.clone_obj       = teds_bitvector_clone;
	teds_handler_BitVector.count_elements  = teds_bitvector_count_elements;
	teds_handler_BitVector.get_properties  = teds_noop_empty_array_get_properties;
	teds_handler_BitVector.get_properties_for = teds_bitvector_get_properties_for;
	teds_handler_BitVector.get_gc          = teds_noop_get_gc;
	teds_handler_BitVector.free_obj        = teds_bitvector_free_storage;

	teds_handler_BitVector.read_dimension  = teds_bitvector_read_dimension;
	teds_handler_BitVector.write_dimension = teds_bitvector_write_dimension;
	teds_handler_BitVector.has_dimension   = teds_bitvector_has_dimension;

	teds_ce_BitVector->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_BitVector->get_iterator = teds_bitvector_get_iterator;

	return SUCCESS;
}
