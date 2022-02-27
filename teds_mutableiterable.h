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

extern zend_class_entry *teds_ce_MutableIterable;

PHP_MINIT_FUNCTION(teds_mutableiterable);

typedef struct _zval_pair {
	zval key;
	zval value;
} zval_pair;

typedef struct _teds_mutableiterable_entries {
	zval_pair *entries;
	uint32_t size;
	uint32_t capacity;
} teds_mutableiterable_entries;

typedef struct _teds_mutableiterable {
	teds_mutableiterable_entries array;
	zend_object                 std;
} teds_mutableiterable;

HashTable* teds_mutableiterable_get_gc(zend_object *obj, zval **table, int *n);
HashTable* teds_mutableiterable_get_properties(zend_object *obj);
int teds_mutableiterable_count_elements(zend_object *object, zend_long *count);
void teds_mutableiterable_free_storage(zend_object *object);
void teds_mutableiterable_clear(teds_mutableiterable *intern);
zend_object_iterator *teds_mutableiterable_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

#endif	/* TEDS_MUTABLEITERABLE_H */
