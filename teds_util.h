#ifndef TEDS_UTIL
#define TEDS_UTIL

#include "zend_types.h"

static inline zval *teds_zval_copy_range(const zval *original, size_t n) {
	const size_t bytes = n * sizeof(zval);
	zval *copy = emalloc(bytes);
	memcpy(copy, original, bytes);
	return copy;
}

static inline void teds_zval_dtor_range(zval *it, size_t n) {
	for (zval *end = it + n; it < end; it++) {
		zval_ptr_dtor(it);
	}
}

#endif
