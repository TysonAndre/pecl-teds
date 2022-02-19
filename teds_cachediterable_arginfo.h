/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: df5e0f1b1f2ea4847c77eef7d0f9aa795e70daf1 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_CachedIterable___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iterator, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_CachedIterable_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_CachedIterable_fromPairs, 0, 1, Teds\\CachedIterable, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_CachedIterable_toArray arginfo_class_Teds_CachedIterable_toPairs

#define arginfo_class_Teds_CachedIterable___serialize arginfo_class_Teds_CachedIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_CachedIterable___set_state, 0, 1, Teds\\CachedIterable, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_CachedIterable_keys arginfo_class_Teds_CachedIterable_toPairs

#define arginfo_class_Teds_CachedIterable_values arginfo_class_Teds_CachedIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_keyAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_CachedIterable_valueAt arginfo_class_Teds_CachedIterable_keyAt

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_indexOfKey, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_indexOfValue, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_containsKey, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_CachedIterable_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_CachedIterable_contains arginfo_class_Teds_CachedIterable_containsValue

#define arginfo_class_Teds_CachedIterable_jsonSerialize arginfo_class_Teds_CachedIterable_toPairs


ZEND_METHOD(Teds_CachedIterable, __construct);
ZEND_METHOD(Teds_CachedIterable, getIterator);
ZEND_METHOD(Teds_CachedIterable, count);
ZEND_METHOD(Teds_CachedIterable, isEmpty);
ZEND_METHOD(Teds_CachedIterable, __set_state);
ZEND_METHOD(Teds_CachedIterable, toPairs);
ZEND_METHOD(Teds_CachedIterable, toArray);
ZEND_METHOD(Teds_CachedIterable, __serialize);
ZEND_METHOD(Teds_CachedIterable, __unserialize);
ZEND_METHOD(Teds_CachedIterable, keys);
ZEND_METHOD(Teds_CachedIterable, values);
ZEND_METHOD(Teds_CachedIterable, keyAt);
ZEND_METHOD(Teds_CachedIterable, valueAt);
ZEND_METHOD(Teds_CachedIterable, clear);
ZEND_METHOD(Teds_CachedIterable, indexOfKey);
ZEND_METHOD(Teds_CachedIterable, indexOfValue);
ZEND_METHOD(Teds_CachedIterable, containsKey);
ZEND_METHOD(Teds_CachedIterable, contains);


static const zend_function_entry class_Teds_CachedIterable_methods[] = {
	ZEND_ME(Teds_CachedIterable, __construct, arginfo_class_Teds_CachedIterable___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, getIterator, arginfo_class_Teds_CachedIterable_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, count, arginfo_class_Teds_CachedIterable_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, isEmpty, arginfo_class_Teds_CachedIterable_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_CachedIterable, fromPairs, __set_state, arginfo_class_Teds_CachedIterable_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_CachedIterable, toPairs, arginfo_class_Teds_CachedIterable_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, toArray, arginfo_class_Teds_CachedIterable_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, __serialize, arginfo_class_Teds_CachedIterable___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, __unserialize, arginfo_class_Teds_CachedIterable___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, __set_state, arginfo_class_Teds_CachedIterable___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_CachedIterable, keys, arginfo_class_Teds_CachedIterable_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, values, arginfo_class_Teds_CachedIterable_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, keyAt, arginfo_class_Teds_CachedIterable_keyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, valueAt, arginfo_class_Teds_CachedIterable_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, clear, arginfo_class_Teds_CachedIterable_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, indexOfKey, arginfo_class_Teds_CachedIterable_indexOfKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, indexOfValue, arginfo_class_Teds_CachedIterable_indexOfValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, containsKey, arginfo_class_Teds_CachedIterable_containsKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_CachedIterable, containsValue, contains, arginfo_class_Teds_CachedIterable_containsValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_CachedIterable, contains, arginfo_class_Teds_CachedIterable_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_CachedIterable, jsonSerialize, toPairs, arginfo_class_Teds_CachedIterable_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_CachedIterable(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Collection, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "CachedIterable", class_Teds_CachedIterable_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Collection, class_entry_JsonSerializable);

	return class_entry;
}
