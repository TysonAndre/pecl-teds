/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 121558f30006a840e719a715fce1b48b61c5eb2f */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap___construct, 0, 0, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterator, Traversable, MAY_BE_ARRAY, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_fromPairs, 0, 1, Teds\\StrictSortedVectorMap, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, pairs, Traversable, MAY_BE_ARRAY, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSortedVectorMap___serialize arginfo_class_Teds_StrictSortedVectorMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap___set_state, 0, 1, Teds\\StrictSortedVectorMap, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSortedVectorMap_toArray arginfo_class_Teds_StrictSortedVectorMap_toPairs

#define arginfo_class_Teds_StrictSortedVectorMap_values arginfo_class_Teds_StrictSortedVectorMap_toPairs

#define arginfo_class_Teds_StrictSortedVectorMap_keys arginfo_class_Teds_StrictSortedVectorMap_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_first, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSortedVectorMap_bottom arginfo_class_Teds_StrictSortedVectorMap_first

#define arginfo_class_Teds_StrictSortedVectorMap_firstKey arginfo_class_Teds_StrictSortedVectorMap_first

#define arginfo_class_Teds_StrictSortedVectorMap_bottomKey arginfo_class_Teds_StrictSortedVectorMap_first

#define arginfo_class_Teds_StrictSortedVectorMap_last arginfo_class_Teds_StrictSortedVectorMap_first

#define arginfo_class_Teds_StrictSortedVectorMap_top arginfo_class_Teds_StrictSortedVectorMap_first

#define arginfo_class_Teds_StrictSortedVectorMap_lastKey arginfo_class_Teds_StrictSortedVectorMap_first

#define arginfo_class_Teds_StrictSortedVectorMap_topKey arginfo_class_Teds_StrictSortedVectorMap_first

#define arginfo_class_Teds_StrictSortedVectorMap_pop arginfo_class_Teds_StrictSortedVectorMap_toPairs

#define arginfo_class_Teds_StrictSortedVectorMap_shift arginfo_class_Teds_StrictSortedVectorMap_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorMap_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSortedVectorMap_contains arginfo_class_Teds_StrictSortedVectorMap_containsValue

#define arginfo_class_Teds_StrictSortedVectorMap_containsKey arginfo_class_Teds_StrictSortedVectorMap_offsetExists

#define arginfo_class_Teds_StrictSortedVectorMap_jsonSerialize arginfo_class_Teds_StrictSortedVectorMap_toPairs


ZEND_METHOD(Teds_StrictSortedVectorMap, __construct);
ZEND_METHOD(Teds_MutableIterable, getIterator);
ZEND_METHOD(Teds_MutableIterable, count);
ZEND_METHOD(Teds_MutableIterable, isEmpty);
ZEND_METHOD(Teds_MutableIterable, clear);
ZEND_METHOD(Teds_MutableIterable, toPairs);
ZEND_METHOD(Teds_StrictSortedVectorMap, fromPairs);
ZEND_METHOD(Teds_MutableIterable, __serialize);
ZEND_METHOD(Teds_StrictSortedVectorMap, __unserialize);
ZEND_METHOD(Teds_StrictSortedVectorMap, __set_state);
ZEND_METHOD(Teds_MutableIterable, toArray);
ZEND_METHOD(Teds_MutableIterable, values);
ZEND_METHOD(Teds_MutableIterable, keys);
ZEND_METHOD(Teds_StrictSortedVectorMap, first);
ZEND_METHOD(Teds_StrictSortedVectorMap, firstKey);
ZEND_METHOD(Teds_StrictSortedVectorMap, last);
ZEND_METHOD(Teds_StrictSortedVectorMap, lastKey);
ZEND_METHOD(Teds_MutableIterable, pop);
ZEND_METHOD(Teds_StrictSortedVectorMap, shift);
ZEND_METHOD(Teds_StrictSortedVectorMap, offsetGet);
ZEND_METHOD(Teds_StrictSortedVectorMap, offsetExists);
ZEND_METHOD(Teds_StrictSortedVectorMap, offsetSet);
ZEND_METHOD(Teds_StrictSortedVectorMap, offsetUnset);
ZEND_METHOD(Teds_StrictSortedVectorMap, get);
ZEND_METHOD(Teds_MutableIterable, contains);
ZEND_METHOD(Teds_StrictSortedVectorMap, containsKey);


static const zend_function_entry class_Teds_StrictSortedVectorMap_methods[] = {
	ZEND_ME(Teds_StrictSortedVectorMap, __construct, arginfo_class_Teds_StrictSortedVectorMap___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, getIterator, getIterator, arginfo_class_Teds_StrictSortedVectorMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, count, count, arginfo_class_Teds_StrictSortedVectorMap_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, isEmpty, isEmpty, arginfo_class_Teds_StrictSortedVectorMap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, clear, clear, arginfo_class_Teds_StrictSortedVectorMap_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, toPairs, toPairs, arginfo_class_Teds_StrictSortedVectorMap_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, fromPairs, arginfo_class_Teds_StrictSortedVectorMap_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Teds_MutableIterable, __serialize, __serialize, arginfo_class_Teds_StrictSortedVectorMap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, __unserialize, arginfo_class_Teds_StrictSortedVectorMap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, __set_state, arginfo_class_Teds_StrictSortedVectorMap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Teds_MutableIterable, toArray, toArray, arginfo_class_Teds_StrictSortedVectorMap_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, values, values, arginfo_class_Teds_StrictSortedVectorMap_values, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, keys, keys, arginfo_class_Teds_StrictSortedVectorMap_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, first, arginfo_class_Teds_StrictSortedVectorMap_first, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictSortedVectorMap, bottom, first, arginfo_class_Teds_StrictSortedVectorMap_bottom, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, firstKey, arginfo_class_Teds_StrictSortedVectorMap_firstKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictSortedVectorMap, bottomKey, firstKey, arginfo_class_Teds_StrictSortedVectorMap_bottomKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, last, arginfo_class_Teds_StrictSortedVectorMap_last, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictSortedVectorMap, top, last, arginfo_class_Teds_StrictSortedVectorMap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, lastKey, arginfo_class_Teds_StrictSortedVectorMap_lastKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictSortedVectorMap, topKey, lastKey, arginfo_class_Teds_StrictSortedVectorMap_topKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, pop, pop, arginfo_class_Teds_StrictSortedVectorMap_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, shift, arginfo_class_Teds_StrictSortedVectorMap_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, offsetGet, arginfo_class_Teds_StrictSortedVectorMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, offsetExists, arginfo_class_Teds_StrictSortedVectorMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, offsetSet, arginfo_class_Teds_StrictSortedVectorMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, offsetUnset, arginfo_class_Teds_StrictSortedVectorMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, get, arginfo_class_Teds_StrictSortedVectorMap_get, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, containsValue, contains, arginfo_class_Teds_StrictSortedVectorMap_containsValue, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, contains, contains, arginfo_class_Teds_StrictSortedVectorMap_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorMap, containsKey, arginfo_class_Teds_StrictSortedVectorMap_containsKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, jsonSerialize, toPairs, arginfo_class_Teds_StrictSortedVectorMap_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictSortedVectorMap(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Map, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictSortedVectorMap", class_Teds_StrictSortedVectorMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Map, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\StableSortedListMap", class_entry);

	return class_entry;
}
