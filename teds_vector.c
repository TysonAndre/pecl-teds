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
#include "teds_vector_arginfo.h"
#include "teds_vector.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_util.h"

#include <stdbool.h>

zend_object_handlers teds_handler_Vector;
zend_class_entry *teds_ce_Vector;

/* This is a placeholder value to distinguish between empty and uninitialized Vector instances.
 * Compilers require at least one element. Make this constant - reads/writes should be impossible. */
static const zval empty_entry_list[1];

typedef struct _teds_vector_entries {
	size_t size;
	size_t capacity;
	zval *entries;
} teds_vector_entries;

typedef struct _teds_vector {
	teds_vector_entries		array;
	zend_object				std;
} teds_vector;

/* Used by InternalIterator returned by Vector->getIterator() */
typedef struct _teds_vector_it {
	zend_object_iterator intern;
	zend_long            current;
} teds_vector_it;

static void teds_vector_raise_capacity(teds_vector *intern, const zend_long new_capacity);

static teds_vector *teds_vector_from_object(zend_object *obj)
{
	return (teds_vector*)((char*)(obj) - XtOffsetOf(teds_vector, std));
}

#define Z_VECTOR_P(zv)  teds_vector_from_object(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then entries == NULL
 *   - if size > 0, then entries != NULL
 *   - size is not less than 0
 */
static bool teds_vector_entries_empty_size(const teds_vector_entries *array)
{
	if (array->size > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		ZEND_ASSERT(array->capacity >= array->size);
		return false;
	}
	// This vector may have reserved capacity.
	return true;
}

static bool teds_vector_entries_empty_capacity(const teds_vector_entries *array)
{
	if (array->capacity > 0) {
		ZEND_ASSERT(array->entries != empty_entry_list);
		return false;
	}
	// This vector may have reserved capacity.
	return true;
}

static bool teds_vector_entries_uninitialized(const teds_vector_entries *array)
{
	if (array->entries == NULL) {
		ZEND_ASSERT(array->size == 0);
		ZEND_ASSERT(array->capacity == 0);
		return true;
	}
	ZEND_ASSERT((array->entries == empty_entry_list && array->capacity == 0) || array->capacity > 0);
	return false;
}

static void teds_vector_raise_capacity(teds_vector *intern, const zend_long new_capacity) {
	ZEND_ASSERT(new_capacity > intern->array.capacity);
	if (intern->array.capacity == 0) {
		intern->array.entries = safe_emalloc(new_capacity, sizeof(zval), 0);
	} else {
		intern->array.entries = safe_erealloc(intern->array.entries, new_capacity, sizeof(zval), 0);
	}
	intern->array.capacity = new_capacity;
	ZEND_ASSERT(intern->array.entries != NULL);
}

static inline void teds_vector_shrink_capacity(teds_vector_entries *array, size_t size, size_t capacity, zval *old_entries) {
	ZEND_ASSERT(size <= capacity);
	ZEND_ASSERT(size == array->size);
	ZEND_ASSERT(capacity > 0);
	ZEND_ASSERT(capacity < array->capacity);
	ZEND_ASSERT(old_entries == array->entries);
	zval *new_entries = safe_emalloc(capacity, sizeof(zval), 0);
	ZEND_ASSERT(new_entries != NULL);
	memcpy(new_entries, old_entries, size * sizeof(zval));

	array->entries = new_entries;
	array->capacity = capacity;
	efree(old_entries);
}

/* Initializes the range [from, to) to null. Does not dtor existing entries. */
/* TODO: Delete if this isn't used in the final version
static void teds_vector_entries_init_elems(teds_vector_entries *array, zend_long from, zend_long to)
{
	ZEND_ASSERT(from <= to);
	zval *begin = &array->entries[from];
	zval *end = &array->entries[to];

	while (begin != end) {
		ZVAL_NULL(begin++);
	}
}
*/

static inline void teds_vector_entries_set_empty_list(teds_vector_entries *array) {
	array->size = 0;
	array->capacity = 0;
	array->entries = (zval *)empty_entry_list;
}

