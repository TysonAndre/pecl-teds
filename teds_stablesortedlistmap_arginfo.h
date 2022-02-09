/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a5116b0b093840c0049dc2e05b40b8d9a30c40f0 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StableSortedListMap___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StableSortedListMap_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StableSortedListMap_fromPairs, 0, 1, Teds\\StableSortedListMap, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableSortedListMap___serialize arginfo_class_Teds_StableSortedListMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StableSortedListMap___set_state, 0, 1, Teds\\StableSortedListMap, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableSortedListMap_toArray arginfo_class_Teds_StableSortedListMap_toPairs

#define arginfo_class_Teds_StableSortedListMap_values arginfo_class_Teds_StableSortedListMap_toPairs

#define arginfo_class_Teds_StableSortedListMap_keys arginfo_class_Teds_StableSortedListMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_bottom, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableSortedListMap_bottomKey arginfo_class_Teds_StableSortedListMap_bottom

#define arginfo_class_Teds_StableSortedListMap_top arginfo_class_Teds_StableSortedListMap_bottom

#define arginfo_class_Teds_StableSortedListMap_topKey arginfo_class_Teds_StableSortedListMap_bottom

#define arginfo_class_Teds_StableSortedListMap_pop arginfo_class_Teds_StableSortedListMap_toPairs

#define arginfo_class_Teds_StableSortedListMap_shift arginfo_class_Teds_StableSortedListMap_bottom

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListMap_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableSortedListMap_containsKey arginfo_class_Teds_StableSortedListMap_containsValue

#define arginfo_class_Teds_StableSortedListMap_jsonSerialize arginfo_class_Teds_StableSortedListMap_toPairs


ZEND_METHOD(Teds_StableSortedListMap, __construct);
ZEND_METHOD(Teds_StableSortedListMap, getIterator);
ZEND_METHOD(Teds_StableSortedListMap, count);
ZEND_METHOD(Teds_StableSortedListMap, isEmpty);
ZEND_METHOD(Teds_StableSortedListMap, clear);
ZEND_METHOD(Teds_StableSortedListMap, toPairs);
ZEND_METHOD(Teds_StableSortedListMap, fromPairs);
ZEND_METHOD(Teds_StableSortedListMap, __serialize);
ZEND_METHOD(Teds_StableSortedListMap, __unserialize);
ZEND_METHOD(Teds_StableSortedListMap, __set_state);
ZEND_METHOD(Teds_StableSortedListMap, values);
ZEND_METHOD(Teds_StableSortedListMap, keys);
ZEND_METHOD(Teds_StableSortedListMap, bottom);
ZEND_METHOD(Teds_StableSortedListMap, bottomKey);
ZEND_METHOD(Teds_StableSortedListMap, top);
ZEND_METHOD(Teds_StableSortedListMap, topKey);
ZEND_METHOD(Teds_StableSortedListMap, pop);
ZEND_METHOD(Teds_StableSortedListMap, shift);
ZEND_METHOD(Teds_StableSortedListMap, offsetGet);
ZEND_METHOD(Teds_StableSortedListMap, offsetExists);
ZEND_METHOD(Teds_StableSortedListMap, offsetSet);
ZEND_METHOD(Teds_StableSortedListMap, offsetUnset);
ZEND_METHOD(Teds_StableSortedListMap, get);
ZEND_METHOD(Teds_StableSortedListMap, containsValue);
ZEND_METHOD(Teds_StableSortedListMap, containsKey);
ZEND_METHOD(Teds_StableSortedListMap, jsonSerialize);


static const zend_function_entry class_Teds_StableSortedListMap_methods[] = {
	ZEND_ME(Teds_StableSortedListMap, __construct, arginfo_class_Teds_StableSortedListMap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, getIterator, arginfo_class_Teds_StableSortedListMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, count, arginfo_class_Teds_StableSortedListMap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, isEmpty, arginfo_class_Teds_StableSortedListMap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, clear, arginfo_class_Teds_StableSortedListMap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, toPairs, arginfo_class_Teds_StableSortedListMap_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, fromPairs, arginfo_class_Teds_StableSortedListMap_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StableSortedListMap, __serialize, arginfo_class_Teds_StableSortedListMap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, __unserialize, arginfo_class_Teds_StableSortedListMap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, __set_state, arginfo_class_Teds_StableSortedListMap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Teds_StableSortedListMap, toArray, values, arginfo_class_Teds_StableSortedListMap_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, values, arginfo_class_Teds_StableSortedListMap_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, keys, arginfo_class_Teds_StableSortedListMap_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, bottom, arginfo_class_Teds_StableSortedListMap_bottom, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, bottomKey, arginfo_class_Teds_StableSortedListMap_bottomKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, top, arginfo_class_Teds_StableSortedListMap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, topKey, arginfo_class_Teds_StableSortedListMap_topKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, pop, arginfo_class_Teds_StableSortedListMap_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, shift, arginfo_class_Teds_StableSortedListMap_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, offsetGet, arginfo_class_Teds_StableSortedListMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, offsetExists, arginfo_class_Teds_StableSortedListMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, offsetSet, arginfo_class_Teds_StableSortedListMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, offsetUnset, arginfo_class_Teds_StableSortedListMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, get, arginfo_class_Teds_StableSortedListMap_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, containsValue, arginfo_class_Teds_StableSortedListMap_containsValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, containsKey, arginfo_class_Teds_StableSortedListMap_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListMap, jsonSerialize, arginfo_class_Teds_StableSortedListMap_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StableSortedListMap(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Collection, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StableSortedListMap", class_Teds_StableSortedListMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Collection, class_entry_JsonSerializable);

	return class_entry;
}
