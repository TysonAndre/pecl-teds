/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_INTVECTOR_H
#define TEDS_INTVECTOR_H

extern zend_class_entry *teds_ce_IntVector;

PHP_MINIT_FUNCTION(teds_intvector);

/* TODO reuse in LowMemoryVector */
#define TEDS_PROMOTE_INT_TYPE_AND_RETURN(int_smaller, entries_smaller, int_larger, entries_larger, TEDS_INTVECTOR__TYPE_LARGER) do { \
		ZEND_ASSERT(sizeof(int_smaller) < sizeof(int_larger)); \
		array->type_tag = TEDS_INTVECTOR__TYPE_LARGER; \
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
		} \
		return; \
	} while (0)

#endif	/* TEDS_INTVECTOR_H */
