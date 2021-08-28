/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 49ba6bc5e6257777a42fc22b58caaae18533b4d2 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_KeyValueVector___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iterator, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_KeyValueVector_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_KeyValueVector_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_KeyValueVector_fromPairs, 0, 1, Teds\\KeyValueVector, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_KeyValueVector_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_KeyValueVector___serialize arginfo_class_Teds_KeyValueVector_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_KeyValueVector___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_KeyValueVector___set_state, 0, 1, Teds\\KeyValueVector, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_KeyValueVector_keys arginfo_class_Teds_KeyValueVector_toPairs

#define arginfo_class_Teds_KeyValueVector_values arginfo_class_Teds_KeyValueVector_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_KeyValueVector_keyAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_KeyValueVector_valueAt arginfo_class_Teds_KeyValueVector_keyAt

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_KeyValueVector_setKeyAt, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_KeyValueVector_setValueAt, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_KeyValueVector_jsonSerialize arginfo_class_Teds_KeyValueVector_toPairs


ZEND_METHOD(Teds_KeyValueVector, __construct);
ZEND_METHOD(Teds_KeyValueVector, getIterator);
ZEND_METHOD(Teds_KeyValueVector, count);
ZEND_METHOD(Teds_KeyValueVector, fromPairs);
ZEND_METHOD(Teds_KeyValueVector, toPairs);
ZEND_METHOD(Teds_KeyValueVector, __serialize);
ZEND_METHOD(Teds_KeyValueVector, __unserialize);
ZEND_METHOD(Teds_KeyValueVector, __set_state);
ZEND_METHOD(Teds_KeyValueVector, keys);
ZEND_METHOD(Teds_KeyValueVector, values);
ZEND_METHOD(Teds_KeyValueVector, keyAt);
ZEND_METHOD(Teds_KeyValueVector, valueAt);
ZEND_METHOD(Teds_KeyValueVector, setKeyAt);
ZEND_METHOD(Teds_KeyValueVector, setValueAt);
ZEND_METHOD(Teds_KeyValueVector, jsonSerialize);


static const zend_function_entry class_Teds_KeyValueVector_methods[] = {
	ZEND_ME(Teds_KeyValueVector, __construct, arginfo_class_Teds_KeyValueVector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, getIterator, arginfo_class_Teds_KeyValueVector_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, count, arginfo_class_Teds_KeyValueVector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, fromPairs, arginfo_class_Teds_KeyValueVector_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_KeyValueVector, toPairs, arginfo_class_Teds_KeyValueVector_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, __serialize, arginfo_class_Teds_KeyValueVector___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, __unserialize, arginfo_class_Teds_KeyValueVector___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, __set_state, arginfo_class_Teds_KeyValueVector___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_KeyValueVector, keys, arginfo_class_Teds_KeyValueVector_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, values, arginfo_class_Teds_KeyValueVector_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, keyAt, arginfo_class_Teds_KeyValueVector_keyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, valueAt, arginfo_class_Teds_KeyValueVector_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, setKeyAt, arginfo_class_Teds_KeyValueVector_setKeyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, setValueAt, arginfo_class_Teds_KeyValueVector_setValueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_KeyValueVector, jsonSerialize, arginfo_class_Teds_KeyValueVector_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_KeyValueVector(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "KeyValueVector", class_Teds_KeyValueVector_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable);

	return class_entry;
}
