/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/
#ifndef TEDS_EXCEPTIONS_H
#define TEDS_EXCEPTIONS_H

#include "Zend/zend_interfaces.h"
#include "php.h"
#include "Zend/zend_portability.h"
#include "Zend/zend_types.h"

extern zend_class_entry *teds_ce_UnsupportedOperationException;

void teds_throw_unsupportedoperationexception(const char *message);
void teds_throw_invalid_sequence_index_exception(void);
void teds_throw_invalid_collection_offset_exception(void);
void teds_throw_missing_map_key_exception(void);
#define TEDS_THROW_UNSUPPORTEDOPERATIONEXCEPTION(message) do { teds_throw_unsupportedoperationexception((message)); RETURN_THROWS(); } while (0)
#define TEDS_THROW_INVALID_SEQUENCE_INDEX_EXCEPTION() do { teds_throw_invalid_sequence_index_exception(); RETURN_THROWS(); } while (0)
#define TEDS_THROW_INVALID_COLLECTION_OFFSET_EXCEPTION() do { teds_throw_invalid_collection_offset_exception(); RETURN_THROWS(); } while (0)
#define TEDS_THROW_MISSING_MAP_KEY_EXCEPTION() do { teds_throw_missing_map_key_exception(); RETURN_THROWS(); } while (0)

void teds_register_exceptions(void);
#endif