static void teds_vector_entries_init_from_array(teds_vector_entries *array, zend_array *values, bool preserve_keys)
{
	const zend_long num_elements = zend_hash_num_elements(values);
	if (num_elements <= 0) {
		teds_vector_entries_set_empty_list(array);
		return;
	}

	zval *val;
	zval *entries;

	array->size = 0; /* reset size in case emalloc() fails */
	if (preserve_keys) {
		zend_string *str_index;
		zend_ulong num_index, max_index = 0;

		ZEND_HASH_FOREACH_KEY(values, num_index, str_index) {
			if (str_index != NULL || (zend_long)num_index < 0) {
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "array must contain only positive integer keys");
				return;
			}

			if (num_index > max_index) {
				max_index = num_index;
			}
		} ZEND_HASH_FOREACH_END();

		const zend_long size = max_index + 1;
		if (size <= 0) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "integer overflow detected");
			return;
		}
		array->entries = entries = safe_emalloc(size, sizeof(zval), 0);
		array->size = size;
		array->capacity = size;
		/* Optimization: Don't need to null remaining elements if all elements from 0..num_elements-1 are set. */
		ZEND_ASSERT(size >= num_elements);
		if (size > num_elements) {
			for (zend_long i = 0; i < size; i++) {
				ZVAL_NULL(&entries[i]);
			}
		}

		ZEND_HASH_FOREACH_KEY_VAL(values, num_index, str_index, val) {
			ZEND_ASSERT(num_index < size);
			ZEND_ASSERT(!str_index);
			ZVAL_COPY_DEREF(&entries[num_index], val);
		} ZEND_HASH_FOREACH_END();
		return;
	}

	int i = 0;
	array->entries = entries = safe_emalloc(num_elements, sizeof(zval), 0);
	array->size = num_elements;
	array->capacity = num_elements;
	ZEND_HASH_FOREACH_VAL(values, val)  {
		ZEND_ASSERT(i < num_elements);
		ZVAL_COPY_DEREF(&entries[i], val);
		i++;
	} ZEND_HASH_FOREACH_END();
}

static void teds_vector_entries_init_from_traversable(teds_vector_entries *array, zend_object *obj, bool preserve_keys)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	zend_long size = 0, capacity = 0;
	array->size = 0;
	array->entries = NULL;
	zval *entries = NULL;
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

	if (preserve_keys) {
		if (!funcs->get_current_key) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Saw traversable without keys");
			return;
		}

		/* size is 0 or max_index + 1 */
		while (funcs->valid(iter) == SUCCESS) {
			if (UNEXPECTED(EG(exception))) {
				break;
			}

			zval key;
			funcs->get_current_key(iter, &key);
			if (UNEXPECTED(EG(exception))) {
				break;
			}
			if (Z_TYPE(key) != IS_LONG || Z_LVAL(key) < 0) {
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "array must contain only positive integer keys");
				break;
			}
			const zend_long num_index = Z_LVAL(key);
			if (num_index >= capacity) {
				if (UNEXPECTED((zend_ulong) num_index > ZEND_LONG_MAX / 2 - 1)) {
					zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "integer overflow detected");
					break;
				}
				const zend_long new_capacity = (num_index + 1) * 2;
				entries = safe_erealloc(entries, new_capacity, sizeof(zval), 0);
				for ( ; capacity < new_capacity; capacity++) {
					ZVAL_NULL(&entries[capacity]);
				}
			}

			zval *value = funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception))) {
				break;
			}

			if (num_index >= size) {
				size = num_index + 1;
				ZEND_ASSERT(size <= capacity);
			}

			/* Allow Traversables such as Generators to repeat keys. Silently overwrite the old key. */
			zval_ptr_dtor(&entries[num_index]);
			if (UNEXPECTED(EG(exception))) {
				break;
			}
			ZVAL_COPY_DEREF(&entries[num_index], value);

			iter->index++;
			funcs->move_forward(iter);
			if (UNEXPECTED(EG(exception))) {
				break;
			}
		}
	} else {
		/* Reindex keys from 0. */
		while (funcs->valid(iter) == SUCCESS) {
			if (EG(exception)) {
				break;
			}
			zval *value = funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception))) {
				break;
			}
			if (UNEXPECTED(EG(exception))) {
				break;
			}

			if (size >= capacity) {
				/* TODO: Could use countable and get_count handler to estimate the size of the array to allocate */
				if (entries) {
					capacity *= 2;
					entries = safe_erealloc(entries, capacity, sizeof(zval), 0);
				} else {
					capacity = 4;
					entries = safe_emalloc(capacity, sizeof(zval), 0);
				}
			}
			ZVAL_COPY_DEREF(&entries[size], value);
			size++;

			iter->index++;
			funcs->move_forward(iter);
			if (UNEXPECTED(EG(exception))) {
				break;
			}
		}
	}
	if (capacity > size) {
		/* Shrink allocated value to actual required size */
		entries = erealloc(entries, size * sizeof(zval));
	}

	array->size = size;
	array->capacity = size;
	array->entries = entries;
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

