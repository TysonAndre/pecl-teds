/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 862b5f703882403e46e0563e134a21a9d1650345 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_MutableIterable___construct, 0, 0, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterator, Traversable, MAY_BE_ARRAY, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_MutableIterable_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_MutableIterable_capacity arginfo_class_Teds_MutableIterable_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_setSize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_MutableIterable_fromPairs, 0, 1, Teds\\MutableIterable, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, pairs, Traversable, MAY_BE_ARRAY, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_MutableIterable___serialize arginfo_class_Teds_MutableIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_MutableIterable___set_state, 0, 1, Teds\\MutableIterable, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_push, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_MutableIterable_pop arginfo_class_Teds_MutableIterable_toPairs

#define arginfo_class_Teds_MutableIterable_keys arginfo_class_Teds_MutableIterable_toPairs

#define arginfo_class_Teds_MutableIterable_values arginfo_class_Teds_MutableIterable_toPairs

#define arginfo_class_Teds_MutableIterable_toArray arginfo_class_Teds_MutableIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_keyAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_MutableIterable_valueAt arginfo_class_Teds_MutableIterable_keyAt

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_setKeyAt, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_setValueAt, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_indexOfKey, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_indexOfValue, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_containsKey, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_MutableIterable_containsValue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_MutableIterable_contains arginfo_class_Teds_MutableIterable_containsValue

#define arginfo_class_Teds_MutableIterable_shrinkToFit arginfo_class_Teds_MutableIterable_clear

#define arginfo_class_Teds_MutableIterable_jsonSerialize arginfo_class_Teds_MutableIterable_toPairs


ZEND_METHOD(Teds_MutableIterable, __construct);
ZEND_METHOD(Teds_MutableIterable, getIterator);
ZEND_METHOD(Teds_MutableIterable, count);
ZEND_METHOD(Teds_MutableIterable, isEmpty);
ZEND_METHOD(Teds_MutableIterable, capacity);
ZEND_METHOD(Teds_MutableIterable, clear);
ZEND_METHOD(Teds_MutableIterable, setSize);
ZEND_METHOD(Teds_MutableIterable, fromPairs);
ZEND_METHOD(Teds_MutableIterable, toPairs);
ZEND_METHOD(Teds_MutableIterable, __serialize);
ZEND_METHOD(Teds_MutableIterable, __unserialize);
ZEND_METHOD(Teds_MutableIterable, __set_state);
ZEND_METHOD(Teds_MutableIterable, push);
ZEND_METHOD(Teds_MutableIterable, pop);
ZEND_METHOD(Teds_MutableIterable, keys);
ZEND_METHOD(Teds_MutableIterable, values);
ZEND_METHOD(Teds_MutableIterable, toArray);
ZEND_METHOD(Teds_MutableIterable, keyAt);
ZEND_METHOD(Teds_MutableIterable, valueAt);
ZEND_METHOD(Teds_MutableIterable, setKeyAt);
ZEND_METHOD(Teds_MutableIterable, setValueAt);
ZEND_METHOD(Teds_MutableIterable, indexOfKey);
ZEND_METHOD(Teds_MutableIterable, indexOfValue);
ZEND_METHOD(Teds_MutableIterable, containsKey);
ZEND_METHOD(Teds_MutableIterable, contains);
ZEND_METHOD(Teds_MutableIterable, shrinkToFit);


static const zend_function_entry class_Teds_MutableIterable_methods[] = {
	ZEND_ME(Teds_MutableIterable, __construct, arginfo_class_Teds_MutableIterable___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, getIterator, arginfo_class_Teds_MutableIterable_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, count, arginfo_class_Teds_MutableIterable_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, isEmpty, arginfo_class_Teds_MutableIterable_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, capacity, arginfo_class_Teds_MutableIterable_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, clear, arginfo_class_Teds_MutableIterable_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, setSize, arginfo_class_Teds_MutableIterable_setSize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, fromPairs, arginfo_class_Teds_MutableIterable_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_MutableIterable, toPairs, arginfo_class_Teds_MutableIterable_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, __serialize, arginfo_class_Teds_MutableIterable___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, __unserialize, arginfo_class_Teds_MutableIterable___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, __set_state, arginfo_class_Teds_MutableIterable___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_MutableIterable, push, arginfo_class_Teds_MutableIterable_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, pop, arginfo_class_Teds_MutableIterable_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, keys, arginfo_class_Teds_MutableIterable_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, values, arginfo_class_Teds_MutableIterable_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, toArray, arginfo_class_Teds_MutableIterable_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, keyAt, arginfo_class_Teds_MutableIterable_keyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, valueAt, arginfo_class_Teds_MutableIterable_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, setKeyAt, arginfo_class_Teds_MutableIterable_setKeyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, setValueAt, arginfo_class_Teds_MutableIterable_setValueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, indexOfKey, arginfo_class_Teds_MutableIterable_indexOfKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, indexOfValue, arginfo_class_Teds_MutableIterable_indexOfValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, containsKey, arginfo_class_Teds_MutableIterable_containsKey, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, containsValue, contains, arginfo_class_Teds_MutableIterable_containsValue, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, contains, arginfo_class_Teds_MutableIterable_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_MutableIterable, shrinkToFit, arginfo_class_Teds_MutableIterable_shrinkToFit, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_MutableIterable, jsonSerialize, toPairs, arginfo_class_Teds_MutableIterable_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_MutableIterable(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Collection, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "MutableIterable", class_Teds_MutableIterable_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Collection, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\KeyValueSequence", class_entry);

	return class_entry;
}
