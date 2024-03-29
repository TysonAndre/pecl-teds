/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_VECTOR_H
#define TEDS_VECTOR_H

#include "teds.h"
#include "teds_internaliterator.h"

extern zend_class_entry *teds_ce_Vector;

PHP_MINIT_FUNCTION(teds_vector);

typedef struct _teds_vector_entries {
	zval *entries;
	uint32_t size;
	uint32_t capacity;
	teds_intrusive_dllist active_iterators;
#if PHP_VERSION_ID < 80300
	bool should_rebuild_properties;
#endif
} teds_vector_entries;

typedef struct _teds_vector {
	teds_vector_entries		array;
	zend_object				std;
} teds_vector;

/* Used by InternalIterator returned by Vector->getIterator() */
typedef struct _teds_vector_it {
	zend_object_iterator intern;
	uint32_t             current;
	teds_intrusive_dllist_node dllist_node;
} teds_vector_it;

void teds_vector_entries_raise_capacity(teds_vector_entries *array, const size_t new_capacity);
TEDS_COUNT_ELEMENTS_RETURN_TYPE teds_vector_count_elements(zend_object *object, zend_long *count);
void teds_vector_clear(teds_vector *intern);
HashTable* teds_vector_get_properties_for(zend_object *obj, zend_prop_purpose purpose);
HashTable* teds_vector_get_gc(zend_object *obj, zval **table, int *n);
void teds_vector_free_storage(zend_object *object);
zend_object_iterator *teds_vector_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

void teds_vector_adjust_iterators_before_remove(teds_vector_entries *array, teds_intrusive_dllist_node *node, const uint32_t removed_offset);
static zend_always_inline void teds_vector_maybe_adjust_iterators_before_remove(teds_vector_entries *array, const uint32_t removed_offset)
{
	if (UNEXPECTED(array->active_iterators.first)) {
		teds_vector_adjust_iterators_before_remove(array, array->active_iterators.first, removed_offset);
	}
}
void teds_vector_adjust_iterators_before_insert(teds_vector_entries *const array, teds_intrusive_dllist_node *node, const uint32_t inserted_offset, const uint32_t n);
static zend_always_inline void teds_vector_maybe_adjust_iterators_before_insert(teds_vector_entries *const array, const uint32_t inserted_offset, const uint32_t n)
{
	ZEND_ASSERT(inserted_offset <= array->size);
	if (UNEXPECTED(array->active_iterators.first)) {
		teds_vector_adjust_iterators_before_insert(array, array->active_iterators.first, inserted_offset, n);
	}
}

void teds_vector_it_dtor(zend_object_iterator *iter);
void teds_vector_it_rewind(zend_object_iterator *iter);
int teds_vector_it_valid(zend_object_iterator *iter);
void teds_vector_it_move_forward(zend_object_iterator *iter);
zval *teds_vector_it_get_current_data(zend_object_iterator *iter);

#endif	/* TEDS_VECTOR_H */
