/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 416a4253ca4287b2f799dd93008ce106fc1deb51 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_SortedStrictMap___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_SortedStrictMap_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_SortedStrictMap_fromPairs, 0, 1, Teds\\SortedStrictMap, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_SortedStrictMap___serialize arginfo_class_Teds_SortedStrictMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_SortedStrictMap___set_state, 0, 1, Teds\\SortedStrictMap, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_SortedStrictMap_values arginfo_class_Teds_SortedStrictMap_toPairs

#define arginfo_class_Teds_SortedStrictMap_keys arginfo_class_Teds_SortedStrictMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictMap_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_SortedStrictMap_containsKey arginfo_class_Teds_SortedStrictMap_containsValue

#define arginfo_class_Teds_SortedStrictMap_jsonSerialize arginfo_class_Teds_SortedStrictMap_toPairs


ZEND_METHOD(Teds_SortedStrictMap, __construct);
ZEND_METHOD(Teds_SortedStrictMap, getIterator);
ZEND_METHOD(Teds_SortedStrictMap, count);
ZEND_METHOD(Teds_SortedStrictMap, isEmpty);
ZEND_METHOD(Teds_SortedStrictMap, clear);
ZEND_METHOD(Teds_SortedStrictMap, toPairs);
ZEND_METHOD(Teds_SortedStrictMap, fromPairs);
ZEND_METHOD(Teds_SortedStrictMap, __serialize);
ZEND_METHOD(Teds_SortedStrictMap, __unserialize);
ZEND_METHOD(Teds_SortedStrictMap, __set_state);
ZEND_METHOD(Teds_SortedStrictMap, values);
ZEND_METHOD(Teds_SortedStrictMap, keys);
ZEND_METHOD(Teds_SortedStrictMap, offsetGet);
ZEND_METHOD(Teds_SortedStrictMap, offsetExists);
ZEND_METHOD(Teds_SortedStrictMap, offsetSet);
ZEND_METHOD(Teds_SortedStrictMap, offsetUnset);
ZEND_METHOD(Teds_SortedStrictMap, containsValue);
ZEND_METHOD(Teds_SortedStrictMap, containsKey);
ZEND_METHOD(Teds_SortedStrictMap, jsonSerialize);


static const zend_function_entry class_Teds_SortedStrictMap_methods[] = {
	ZEND_ME(Teds_SortedStrictMap, __construct, arginfo_class_Teds_SortedStrictMap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, getIterator, arginfo_class_Teds_SortedStrictMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, count, arginfo_class_Teds_SortedStrictMap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, isEmpty, arginfo_class_Teds_SortedStrictMap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, clear, arginfo_class_Teds_SortedStrictMap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, toPairs, arginfo_class_Teds_SortedStrictMap_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, fromPairs, arginfo_class_Teds_SortedStrictMap_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_SortedStrictMap, __serialize, arginfo_class_Teds_SortedStrictMap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, __unserialize, arginfo_class_Teds_SortedStrictMap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, __set_state, arginfo_class_Teds_SortedStrictMap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_SortedStrictMap, values, arginfo_class_Teds_SortedStrictMap_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, keys, arginfo_class_Teds_SortedStrictMap_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, offsetGet, arginfo_class_Teds_SortedStrictMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, offsetExists, arginfo_class_Teds_SortedStrictMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, offsetSet, arginfo_class_Teds_SortedStrictMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, offsetUnset, arginfo_class_Teds_SortedStrictMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, containsValue, arginfo_class_Teds_SortedStrictMap_containsValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, containsKey, arginfo_class_Teds_SortedStrictMap_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictMap, jsonSerialize, arginfo_class_Teds_SortedStrictMap_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_SortedStrictMap(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "SortedStrictMap", class_Teds_SortedStrictMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable, class_entry_ArrayAccess);

	return class_entry;
}