/* Copies the range [begin, end) into the vector, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void teds_vector_copy_range(teds_vector_entries *array, size_t offset, zval *begin, zval *end)
{
	ZEND_ASSERT(array->size - offset >= end - begin);

	zval *to = &array->entries[offset];
	while (begin != end) {
		ZVAL_COPY(to, begin);
		begin++;
		to++;
	}
}

static void teds_vector_entries_copy_ctor(teds_vector_entries *to, const teds_vector_entries *from)
{
	zend_long size = from->size;
	if (!size) {
		teds_vector_entries_set_empty_list(to);
		return;
	}

	to->size = 0; /* reset size in case emalloc() fails */
	to->capacity = 0;
	to->entries = safe_emalloc(size, sizeof(zval), 0);
	to->size = size;
	to->capacity = size;

	zval *begin = from->entries, *end = from->entries + size;
	teds_vector_copy_range(to, 0, begin, end);
}

/* Destructs the entries in the range [from, to).
 * Caller is expected to bounds check.
 */
static void teds_vector_entries_dtor_range(teds_vector_entries *array, size_t from, size_t to)
{
	zval *begin = array->entries + from, *end = array->entries + to;
	while (begin != end) {
		zval_ptr_dtor(begin);
		begin++;
	}
}

/* Destructs and frees contents but not the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void teds_vector_entries_dtor(teds_vector_entries *array)
{
	if (!teds_vector_entries_empty_capacity(array)) {
		teds_vector_entries_dtor_range(array, 0, array->size);
		efree(array->entries);
	}
}

static HashTable* teds_vector_get_gc(zend_object *obj, zval **table, int *n)
{
	teds_vector *intern = teds_vector_from_object(obj);

	*table = intern->array.entries;
	*n = (int)intern->array.size;

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_vector_get_properties(zend_object *obj)
{
	teds_vector *intern = teds_vector_from_object(obj);
	HashTable *ht = zend_std_get_properties(obj);

	/* Re-initialize properties array */

	// Note that destructors may mutate the original array,
	// so we fetch the size and circular buffer each time to avoid invalid memory accesses.
	for (size_t i = 0; i < intern->array.size; i++) {
		zval *elem = &intern->array.entries[i];
		Z_TRY_ADDREF_P(elem);
		zend_hash_index_update(ht, i, elem);
	}
	const size_t properties_size = zend_hash_num_elements(ht);
	if (UNEXPECTED(properties_size > intern->array.size)) {
		for (size_t i = intern->array.size; i < properties_size; i++) {
			zend_hash_index_del(ht, i);
		}
	}

	return ht;
}

static void teds_vector_free_storage(zend_object *object)
{
	teds_vector *intern = teds_vector_from_object(object);
	teds_vector_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_vector_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_vector *intern;

	intern = zend_object_alloc(sizeof(teds_vector), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_Vector);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_Vector;

	if (orig && clone_orig) {
		teds_vector *other = teds_vector_from_object(orig);
		teds_vector_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.entries = NULL;
	}

	return &intern->std;
}

static zend_object *teds_vector_new(zend_class_entry *class_type)
{
	return teds_vector_new_ex(class_type, NULL, 0);
}


