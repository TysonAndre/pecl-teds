/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STRICTSET_H
#define TEDS_STRICTSET_H

#include "Zend/zend_types.h"

PHP_MINIT_FUNCTION(teds_strictset);

typedef struct _teds_strictset_entry {
	zval key;   /* TODO: Stores Z_NEXT - similar to https://github.com/php/php-src/pull/6588 */
	uint64_t hash;
} teds_strictset_entry;

typedef struct _teds_strictset_entries {
	size_t size;
	size_t capacity;
	teds_strictset_entry *entries;
} teds_strictset_entries;

typedef struct _teds_strictset {
	teds_strictset_entries		array;
	zend_object				std;
} teds_strictset;

void teds_strictset_entries_init_from_array(teds_strictset_entries *array, zend_array *values);

void teds_strictset_entries_init_from_traversable(teds_strictset_entries *array, zend_object *obj);

teds_strictset_entry *teds_strictset_entries_find_key(const teds_strictset_entries *array, zval *key, uint64_t hash);

void teds_strictset_entries_dtor(teds_strictset_entries *array);

void teds_strictset_entries_dtor_range(teds_strictset_entry *start, size_t from, size_t to);

#endif	/* TEDS_STRICTSET_H */
