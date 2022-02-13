/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 9d55a9035fd14c15208eff5a795140a197dd9641 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictHashMap___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictHashMap_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictHashMap_fromPairs, 0, 1, Teds\\StrictHashMap, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictHashMap___serialize arginfo_class_Teds_StrictHashMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictHashMap___set_state, 0, 1, Teds\\StrictHashMap, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictHashMap_toArray arginfo_class_Teds_StrictHashMap_toPairs

#define arginfo_class_Teds_StrictHashMap_values arginfo_class_Teds_StrictHashMap_toPairs

#define arginfo_class_Teds_StrictHashMap_keys arginfo_class_Teds_StrictHashMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictHashMap_contains arginfo_class_Teds_StrictHashMap_containsValue

#define arginfo_class_Teds_StrictHashMap_containsKey arginfo_class_Teds_StrictHashMap_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashMap_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictHashMap_jsonSerialize arginfo_class_Teds_StrictHashMap_toPairs


ZEND_METHOD(Teds_StrictHashMap, __construct);
ZEND_METHOD(Teds_StrictHashMap, getIterator);
ZEND_METHOD(Teds_StrictHashMap, count);
ZEND_METHOD(Teds_StrictHashMap, isEmpty);
ZEND_METHOD(Teds_StrictHashMap, clear);
ZEND_METHOD(Teds_StrictHashMap, toPairs);
ZEND_METHOD(Teds_StrictHashMap, fromPairs);
ZEND_METHOD(Teds_StrictHashMap, __serialize);
ZEND_METHOD(Teds_StrictHashMap, __unserialize);
ZEND_METHOD(Teds_StrictHashMap, __set_state);
ZEND_METHOD(Teds_StrictHashMap, toArray);
ZEND_METHOD(Teds_StrictHashMap, values);
ZEND_METHOD(Teds_StrictHashMap, keys);
ZEND_METHOD(Teds_StrictHashMap, offsetGet);
ZEND_METHOD(Teds_StrictHashMap, offsetExists);
ZEND_METHOD(Teds_StrictHashMap, offsetSet);
ZEND_METHOD(Teds_StrictHashMap, offsetUnset);
ZEND_METHOD(Teds_StrictHashMap, contains);
ZEND_METHOD(Teds_StrictHashMap, containsKey);
ZEND_METHOD(Teds_StrictHashMap, get);


static const zend_function_entry class_Teds_StrictHashMap_methods[] = {
	ZEND_ME(Teds_StrictHashMap, __construct, arginfo_class_Teds_StrictHashMap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, getIterator, arginfo_class_Teds_StrictHashMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, count, arginfo_class_Teds_StrictHashMap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, isEmpty, arginfo_class_Teds_StrictHashMap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, clear, arginfo_class_Teds_StrictHashMap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, toPairs, arginfo_class_Teds_StrictHashMap_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, fromPairs, arginfo_class_Teds_StrictHashMap_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictHashMap, __serialize, arginfo_class_Teds_StrictHashMap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, __unserialize, arginfo_class_Teds_StrictHashMap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, __set_state, arginfo_class_Teds_StrictHashMap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictHashMap, toArray, arginfo_class_Teds_StrictHashMap_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, values, arginfo_class_Teds_StrictHashMap_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, keys, arginfo_class_Teds_StrictHashMap_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, offsetGet, arginfo_class_Teds_StrictHashMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, offsetExists, arginfo_class_Teds_StrictHashMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, offsetSet, arginfo_class_Teds_StrictHashMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, offsetUnset, arginfo_class_Teds_StrictHashMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictHashMap, containsValue, contains, arginfo_class_Teds_StrictHashMap_containsValue, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(Teds_StrictHashMap, contains, arginfo_class_Teds_StrictHashMap_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, containsKey, arginfo_class_Teds_StrictHashMap_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashMap, get, arginfo_class_Teds_StrictHashMap_get, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictHashMap, jsonSerialize, toPairs, arginfo_class_Teds_StrictHashMap_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictHashMap(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Map, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictHashMap", class_Teds_StrictHashMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Map, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\StrictMap", class_entry);

	return class_entry;
}
