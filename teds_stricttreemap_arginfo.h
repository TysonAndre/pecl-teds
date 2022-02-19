/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d1bef3adc48598684a70bee5613088b1feea1c6b */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictTreeMap___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictTreeMap_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeMap_toArray arginfo_class_Teds_StrictTreeMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictTreeMap_fromPairs, 0, 1, Teds\\StrictTreeMap, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeMap___serialize arginfo_class_Teds_StrictTreeMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictTreeMap___set_state, 0, 1, Teds\\StrictTreeMap, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeMap_values arginfo_class_Teds_StrictTreeMap_toPairs

#define arginfo_class_Teds_StrictTreeMap_keys arginfo_class_Teds_StrictTreeMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_first, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeMap_bottom arginfo_class_Teds_StrictTreeMap_first

#define arginfo_class_Teds_StrictTreeMap_firstKey arginfo_class_Teds_StrictTreeMap_first

#define arginfo_class_Teds_StrictTreeMap_bottomKey arginfo_class_Teds_StrictTreeMap_first

#define arginfo_class_Teds_StrictTreeMap_last arginfo_class_Teds_StrictTreeMap_first

#define arginfo_class_Teds_StrictTreeMap_top arginfo_class_Teds_StrictTreeMap_first

#define arginfo_class_Teds_StrictTreeMap_lastKey arginfo_class_Teds_StrictTreeMap_first

#define arginfo_class_Teds_StrictTreeMap_topKey arginfo_class_Teds_StrictTreeMap_first

#define arginfo_class_Teds_StrictTreeMap_pop arginfo_class_Teds_StrictTreeMap_toPairs

#define arginfo_class_Teds_StrictTreeMap_shift arginfo_class_Teds_StrictTreeMap_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeMap_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeMap_contains arginfo_class_Teds_StrictTreeMap_containsValue

#define arginfo_class_Teds_StrictTreeMap_containsKey arginfo_class_Teds_StrictTreeMap_offsetExists

#define arginfo_class_Teds_StrictTreeMap_jsonSerialize arginfo_class_Teds_StrictTreeMap_toPairs

#define arginfo_class_Teds_StrictTreeMap_debugGetTreeRepresentation arginfo_class_Teds_StrictTreeMap_toPairs

#define arginfo_class_Teds_StrictTreeMap_debugIsBalanced arginfo_class_Teds_StrictTreeMap_isEmpty


ZEND_METHOD(Teds_StrictTreeMap, __construct);
ZEND_METHOD(Teds_StrictTreeMap, getIterator);
ZEND_METHOD(Teds_StrictTreeMap, count);
ZEND_METHOD(Teds_StrictTreeMap, isEmpty);
ZEND_METHOD(Teds_StrictTreeMap, clear);
ZEND_METHOD(Teds_StrictTreeMap, toPairs);
ZEND_METHOD(Teds_StrictTreeMap, toArray);
ZEND_METHOD(Teds_StrictTreeMap, fromPairs);
ZEND_METHOD(Teds_StrictTreeMap, __serialize);
ZEND_METHOD(Teds_StrictTreeMap, __unserialize);
ZEND_METHOD(Teds_StrictTreeMap, __set_state);
ZEND_METHOD(Teds_StrictTreeMap, values);
ZEND_METHOD(Teds_StrictTreeMap, keys);
ZEND_METHOD(Teds_StrictTreeMap, first);
ZEND_METHOD(Teds_StrictTreeMap, firstKey);
ZEND_METHOD(Teds_StrictTreeMap, last);
ZEND_METHOD(Teds_StrictTreeMap, lastKey);
ZEND_METHOD(Teds_StrictTreeMap, pop);
ZEND_METHOD(Teds_StrictTreeMap, shift);
ZEND_METHOD(Teds_StrictTreeMap, offsetGet);
ZEND_METHOD(Teds_StrictTreeMap, offsetExists);
ZEND_METHOD(Teds_StrictTreeMap, offsetSet);
ZEND_METHOD(Teds_StrictTreeMap, offsetUnset);
ZEND_METHOD(Teds_StrictTreeMap, get);
ZEND_METHOD(Teds_StrictTreeMap, contains);
ZEND_METHOD(Teds_StrictTreeMap, containsKey);
ZEND_METHOD(Teds_StrictTreeMap, debugGetTreeRepresentation);
ZEND_METHOD(Teds_StrictTreeMap, debugIsBalanced);


static const zend_function_entry class_Teds_StrictTreeMap_methods[] = {
	ZEND_ME(Teds_StrictTreeMap, __construct, arginfo_class_Teds_StrictTreeMap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, getIterator, arginfo_class_Teds_StrictTreeMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, count, arginfo_class_Teds_StrictTreeMap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, isEmpty, arginfo_class_Teds_StrictTreeMap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, clear, arginfo_class_Teds_StrictTreeMap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, toPairs, arginfo_class_Teds_StrictTreeMap_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, toArray, arginfo_class_Teds_StrictTreeMap_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, fromPairs, arginfo_class_Teds_StrictTreeMap_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictTreeMap, __serialize, arginfo_class_Teds_StrictTreeMap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, __unserialize, arginfo_class_Teds_StrictTreeMap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, __set_state, arginfo_class_Teds_StrictTreeMap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictTreeMap, values, arginfo_class_Teds_StrictTreeMap_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, keys, arginfo_class_Teds_StrictTreeMap_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, first, arginfo_class_Teds_StrictTreeMap_first, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeMap, bottom, first, arginfo_class_Teds_StrictTreeMap_bottom, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, firstKey, arginfo_class_Teds_StrictTreeMap_firstKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeMap, bottomKey, firstKey, arginfo_class_Teds_StrictTreeMap_bottomKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, last, arginfo_class_Teds_StrictTreeMap_last, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeMap, top, last, arginfo_class_Teds_StrictTreeMap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, lastKey, arginfo_class_Teds_StrictTreeMap_lastKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeMap, topKey, lastKey, arginfo_class_Teds_StrictTreeMap_topKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, pop, arginfo_class_Teds_StrictTreeMap_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, shift, arginfo_class_Teds_StrictTreeMap_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, offsetGet, arginfo_class_Teds_StrictTreeMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, offsetExists, arginfo_class_Teds_StrictTreeMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, offsetSet, arginfo_class_Teds_StrictTreeMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, offsetUnset, arginfo_class_Teds_StrictTreeMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, get, arginfo_class_Teds_StrictTreeMap_get, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeMap, containsValue, contains, arginfo_class_Teds_StrictTreeMap_containsValue, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(Teds_StrictTreeMap, contains, arginfo_class_Teds_StrictTreeMap_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, containsKey, arginfo_class_Teds_StrictTreeMap_containsKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeMap, jsonSerialize, toPairs, arginfo_class_Teds_StrictTreeMap_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, debugGetTreeRepresentation, arginfo_class_Teds_StrictTreeMap_debugGetTreeRepresentation, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeMap, debugIsBalanced, arginfo_class_Teds_StrictTreeMap_debugIsBalanced, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictTreeMap(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Map, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictTreeMap", class_Teds_StrictTreeMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Map, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\StableSortedMap", class_entry);

	return class_entry;
}