static zend_object *teds_vector_clone(zend_object *old_object)
{
	zend_object *new_object = teds_vector_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_vector_count_elements(zend_object *object, zend_long *count)
{
	const teds_vector *intern = teds_vector_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get number of entries in this vector */
PHP_METHOD(Teds_Vector, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_vector *intern = Z_VECTOR_P(object);
	RETURN_LONG(intern->array.size);
}

/* Get capacity of this vector */
PHP_METHOD(Teds_Vector, capacity)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	const teds_vector *intern = Z_VECTOR_P(object);
	RETURN_LONG(intern->array.capacity);
}

/* Free elements and backing storage of this vector */
PHP_METHOD(Teds_Vector, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	if (intern->array.capacity == 0) {
		/* Nothing to clear */
		return;
	}
	/* Immediately make the original storage inaccessible and set count/capacity to 0 in case destructors modify the vector */
	const size_t old_size = intern->array.size;
	zval *const old_entries = intern->array.entries;
	teds_vector_entries_set_empty_list(&intern->array);
	teds_zval_dtor_range(old_entries, old_size);
	efree(old_entries);
}

/* Set size of this vector */
PHP_METHOD(Teds_Vector, setSize)
{
	zend_long size;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	const size_t old_size = intern->array.size;
	if (size > old_size) {
		/* Raise the capacity as needed and fill the space with nulls */
		if (size > intern->array.capacity) {
			teds_vector_raise_capacity(intern, size);
		}
		intern->array.size = size;
		zval * const entries = intern->array.entries;
		for (size_t i = old_size; i < size; i++) {
			ZVAL_NULL(&entries[i]);
		}
		return;
	}
	/* Reduce the size and invalidate memory. If a destructor unexpectedly changes the size then read the new size and keep removing elements. */
	const size_t entries_to_remove = old_size - size;
	if (entries_to_remove == 0) {
		return;
	}
	zval *const old_entries = intern->array.entries;
	zval * old_copy;
	if (size == 0) {
		old_copy = old_entries;
		teds_vector_entries_set_empty_list(&intern->array);
	} else {
		old_copy = teds_zval_copy_range(&old_entries[size], entries_to_remove);
		intern->array.size = size;

		if (size * 4 < intern->array.capacity) {
			const size_t size = old_size - 1;
			const size_t capacity = size > 2 ? size * 2 : 4;
			if (capacity < intern->array.capacity) {
				teds_vector_shrink_capacity(&intern->array, size, capacity, old_entries);
			}
		}
	}

	teds_zval_dtor_range(old_copy, entries_to_remove);
	efree(old_copy);
}

/* Create this from an optional iterable */
PHP_METHOD(Teds_Vector, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable = NULL;
	bool preserve_keys = true;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
		Z_PARAM_BOOL(preserve_keys)
	ZEND_PARSE_PARAMETERS_END();

	teds_vector *intern = Z_VECTOR_P(object);

	if (UNEXPECTED(!teds_vector_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\Vector::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}
	if (!iterable) {
		teds_vector_entries_set_empty_list(&intern->array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_vector_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable), preserve_keys);
			return;
		case IS_OBJECT:
			teds_vector_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable), preserve_keys);
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_Vector, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_vector_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void teds_vector_it_rewind(zend_object_iterator *iter)
{
	((teds_vector_it*)iter)->current = 0;
}

static int teds_vector_it_valid(zend_object_iterator *iter)
{
	teds_vector_it     *iterator = (teds_vector_it*)iter;
	teds_vector *object   = Z_VECTOR_P(&iter->data);

	if (iterator->current >= 0 && iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval *teds_vector_read_offset_helper(teds_vector *intern, size_t offset)
{
	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (UNEXPECTED(offset >= intern->array.size)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

static zval *teds_vector_it_get_current_data(zend_object_iterator *iter)
{
	teds_vector_it     *iterator = (teds_vector_it*)iter;
	teds_vector *object   = Z_VECTOR_P(&iter->data);

	zval *data = teds_vector_read_offset_helper(object, iterator->current);

	if (UNEXPECTED(data == NULL)) {
		return &EG(uninitialized_zval);
	} else {
		return data;
	}
}

static void teds_vector_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_vector_it     *iterator = (teds_vector_it*)iter;
	teds_vector *object   = Z_VECTOR_P(&iter->data);

	size_t offset = iterator->current;
	if (offset >= object->array.size) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, offset);
	}
}

static void teds_vector_it_move_forward(zend_object_iterator *iter)
{
	((teds_vector_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_vector_it_funcs = {
	teds_vector_it_dtor,
	teds_vector_it_valid,
	teds_vector_it_get_current_data,
	teds_vector_it_get_current_key,
	teds_vector_it_move_forward,
	teds_vector_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_vector_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	// This is final
	ZEND_ASSERT(ce == teds_ce_Vector);
	teds_vector_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_vector_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_vector_it_funcs;

	return &iterator->intern;
}

PHP_METHOD(Teds_Vector, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	const size_t num_entries = zend_hash_num_elements(raw_data);
	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	if (UNEXPECTED(!teds_vector_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	ZEND_ASSERT(intern->array.entries == NULL);

	zval *const entries = safe_emalloc(num_entries, sizeof(zval), 0);
	zval *it = entries;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			for (zval *deleteIt = entries; deleteIt < it; deleteIt++) {
				zval_ptr_dtor_nogc(deleteIt);
			}
			efree(entries);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\Vector::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}
		ZVAL_COPY_DEREF(it++, val);
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it == entries + num_entries);

	intern->array.size = num_entries;
	intern->array.capacity = num_entries;
	intern->array.entries = entries;
}

static void teds_vector_entries_init_from_array_values(teds_vector_entries *array, zend_array *raw_data)
{
	size_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->capacity = 0;
		array->entries = NULL;
		return;
	}
	zval *entries = safe_emalloc(num_entries, sizeof(zval), 0);
	size_t actual_size = 0;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		ZVAL_COPY_DEREF(&entries[actual_size], val);
		actual_size++;
	} ZEND_HASH_FOREACH_END();

	ZEND_ASSERT(actual_size <= num_entries);
	if (UNEXPECTED(!actual_size)) {
		efree(entries);
		entries = NULL;
		num_entries = 0;
	}

	array->entries = entries;
	array->size = actual_size;
	array->capacity = num_entries;
}

PHP_METHOD(Teds_Vector, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_vector_new(teds_ce_Vector);
	teds_vector *intern = teds_vector_from_object(object);
	teds_vector_entries_init_from_array_values(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_Vector, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);

	if (teds_vector_entries_empty_size(&intern->array)) {
		RETURN_EMPTY_ARRAY();
	}
	zval *entries = intern->array.entries;
	size_t len = intern->array.size;
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	ZEND_HASH_FILL_PACKED(flat_entries_array) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i];
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();

	RETURN_ARR(flat_entries_array);
}

PHP_METHOD(Teds_Vector, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	size_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zval *entries = intern->array.entries;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		for (size_t i = 0; i < len; i++) {
			zval *tmp = &entries[i];
			Z_TRY_ADDREF_P(tmp);
			ZEND_HASH_FILL_ADD(tmp);
		}
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

static zend_always_inline void teds_vector_get_value_at_offset(zval *return_value, const zval *zval_this, zend_long offset)
{
	teds_vector *intern = Z_VECTOR_P(zval_this);
	size_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		RETURN_THROWS();
	}
	RETURN_COPY(&intern->array.entries[offset]);
}

PHP_METHOD(Teds_Vector, valueAt)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_vector_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_Vector, offsetGet)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	teds_vector_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Teds_Vector, offsetExists)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	const teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	RETURN_BOOL((zend_ulong) offset < len);
}

