/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6272ed0dd6adedfa018ed2cacfc5d6f6715ddf67 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iterator, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_fromPairs, 0, 1, Teds\\ImmutableKeyValueSequence, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableKeyValueSequence___serialize arginfo_class_Teds_ImmutableKeyValueSequence_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence___set_state, 0, 1, Teds\\ImmutableKeyValueSequence, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableKeyValueSequence_keys arginfo_class_Teds_ImmutableKeyValueSequence_toPairs

#define arginfo_class_Teds_ImmutableKeyValueSequence_values arginfo_class_Teds_ImmutableKeyValueSequence_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_keyAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableKeyValueSequence_valueAt arginfo_class_Teds_ImmutableKeyValueSequence_keyAt

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_indexOfKey, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_indexOfValue, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_containsKey, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableKeyValueSequence_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableKeyValueSequence_jsonSerialize arginfo_class_Teds_ImmutableKeyValueSequence_toPairs


ZEND_METHOD(Teds_ImmutableKeyValueSequence, __construct);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, getIterator);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, count);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, isEmpty);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, fromPairs);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, toPairs);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, __serialize);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, __unserialize);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, __set_state);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, keys);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, values);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, keyAt);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, valueAt);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, indexOfKey);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, indexOfValue);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, containsKey);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, containsValue);
ZEND_METHOD(Teds_ImmutableKeyValueSequence, jsonSerialize);


static const zend_function_entry class_Teds_ImmutableKeyValueSequence_methods[] = {
	ZEND_ME(Teds_ImmutableKeyValueSequence, __construct, arginfo_class_Teds_ImmutableKeyValueSequence___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, getIterator, arginfo_class_Teds_ImmutableKeyValueSequence_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, count, arginfo_class_Teds_ImmutableKeyValueSequence_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, isEmpty, arginfo_class_Teds_ImmutableKeyValueSequence_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, fromPairs, arginfo_class_Teds_ImmutableKeyValueSequence_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, toPairs, arginfo_class_Teds_ImmutableKeyValueSequence_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, __serialize, arginfo_class_Teds_ImmutableKeyValueSequence___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, __unserialize, arginfo_class_Teds_ImmutableKeyValueSequence___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, __set_state, arginfo_class_Teds_ImmutableKeyValueSequence___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, keys, arginfo_class_Teds_ImmutableKeyValueSequence_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, values, arginfo_class_Teds_ImmutableKeyValueSequence_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, keyAt, arginfo_class_Teds_ImmutableKeyValueSequence_keyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, valueAt, arginfo_class_Teds_ImmutableKeyValueSequence_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, indexOfKey, arginfo_class_Teds_ImmutableKeyValueSequence_indexOfKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, indexOfValue, arginfo_class_Teds_ImmutableKeyValueSequence_indexOfValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, containsKey, arginfo_class_Teds_ImmutableKeyValueSequence_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, containsValue, arginfo_class_Teds_ImmutableKeyValueSequence_containsValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableKeyValueSequence, jsonSerialize, arginfo_class_Teds_ImmutableKeyValueSequence_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_ImmutableKeyValueSequence(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "ImmutableKeyValueSequence", class_Teds_ImmutableKeyValueSequence_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable);

	return class_entry;
}
