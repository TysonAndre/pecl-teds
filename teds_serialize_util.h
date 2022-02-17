#ifndef TEDS_SERIALIZE_UTIL_H
#define TEDS_SERIALIZE_UTIL_H

#include "Zend/zend_types.h"
#include "Zend/zend_hash.h"
#include "Zend/zend_portability.h"

static zend_always_inline HashTable *teds_create_serialize_pair(const uint8_t type, zval *data) {
	zval type_zv;
	ZVAL_LONG(&type_zv, type);
	return zend_new_pair(&type_zv, data);
}

#endif
