/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b99c5b20cb6f949354376325c4bfba6667fe8480 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_ImmutableIterable___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iterator, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableIterable_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableIterable_fromPairs, 0, 1, Teds\\ImmutableIterable, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableIterable_toArray arginfo_class_Teds_ImmutableIterable_toPairs

#define arginfo_class_Teds_ImmutableIterable___serialize arginfo_class_Teds_ImmutableIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableIterable___set_state, 0, 1, Teds\\ImmutableIterable, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableIterable_keys arginfo_class_Teds_ImmutableIterable_toPairs

#define arginfo_class_Teds_ImmutableIterable_values arginfo_class_Teds_ImmutableIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_keyAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableIterable_valueAt arginfo_class_Teds_ImmutableIterable_keyAt

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_indexOfKey, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_indexOfValue, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_containsKey, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableIterable_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableIterable_contains arginfo_class_Teds_ImmutableIterable_containsValue

#define arginfo_class_Teds_ImmutableIterable_jsonSerialize arginfo_class_Teds_ImmutableIterable_toPairs


ZEND_METHOD(Teds_ImmutableIterable, __construct);
ZEND_METHOD(Teds_ImmutableIterable, getIterator);
ZEND_METHOD(Teds_ImmutableIterable, count);
ZEND_METHOD(Teds_ImmutableIterable, isEmpty);
ZEND_METHOD(Teds_ImmutableIterable, fromPairs);
ZEND_METHOD(Teds_ImmutableIterable, toPairs);
ZEND_METHOD(Teds_ImmutableIterable, toArray);
ZEND_METHOD(Teds_ImmutableIterable, __serialize);
ZEND_METHOD(Teds_ImmutableIterable, __unserialize);
ZEND_METHOD(Teds_ImmutableIterable, __set_state);
ZEND_METHOD(Teds_ImmutableIterable, keys);
ZEND_METHOD(Teds_ImmutableIterable, values);
ZEND_METHOD(Teds_ImmutableIterable, keyAt);
ZEND_METHOD(Teds_ImmutableIterable, valueAt);
ZEND_METHOD(Teds_ImmutableIterable, clear);
ZEND_METHOD(Teds_ImmutableIterable, indexOfKey);
ZEND_METHOD(Teds_ImmutableIterable, indexOfValue);
ZEND_METHOD(Teds_ImmutableIterable, containsKey);
ZEND_METHOD(Teds_ImmutableIterable, contains);


static const zend_function_entry class_Teds_ImmutableIterable_methods[] = {
	ZEND_ME(Teds_ImmutableIterable, __construct, arginfo_class_Teds_ImmutableIterable___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, getIterator, arginfo_class_Teds_ImmutableIterable_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, count, arginfo_class_Teds_ImmutableIterable_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, isEmpty, arginfo_class_Teds_ImmutableIterable_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, fromPairs, arginfo_class_Teds_ImmutableIterable_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableIterable, toPairs, arginfo_class_Teds_ImmutableIterable_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, toArray, arginfo_class_Teds_ImmutableIterable_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, __serialize, arginfo_class_Teds_ImmutableIterable___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, __unserialize, arginfo_class_Teds_ImmutableIterable___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, __set_state, arginfo_class_Teds_ImmutableIterable___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableIterable, keys, arginfo_class_Teds_ImmutableIterable_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, values, arginfo_class_Teds_ImmutableIterable_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, keyAt, arginfo_class_Teds_ImmutableIterable_keyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, valueAt, arginfo_class_Teds_ImmutableIterable_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, clear, arginfo_class_Teds_ImmutableIterable_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, indexOfKey, arginfo_class_Teds_ImmutableIterable_indexOfKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, indexOfValue, arginfo_class_Teds_ImmutableIterable_indexOfValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, containsKey, arginfo_class_Teds_ImmutableIterable_containsKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableIterable, containsValue, contains, arginfo_class_Teds_ImmutableIterable_containsValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableIterable, contains, arginfo_class_Teds_ImmutableIterable_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableIterable, jsonSerialize, toPairs, arginfo_class_Teds_ImmutableIterable_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_ImmutableIterable(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Collection, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "ImmutableIterable", class_Teds_ImmutableIterable_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Collection, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\ImmutableKeyValueSequence", class_entry);

	return class_entry;
}