PHP_METHOD(Teds_Vector, indexOf)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i])) {
			RETURN_LONG(i);
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_Vector, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	const size_t len = intern->array.size;
	zval *entries = intern->array.entries;
	for (size_t i = 0; i < len; i++) {
		if (zend_is_identical(value, &entries[i])) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

static zend_always_inline void teds_vector_set_value_at_offset(zend_object *object, zend_long offset, zval *value) {
	const teds_vector *intern = teds_vector_from_object(object);
	zval *const ptr = &intern->array.entries[offset];

	size_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		return;
	}
	zval tmp;
	ZVAL_COPY_VALUE(&tmp, ptr);
	ZVAL_COPY(ptr, value);
	zval_ptr_dtor(&tmp);
}

PHP_METHOD(Teds_Vector, setValueAt)
{
	zend_long offset;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_vector_set_value_at_offset(Z_OBJ_P(ZEND_THIS), offset, value);
}

PHP_METHOD(Teds_Vector, offsetSet)
{
	zval                  *offset_zv, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(offset_zv)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	teds_vector_set_value_at_offset(Z_OBJ_P(ZEND_THIS), offset, value);
}

PHP_METHOD(Teds_Vector, push)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	const size_t old_size = intern->array.size;
	const size_t old_capacity = intern->array.capacity;

	if (old_size >= old_capacity) {
		ZEND_ASSERT(old_size == old_capacity);
		teds_vector_raise_capacity(intern, old_size ? old_size * 2 : 4);
	}
	ZVAL_COPY(&intern->array.entries[old_size], value);
	intern->array.size++;
}

