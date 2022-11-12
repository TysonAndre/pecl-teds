/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 899af990f800b14f61f196d07c61213cfc687d25 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_LowMemoryVector___construct, 0, 0, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterator, Traversable, MAY_BE_ARRAY, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_LowMemoryVector_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_LowMemoryVector_capacity arginfo_class_Teds_LowMemoryVector_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_LowMemoryVector___set_state, 0, 1, Teds\\LowMemoryVector, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_first, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_LowMemoryVector_last arginfo_class_Teds_LowMemoryVector_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_LowMemoryVector_pop arginfo_class_Teds_LowMemoryVector_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_insert, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_LowMemoryVector_pushFront arginfo_class_Teds_LowMemoryVector_push

#define arginfo_class_Teds_LowMemoryVector_popFront arginfo_class_Teds_LowMemoryVector_first

#define arginfo_class_Teds_LowMemoryVector_unshift arginfo_class_Teds_LowMemoryVector_push

#define arginfo_class_Teds_LowMemoryVector_shift arginfo_class_Teds_LowMemoryVector_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_LowMemoryVector_toArray arginfo_class_Teds_LowMemoryVector___serialize

#define arginfo_class_Teds_LowMemoryVector_values arginfo_class_Teds_LowMemoryVector___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_LowMemoryVector_containsKey arginfo_class_Teds_LowMemoryVector_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_LowMemoryVector_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_LowMemoryVector_jsonSerialize arginfo_class_Teds_LowMemoryVector___serialize


ZEND_METHOD(Teds_LowMemoryVector, __construct);
ZEND_METHOD(Teds_LowMemoryVector, getIterator);
ZEND_METHOD(Teds_LowMemoryVector, count);
ZEND_METHOD(Teds_LowMemoryVector, isEmpty);
ZEND_METHOD(Teds_LowMemoryVector, capacity);
ZEND_METHOD(Teds_LowMemoryVector, __serialize);
ZEND_METHOD(Teds_LowMemoryVector, __unserialize);
ZEND_METHOD(Teds_LowMemoryVector, __set_state);
ZEND_METHOD(Teds_LowMemoryVector, first);
ZEND_METHOD(Teds_LowMemoryVector, last);
ZEND_METHOD(Teds_LowMemoryVector, push);
ZEND_METHOD(Teds_LowMemoryVector, pop);
ZEND_METHOD(Teds_LowMemoryVector, insert);
ZEND_METHOD(Teds_LowMemoryVector, pushFront);
ZEND_METHOD(Teds_LowMemoryVector, popFront);
ZEND_METHOD(Teds_LowMemoryVector, clear);
ZEND_METHOD(Teds_LowMemoryVector, toArray);
ZEND_METHOD(Teds_LowMemoryVector, get);
ZEND_METHOD(Teds_LowMemoryVector, set);
ZEND_METHOD(Teds_LowMemoryVector, offsetGet);
ZEND_METHOD(Teds_LowMemoryVector, offsetExists);
ZEND_METHOD(Teds_LowMemoryVector, containsKey);
ZEND_METHOD(Teds_LowMemoryVector, offsetSet);
ZEND_METHOD(Teds_LowMemoryVector, offsetUnset);
ZEND_METHOD(Teds_LowMemoryVector, indexOf);
ZEND_METHOD(Teds_LowMemoryVector, contains);


static const zend_function_entry class_Teds_LowMemoryVector_methods[] = {
	ZEND_ME(Teds_LowMemoryVector, __construct, arginfo_class_Teds_LowMemoryVector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, getIterator, arginfo_class_Teds_LowMemoryVector_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, count, arginfo_class_Teds_LowMemoryVector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, isEmpty, arginfo_class_Teds_LowMemoryVector_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, capacity, arginfo_class_Teds_LowMemoryVector_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, __serialize, arginfo_class_Teds_LowMemoryVector___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, __unserialize, arginfo_class_Teds_LowMemoryVector___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, __set_state, arginfo_class_Teds_LowMemoryVector___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_LowMemoryVector, first, arginfo_class_Teds_LowMemoryVector_first, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, last, arginfo_class_Teds_LowMemoryVector_last, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, push, arginfo_class_Teds_LowMemoryVector_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, pop, arginfo_class_Teds_LowMemoryVector_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, insert, arginfo_class_Teds_LowMemoryVector_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, pushFront, arginfo_class_Teds_LowMemoryVector_pushFront, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, popFront, arginfo_class_Teds_LowMemoryVector_popFront, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_LowMemoryVector, unshift, pushFront, arginfo_class_Teds_LowMemoryVector_unshift, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_LowMemoryVector, shift, popFront, arginfo_class_Teds_LowMemoryVector_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, clear, arginfo_class_Teds_LowMemoryVector_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, toArray, arginfo_class_Teds_LowMemoryVector_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_LowMemoryVector, values, toArray, arginfo_class_Teds_LowMemoryVector_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, get, arginfo_class_Teds_LowMemoryVector_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, set, arginfo_class_Teds_LowMemoryVector_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, offsetGet, arginfo_class_Teds_LowMemoryVector_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, offsetExists, arginfo_class_Teds_LowMemoryVector_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, containsKey, arginfo_class_Teds_LowMemoryVector_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, offsetSet, arginfo_class_Teds_LowMemoryVector_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, offsetUnset, arginfo_class_Teds_LowMemoryVector_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, indexOf, arginfo_class_Teds_LowMemoryVector_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_LowMemoryVector, contains, arginfo_class_Teds_LowMemoryVector_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_LowMemoryVector, jsonSerialize, toArray, arginfo_class_Teds_LowMemoryVector_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_LowMemoryVector(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Sequence, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "LowMemoryVector", class_Teds_LowMemoryVector_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Sequence, class_entry_JsonSerializable);

	return class_entry;
}
