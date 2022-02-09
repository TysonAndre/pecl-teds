/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e597407fa9d50af1bfbcd13f829e82b9b82dc1ab */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_IntVector___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_IntVector_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_IntVector_capacity arginfo_class_Teds_IntVector_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_IntVector___set_state, 0, 1, Teds\\IntVector, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_pushInts, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_IntVector_pop arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_IntVector_toArray arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_IntVector_values arginfo_class_Teds_IntVector___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_get, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_setInt, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_IntVector_containsKey arginfo_class_Teds_IntVector_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_IntVector_jsonSerialize arginfo_class_Teds_IntVector___serialize


ZEND_METHOD(Teds_IntVector, __construct);
ZEND_METHOD(Teds_IntVector, getIterator);
ZEND_METHOD(Teds_IntVector, count);
ZEND_METHOD(Teds_IntVector, isEmpty);
ZEND_METHOD(Teds_IntVector, capacity);
ZEND_METHOD(Teds_IntVector, __serialize);
ZEND_METHOD(Teds_IntVector, __unserialize);
ZEND_METHOD(Teds_IntVector, __set_state);
ZEND_METHOD(Teds_IntVector, push);
ZEND_METHOD(Teds_IntVector, pushInts);
ZEND_METHOD(Teds_IntVector, pop);
ZEND_METHOD(Teds_IntVector, toArray);
ZEND_METHOD(Teds_IntVector, get);
ZEND_METHOD(Teds_IntVector, set);
ZEND_METHOD(Teds_IntVector, setInt);
ZEND_METHOD(Teds_IntVector, offsetGet);
ZEND_METHOD(Teds_IntVector, offsetExists);
ZEND_METHOD(Teds_IntVector, containsKey);
ZEND_METHOD(Teds_IntVector, offsetSet);
ZEND_METHOD(Teds_IntVector, offsetUnset);
ZEND_METHOD(Teds_IntVector, indexOf);
ZEND_METHOD(Teds_IntVector, contains);


static const zend_function_entry class_Teds_IntVector_methods[] = {
	ZEND_ME(Teds_IntVector, __construct, arginfo_class_Teds_IntVector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, getIterator, arginfo_class_Teds_IntVector_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, count, arginfo_class_Teds_IntVector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, isEmpty, arginfo_class_Teds_IntVector_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, capacity, arginfo_class_Teds_IntVector_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, __serialize, arginfo_class_Teds_IntVector___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, __unserialize, arginfo_class_Teds_IntVector___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, __set_state, arginfo_class_Teds_IntVector___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_IntVector, push, arginfo_class_Teds_IntVector_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, pushInts, arginfo_class_Teds_IntVector_pushInts, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, pop, arginfo_class_Teds_IntVector_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, toArray, arginfo_class_Teds_IntVector_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, values, toArray, arginfo_class_Teds_IntVector_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, get, arginfo_class_Teds_IntVector_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, set, arginfo_class_Teds_IntVector_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, setInt, arginfo_class_Teds_IntVector_setInt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, offsetGet, arginfo_class_Teds_IntVector_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, offsetExists, arginfo_class_Teds_IntVector_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, containsKey, arginfo_class_Teds_IntVector_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, offsetSet, arginfo_class_Teds_IntVector_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, offsetUnset, arginfo_class_Teds_IntVector_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, indexOf, arginfo_class_Teds_IntVector_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, contains, arginfo_class_Teds_IntVector_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, jsonSerialize, toArray, arginfo_class_Teds_IntVector_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_IntVector(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_ListInterface, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "IntVector", class_Teds_IntVector_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_ListInterface, class_entry_JsonSerializable);

	return class_entry;
}