PHP_METHOD(Teds_Vector, pop)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	const size_t old_size = intern->array.size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty Teds\\Vector", 0);
		RETURN_THROWS();
	}
	const size_t old_capacity = intern->array.capacity;
	intern->array.size--;
	RETVAL_COPY_VALUE(&intern->array.entries[intern->array.size]);
	if (old_size * 4 < old_capacity) {
		/* Shrink the storage if only a quarter of the capacity is used  */
		const size_t size = old_size - 1;
		zval *old_entries = intern->array.entries;
		const size_t capacity = size > 2 ? size * 2 : 4;
		if (capacity < old_capacity) {
			teds_vector_shrink_capacity(&intern->array, size, capacity, old_entries);
		}
	}
}

PHP_METHOD(Teds_Vector, shrinkToFit)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	const size_t size = intern->array.size;
	const size_t old_capacity = intern->array.capacity;
	if (size >= old_capacity) {
		ZEND_ASSERT(size == old_capacity);
		return;
	}

	if (size == 0) {
		efree(intern->array.entries);
		intern->array.entries = (zval *)empty_entry_list;
	} else {
		intern->array.entries = safe_erealloc(intern->array.entries, size, sizeof(zval), 0);
	}
	intern->array.capacity = size;
}

PHP_METHOD(Teds_Vector, offsetUnset)
{
	zval                  *offset_zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &offset_zv) == FAILURE) {
		RETURN_THROWS();
	}
	zend_throw_exception(spl_ce_RuntimeException, "Teds\\Vector does not support offsetUnset - elements must be set to null or removed by resizing", 0);
	RETURN_THROWS();
}

static void teds_vector_return_list(zval *return_value, teds_vector *intern)
{
	teds_convert_zval_list_to_php_array_list(return_value, intern->array.entries, intern->array.size);
}

PHP_METHOD(Teds_Vector, jsonSerialize)
{
	/* json_encoder.c will always encode objects as {"0":..., "1":...}, and detects recursion if an object returns its internal property array, so we have to return a new array */
	ZEND_PARSE_PARAMETERS_NONE();
	teds_vector *intern = Z_VECTOR_P(ZEND_THIS);
	teds_vector_return_list(return_value, intern);
}

static void teds_vector_write_dimension(zend_object *object, zval *offset_zv, zval *value)
{
	if (!offset_zv) {
		zend_throw_exception(spl_ce_RuntimeException, "[] operator not supported for Teds\\Vector", 0);
		return;
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	const teds_vector *intern = teds_vector_from_object(object);
	if (offset < 0 || offset >= intern->array.size) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index invalid or out of range", 0);
		return;
	}
	ZVAL_DEREF(value);
	teds_vector_set_value_at_offset(object, offset, value);
}

static zval *teds_vector_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (!offset_zv) {
		zend_throw_exception(spl_ce_RuntimeException, "[] operator not supported for Teds\\Vector", 0);
		return NULL;
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW_RETURN_NULLPTR(offset, offset_zv);

	const teds_vector *intern = teds_vector_from_object(object);

	if (offset < 0 || offset >= intern->array.size) {
		if (type != BP_VAR_IS) {
			zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
		}
		return NULL;
	} else {
		return &intern->array.entries[offset];
	}
}

PHP_MINIT_FUNCTION(teds_vector)
{
	teds_ce_Vector = register_class_Teds_Vector(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
	teds_ce_Vector->create_object = teds_vector_new;

	memcpy(&teds_handler_Vector, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_Vector.offset          = XtOffsetOf(teds_vector, std);
	teds_handler_Vector.clone_obj       = teds_vector_clone;
	teds_handler_Vector.count_elements  = teds_vector_count_elements;
	teds_handler_Vector.get_properties  = teds_vector_get_properties;
	teds_handler_Vector.get_gc          = teds_vector_get_gc;
	teds_handler_Vector.dtor_obj        = zend_objects_destroy_object;
	teds_handler_Vector.free_obj        = teds_vector_free_storage;

	teds_handler_Vector.read_dimension  = teds_vector_read_dimension;
	teds_handler_Vector.write_dimension = teds_vector_write_dimension;
	//teds_handler_Vector.unset_dimension = teds_vector_unset_dimension;
	//teds_handler_Vector.has_dimension   = teds_vector_has_dimension;

	teds_ce_Vector->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_Vector->get_iterator = teds_vector_get_iterator;

	return SUCCESS;
}
