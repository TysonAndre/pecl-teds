/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 736a9d8d7cfd78f2ebcbee39433c42b6a040b795 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_BitSet___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_BitSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitSet_capacity arginfo_class_Teds_BitSet_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_BitSet___set_state, 0, 1, Teds\\BitSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitSet_unshift arginfo_class_Teds_BitSet_push

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_pushBits, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitSet_pop arginfo_class_Teds_BitSet_isEmpty

#define arginfo_class_Teds_BitSet_shift arginfo_class_Teds_BitSet_isEmpty

#define arginfo_class_Teds_BitSet_toArray arginfo_class_Teds_BitSet___serialize

#define arginfo_class_Teds_BitSet_values arginfo_class_Teds_BitSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_get, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_setBit, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitSet_containsKey arginfo_class_Teds_BitSet_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitSet_jsonSerialize arginfo_class_Teds_BitSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitSet_setSize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()


ZEND_METHOD(Teds_BitSet, __construct);
ZEND_METHOD(Teds_BitSet, getIterator);
ZEND_METHOD(Teds_BitSet, count);
ZEND_METHOD(Teds_BitSet, isEmpty);
ZEND_METHOD(Teds_BitSet, capacity);
ZEND_METHOD(Teds_BitSet, __serialize);
ZEND_METHOD(Teds_BitSet, __unserialize);
ZEND_METHOD(Teds_BitSet, __set_state);
ZEND_METHOD(Teds_BitSet, push);
ZEND_METHOD(Teds_BitSet, unshift);
ZEND_METHOD(Teds_BitSet, pushBits);
ZEND_METHOD(Teds_BitSet, pop);
ZEND_METHOD(Teds_BitSet, shift);
ZEND_METHOD(Teds_BitSet, toArray);
ZEND_METHOD(Teds_BitSet, get);
ZEND_METHOD(Teds_BitSet, set);
ZEND_METHOD(Teds_BitSet, setBit);
ZEND_METHOD(Teds_BitSet, offsetGet);
ZEND_METHOD(Teds_BitSet, offsetExists);
ZEND_METHOD(Teds_BitSet, containsKey);
ZEND_METHOD(Teds_BitSet, offsetSet);
ZEND_METHOD(Teds_BitSet, offsetUnset);
ZEND_METHOD(Teds_BitSet, indexOf);
ZEND_METHOD(Teds_BitSet, contains);
ZEND_METHOD(Teds_BitSet, setSize);


static const zend_function_entry class_Teds_BitSet_methods[] = {
	ZEND_ME(Teds_BitSet, __construct, arginfo_class_Teds_BitSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, getIterator, arginfo_class_Teds_BitSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, count, arginfo_class_Teds_BitSet_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, isEmpty, arginfo_class_Teds_BitSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, capacity, arginfo_class_Teds_BitSet_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, __serialize, arginfo_class_Teds_BitSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, __unserialize, arginfo_class_Teds_BitSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, __set_state, arginfo_class_Teds_BitSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_BitSet, push, arginfo_class_Teds_BitSet_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, unshift, arginfo_class_Teds_BitSet_unshift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, pushBits, arginfo_class_Teds_BitSet_pushBits, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, pop, arginfo_class_Teds_BitSet_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, shift, arginfo_class_Teds_BitSet_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, toArray, arginfo_class_Teds_BitSet_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_BitSet, values, toArray, arginfo_class_Teds_BitSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, get, arginfo_class_Teds_BitSet_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, set, arginfo_class_Teds_BitSet_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, setBit, arginfo_class_Teds_BitSet_setBit, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, offsetGet, arginfo_class_Teds_BitSet_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, offsetExists, arginfo_class_Teds_BitSet_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, containsKey, arginfo_class_Teds_BitSet_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, offsetSet, arginfo_class_Teds_BitSet_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, offsetUnset, arginfo_class_Teds_BitSet_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, indexOf, arginfo_class_Teds_BitSet_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, contains, arginfo_class_Teds_BitSet_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_BitSet, jsonSerialize, toArray, arginfo_class_Teds_BitSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitSet, setSize, arginfo_class_Teds_BitSet_setSize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_BitSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Sequence, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "BitSet", class_Teds_BitSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Sequence, class_entry_JsonSerializable);

	return class_entry;
}
