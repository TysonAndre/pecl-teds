/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_MUTABLEITERABLE_H
#define TEDS_MUTABLEITERABLE_H

#include "teds.h"
#include "teds_zval_pair.h"
#include "teds_internaliterator.h"

extern zend_class_entry *teds_ce_MutableIterable;

PHP_MINIT_FUNCTION(teds_mutableiterable);

typedef struct _teds_mutableiterable_entries {
	zval_pair *entries;
	uint32_t size;
	uint32_t capacity;
	teds_intrusive_dllist active_iterators;
} teds_mutableiterable_entries;

typedef struct _teds_mutableiterable {
	teds_mutableiterable_entries array;
	zend_object                 std;
} teds_mutableiterable;

HashTable* teds_mutableiterable_get_gc(zend_object *obj, zval **table, int *n);
HashTable* teds_mutableiterable_get_properties_for(zend_object *obj, zend_prop_purpose purpose);
TEDS_COUNT_ELEMENTS_RETURN_TYPE teds_mutableiterable_count_elements(zend_object *object, zend_long *count);
void teds_mutableiterable_free_storage(zend_object *object);
void teds_mutableiterable_clear(teds_mutableiterable *intern);
zend_object_iterator *teds_mutableiterable_get_iterator(zend_class_entry *ce, zval *object, int by_ref);
/* Used by InternalIterator returned by MutableIterable->getIterator() */
typedef struct _teds_mutableiterable_it {
	zend_object_iterator intern;
	teds_intrusive_dllist_node dllist_node;
	uint32_t             current;
} teds_mutableiterable_it;

void teds_mutableiterable_it_dtor(zend_object_iterator *iter);
void teds_mutableiterable_it_rewind(zend_object_iterator *iter);
int teds_mutableiterable_it_valid(zend_object_iterator *iter);

#endif	/* TEDS_MUTABLEITERABLE_H */
