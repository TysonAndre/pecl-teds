/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c but uses less memory and is immutable */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if PHP_VERSION_ID >= 80100
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "zend_types.h"

#include "php_teds.h"
#include "Zend/zend_enum.h"
#include "teds_emptycollection_arginfo.h"
#include "teds_emptycollection.h"
#include "teds.h"
#include "teds_util.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "teds_interfaces.h"
#include "teds_exceptions.h"
#include "teds_util.h"

#include <stdbool.h>

zend_object_handlers teds_handler_EmptySequence;
zend_class_entry *teds_ce_EmptySequence;
zend_object_handlers teds_handler_EmptyMap;
zend_class_entry *teds_ce_EmptyMap;
zend_object_handlers teds_handler_EmptySet;
zend_class_entry *teds_ce_EmptySet;

static zend_object *teds_emptysequence_new(zend_class_entry *class_type)
{
	zend_object *std = zend_object_alloc(sizeof(zend_object), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_EmptySequence);

	zend_object_std_init(std, class_type);
	object_properties_init(std, class_type);
	std->handlers = &teds_handler_EmptySequence;

	return std;
}

static void teds_emptycollection_it_noop(zend_object_iterator *iter) {
	(void)iter;
} /* noop */

static int teds_emptycollection_it_valid(zend_object_iterator *iter) {
	(void)iter;
	return FAILURE;
}

static zval *teds_emptycollection_it_get_current_data(zend_object_iterator *iter)
{
	(void)iter;
	teds_throw_invalid_sequence_index_exception();
	return &EG(uninitialized_zval);
}

static const zend_object_iterator_funcs teds_emptycollection_it_funcs = {
	teds_emptycollection_it_noop, /* dtor */
	teds_emptycollection_it_valid,
	teds_emptycollection_it_get_current_data,
	NULL, /* get_current_key is optional */
	teds_emptycollection_it_noop, /* move_forward */
	NULL, /* rewind is optional */
	NULL,
	NULL, /* get_gc */
};
static zend_object_iterator *teds_emptycollection_get_iterator(zend_class_entry *ce, zval *object, int byref)
{
	(void)ce;
	(void)object;
	if (UNEXPECTED(byref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	zend_object_iterator *iterator = emalloc(sizeof(zend_object_iterator));
	zend_iterator_init(iterator);
	iterator->funcs = &teds_emptycollection_it_funcs;
	return iterator;
}

static zend_object *teds_emptymap_new(zend_class_entry *class_type)
{
	zend_object *std = zend_object_alloc(sizeof(zend_object), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_EmptyMap);

	zend_object_std_init(std, class_type);
	object_properties_init(std, class_type);
	std->handlers = &teds_handler_EmptyMap;

	return std;
}

static zend_object *teds_emptyset_new(zend_class_entry *class_type)
{
	zend_object *std = zend_object_alloc(sizeof(zend_object), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_EmptySet);

	zend_object_std_init(std, class_type);
	object_properties_init(std, class_type);
	std->handlers = &teds_handler_EmptySet;

	return std;
}

static int teds_emptysequence_count_elements(zend_object *object, zend_long *count)
{
	(void)object;
	*count = 0;
	return SUCCESS;
}

/* Get number of entries in this EmptySequence */
PHP_METHOD(Teds_EmptySequence, count)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(0);
}

/* Get whether this EmptySequence is empty */
PHP_METHOD(Teds_EmptySequence, isEmpty)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_TRUE;
}

/* Get whether this EmptySequence iterator is valid. Always false. */
PHP_METHOD(Teds_EmptySequence, valid)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_FALSE;
}

PHP_METHOD(Teds_EmptySequence, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_EMPTY_ARRAY();
}

ZEND_COLD PHP_METHOD(Teds_EmptySequence, get)
{
	zend_long offset;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	teds_throw_invalid_sequence_index_exception();
	RETURN_THROWS();
}

