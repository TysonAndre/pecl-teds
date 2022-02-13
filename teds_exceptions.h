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
#define TEDS_THROW_UNSUPPORTEDOPERATIONEXCEPTION(message) do { teds_throw_unsupportedoperationexception((message)); RETURN_THROWS(); } while (0)

void teds_register_exceptions(void);
#endif
