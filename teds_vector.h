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

#include "teds_internaliterator.h"

extern zend_class_entry *teds_ce_Vector;

PHP_MINIT_FUNCTION(teds_vector);

typedef struct _teds_vector_entries {
	zval *entries;
	uint32_t size;
	uint32_t capacity;
	teds_intrusive_dllist active_iterators;
	bool should_rebuild_properties;
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
int teds_vector_count_elements(zend_object *object, zend_long *count);
void teds_vector_clear(teds_vector *intern);
HashTable* teds_vector_get_properties_for(zend_object *obj, zend_prop_purpose purpose);
HashTable* teds_vector_get_gc(zend_object *obj, zval **table, int *n);
void teds_vector_free_storage(zend_object *object);
zend_object_iterator *teds_vector_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

#endif	/* TEDS_VECTOR_H */