static void teds_throw_mutate_empty_collection_exception(void)
{
	teds_throw_unsupportedoperationexception("Cannot mutate an immutable empty collection");
}
#define TEDS_THROW_MUTATE_EMPTY_COLLECTION_EXCEPTION() do { teds_throw_mutate_empty_collection_exception(); RETURN_THROWS(); } while (0)

ZEND_COLD PHP_METHOD(Teds_EmptySequence, set)
{
	zend_long offset;
	zval *tmp;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &offset, &tmp) == FAILURE) {
		RETURN_THROWS();
	}
	TEDS_THROW_MUTATE_EMPTY_COLLECTION_EXCEPTION();
}

ZEND_COLD PHP_METHOD(Teds_EmptySequence, push)
{
	zval *tmp;
	int num_varargs;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &tmp, &num_varargs) == FAILURE) {
		RETURN_THROWS();
	}

	TEDS_THROW_MUTATE_EMPTY_COLLECTION_EXCEPTION();
}

ZEND_COLD PHP_METHOD(Teds_EmptySequence, insert)
{
	zval offset;
	zval *tmp;
	int num_varargs;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l*", &offset, &tmp, &num_varargs) == FAILURE) {
		RETURN_THROWS();
	}

	TEDS_THROW_MUTATE_EMPTY_COLLECTION_EXCEPTION();
}

ZEND_COLD PHP_METHOD(Teds_EmptySequence, pop)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	TEDS_THROW_MUTATE_EMPTY_COLLECTION_EXCEPTION();
}

ZEND_COLD PHP_METHOD(Teds_EmptySequence, first)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	TEDS_THROW_INVALID_SEQUENCE_INDEX_EXCEPTION();
}

PHP_METHOD(Teds_EmptySequence, offsetGet)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) {
		teds_get_offset(offset_zv); /* Throws for invalid type */
		RETURN_THROWS();
	}
	teds_throw_invalid_sequence_index_exception();
}

PHP_METHOD(Teds_EmptyMap, offsetGet)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_NULL();
}

PHP_METHOD(Teds_EmptyMap, offsetUnset)
{
	zval *zindex;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Teds_EmptyMap, get)
{
	zval *key;
	zval *default_zv = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_zv)
	ZEND_PARSE_PARAMETERS_END();
	if (default_zv != NULL) {
		RETURN_COPY(default_zv);
	}
	TEDS_THROW_MISSING_MAP_KEY_EXCEPTION();
}

PHP_METHOD(Teds_EmptySequence, offsetExists)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) {
		teds_get_offset(offset_zv); /* Throws for invalid type */
		RETURN_THROWS();
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_EmptySequence, indexOf)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_NULL();
}

PHP_METHOD(Teds_EmptySequence, filter)
{
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(Teds_EmptySequence, map)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(Teds_EmptySequence, contains)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_FALSE;
}

ZEND_COLD PHP_METHOD(Teds_EmptySequence, offsetSet)
{
	zval                  *zindex, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		RETURN_THROWS();
	}
	TEDS_THROW_MUTATE_EMPTY_COLLECTION_EXCEPTION();
}

ZEND_COLD PHP_METHOD(Teds_EmptySequence, offsetUnset)
{
	zval                  *offset_zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &offset_zv) == FAILURE) {
		RETURN_THROWS();
	}

	TEDS_THROW_MUTATE_EMPTY_COLLECTION_EXCEPTION();
}

static zval *teds_emptysequence_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (UNEXPECTED(!offset_zv || Z_ISUNDEF_P(offset_zv))) {
handle_missing_key:
		if (type != BP_VAR_IS) {
			teds_throw_invalid_sequence_index_exception();
			return NULL;
		}
		return &EG(uninitialized_zval);
	}
	THROW_IF_OFFSET_NOT_LONG(offset_zv);
	(void)rv;
	(void)object;

	if (UNEXPECTED(type != BP_VAR_IS && type != BP_VAR_R)) {
		teds_throw_mutate_empty_collection_exception();
		return NULL;
	}

	goto handle_missing_key;
}

