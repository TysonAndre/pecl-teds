/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b78acbb53e9e33a5c7551e0c802741d7a156fb2a */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictMap___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictMap_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictMap_fromPairs, 0, 1, Teds\\StrictMap, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMap___serialize arginfo_class_Teds_StrictMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictMap___set_state, 0, 1, Teds\\StrictMap, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMap_toArray arginfo_class_Teds_StrictMap_toPairs

#define arginfo_class_Teds_StrictMap_values arginfo_class_Teds_StrictMap_toPairs

#define arginfo_class_Teds_StrictMap_keys arginfo_class_Teds_StrictMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMap_contains arginfo_class_Teds_StrictMap_containsValue

#define arginfo_class_Teds_StrictMap_containsKey arginfo_class_Teds_StrictMap_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMap_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMap_jsonSerialize arginfo_class_Teds_StrictMap_toPairs


ZEND_METHOD(Teds_StrictMap, __construct);
ZEND_METHOD(Teds_StrictMap, getIterator);
ZEND_METHOD(Teds_StrictMap, count);
ZEND_METHOD(Teds_StrictMap, isEmpty);
ZEND_METHOD(Teds_StrictMap, clear);
ZEND_METHOD(Teds_StrictMap, toPairs);
ZEND_METHOD(Teds_StrictMap, fromPairs);
ZEND_METHOD(Teds_StrictMap, __serialize);
ZEND_METHOD(Teds_StrictMap, __unserialize);
ZEND_METHOD(Teds_StrictMap, __set_state);
ZEND_METHOD(Teds_StrictMap, toArray);
ZEND_METHOD(Teds_StrictMap, values);
ZEND_METHOD(Teds_StrictMap, keys);
ZEND_METHOD(Teds_StrictMap, offsetGet);
ZEND_METHOD(Teds_StrictMap, offsetExists);
ZEND_METHOD(Teds_StrictMap, offsetSet);
ZEND_METHOD(Teds_StrictMap, offsetUnset);
ZEND_METHOD(Teds_StrictMap, contains);
ZEND_METHOD(Teds_StrictMap, containsKey);
ZEND_METHOD(Teds_StrictMap, get);


static const zend_function_entry class_Teds_StrictMap_methods[] = {
	ZEND_ME(Teds_StrictMap, __construct, arginfo_class_Teds_StrictMap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, getIterator, arginfo_class_Teds_StrictMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, count, arginfo_class_Teds_StrictMap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, isEmpty, arginfo_class_Teds_StrictMap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, clear, arginfo_class_Teds_StrictMap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, toPairs, arginfo_class_Teds_StrictMap_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, fromPairs, arginfo_class_Teds_StrictMap_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictMap, __serialize, arginfo_class_Teds_StrictMap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, __unserialize, arginfo_class_Teds_StrictMap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, __set_state, arginfo_class_Teds_StrictMap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictMap, toArray, arginfo_class_Teds_StrictMap_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, values, arginfo_class_Teds_StrictMap_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, keys, arginfo_class_Teds_StrictMap_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, offsetGet, arginfo_class_Teds_StrictMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, offsetExists, arginfo_class_Teds_StrictMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, offsetSet, arginfo_class_Teds_StrictMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, offsetUnset, arginfo_class_Teds_StrictMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMap, containsValue, contains, arginfo_class_Teds_StrictMap_containsValue, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(Teds_StrictMap, contains, arginfo_class_Teds_StrictMap_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, containsKey, arginfo_class_Teds_StrictMap_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMap, get, arginfo_class_Teds_StrictMap_get, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMap, jsonSerialize, toPairs, arginfo_class_Teds_StrictMap_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictMap(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Collection, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictMap", class_Teds_StrictMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Collection, class_entry_JsonSerializable);

	return class_entry;
}