static zval *teds_emptymap_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (UNEXPECTED(!offset_zv || Z_ISUNDEF_P(offset_zv))) {
handle_missing_key:
		if (type != BP_VAR_IS) {
			teds_throw_missing_map_key_exception();
			return NULL;
		}
		return &EG(uninitialized_zval);
	}
	(void)rv;
	(void)object;

	if (UNEXPECTED(type != BP_VAR_IS && type != BP_VAR_R)) {
		teds_throw_mutate_empty_collection_exception();
		return NULL;
	}

	goto handle_missing_key;
}

static int teds_emptysequence_has_dimension(zend_object *object, zval *offset_zv, int check_empty)
{
	if (UNEXPECTED(Z_TYPE_P(offset_zv) != IS_LONG)) {
		teds_get_offset(offset_zv); /* Throws as side effect */
	}
	(void)check_empty;
	(void)object;

	return 0;
}

static int teds_emptymap_has_dimension(zend_object *object, zval *offset_zv, int check_empty)
{
	(void)check_empty;
	(void)offset_zv;
	(void)object;

	return 0;
}

static void teds_handlers_empty_enum_init_common(zend_object_handlers *handlers)
{
	// Based on static function zend_register_enum_ce
	memcpy(handlers, &std_object_handlers, sizeof(zend_object_handlers));
	handlers->clone_obj       = NULL;
	handlers->offset          = 0;
	handlers->compare         = zend_objects_not_comparable;
	handlers->count_elements  = teds_emptysequence_count_elements;
	handlers->get_properties  = teds_noop_empty_array_get_properties;
	handlers->get_gc          = teds_noop_get_gc;
}

PHP_MINIT_FUNCTION(teds_emptycollection)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_EmptySequence = register_class_Teds_EmptySequence(zend_ce_iterator, teds_ce_Sequence, php_json_serializable_ce);
	teds_ce_EmptySequence->create_object = teds_emptysequence_new;
#if PHP_VERSION_ID >= 80300
	teds_ce_EmptySequence->default_object_handlers = &teds_handler_EmptySequence;
#endif

	teds_handlers_empty_enum_init_common(&teds_handler_EmptySequence);
	teds_handler_EmptySequence.read_dimension  = teds_emptysequence_read_dimension;
	teds_handler_EmptySequence.has_dimension   = teds_emptysequence_has_dimension;

	teds_ce_EmptySequence->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_EmptySequence->get_iterator = teds_emptycollection_get_iterator;

	/* EmptyMap */
	teds_ce_EmptyMap = register_class_Teds_EmptyMap(zend_ce_iterator, teds_ce_Map, php_json_serializable_ce);
	teds_ce_EmptyMap->create_object = teds_emptymap_new;
#if PHP_VERSION_ID >= 80300
	teds_ce_EmptyMap->default_object_handlers = &teds_handler_EmptyMap;
#endif

	memcpy(&teds_handler_EmptyMap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handlers_empty_enum_init_common(&teds_handler_EmptyMap);
	teds_handler_EmptyMap.read_dimension  = teds_emptymap_read_dimension;
	teds_handler_EmptyMap.has_dimension   = teds_emptymap_has_dimension;

	teds_ce_EmptyMap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_EmptyMap->get_iterator = teds_emptycollection_get_iterator;

	/* EmptySet */
	teds_ce_EmptySet = register_class_Teds_EmptySet(zend_ce_iterator, teds_ce_Set, php_json_serializable_ce);
	teds_ce_EmptySet->create_object = teds_emptyset_new;
#if PHP_VERSION_ID >= 80300
	teds_ce_EmptySet->default_object_handlers = &teds_handler_EmptySet;
#endif

	teds_handlers_empty_enum_init_common(&teds_handler_EmptySet);

	teds_ce_EmptySet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_EmptySet->get_iterator = teds_emptycollection_get_iterator;
	return SUCCESS;
}
#endif /* PHP_VERSION_ID >= 80100 */
