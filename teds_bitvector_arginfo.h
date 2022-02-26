/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f2edde052d7a08bdab1fcebd1a5077f3aa71b11c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_BitVector___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_BitVector_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_capacity arginfo_class_Teds_BitVector_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_serialize, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_BitVector_unserialize, 0, 1, Teds\\BitVector, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_toBinaryString arginfo_class_Teds_BitVector_serialize

#define arginfo_class_Teds_BitVector_fromBinaryString arginfo_class_Teds_BitVector_unserialize

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_BitVector___set_state, 0, 1, Teds\\BitVector, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_unshift arginfo_class_Teds_BitVector_push

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_insert, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_pushBits, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_first arginfo_class_Teds_BitVector_isEmpty

#define arginfo_class_Teds_BitVector_last arginfo_class_Teds_BitVector_isEmpty

#define arginfo_class_Teds_BitVector_pop arginfo_class_Teds_BitVector_isEmpty

#define arginfo_class_Teds_BitVector_shift arginfo_class_Teds_BitVector_isEmpty

#define arginfo_class_Teds_BitVector_toArray arginfo_class_Teds_BitVector___serialize

#define arginfo_class_Teds_BitVector_values arginfo_class_Teds_BitVector___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_get, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_setBit, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_getInt8, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_getUInt8 arginfo_class_Teds_BitVector_getInt8

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_setInt8, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_getInt16 arginfo_class_Teds_BitVector_getInt8

#define arginfo_class_Teds_BitVector_getUInt16 arginfo_class_Teds_BitVector_getInt8

#define arginfo_class_Teds_BitVector_setInt16 arginfo_class_Teds_BitVector_setInt8

#define arginfo_class_Teds_BitVector_getInt32 arginfo_class_Teds_BitVector_getInt8

#define arginfo_class_Teds_BitVector_getUInt32 arginfo_class_Teds_BitVector_getInt8

#define arginfo_class_Teds_BitVector_setInt32 arginfo_class_Teds_BitVector_setInt8

#define arginfo_class_Teds_BitVector_getInt64 arginfo_class_Teds_BitVector_getInt8

#define arginfo_class_Teds_BitVector_setInt64 arginfo_class_Teds_BitVector_setInt8

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_containsKey arginfo_class_Teds_BitVector_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_BitVector_jsonSerialize arginfo_class_Teds_BitVector___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_BitVector_setSize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()


ZEND_METHOD(Teds_BitVector, __construct);
ZEND_METHOD(Teds_BitVector, getIterator);
ZEND_METHOD(Teds_BitVector, count);
ZEND_METHOD(Teds_BitVector, isEmpty);
ZEND_METHOD(Teds_BitVector, capacity);
ZEND_METHOD(Teds_BitVector, __serialize);
ZEND_METHOD(Teds_BitVector, __unserialize);
ZEND_METHOD(Teds_BitVector, serialize);
ZEND_METHOD(Teds_BitVector, unserialize);
ZEND_METHOD(Teds_BitVector, toBinaryString);
ZEND_METHOD(Teds_BitVector, fromBinaryString);
ZEND_METHOD(Teds_BitVector, __set_state);
ZEND_METHOD(Teds_BitVector, push);
ZEND_METHOD(Teds_BitVector, unshift);
ZEND_METHOD(Teds_BitVector, insert);
ZEND_METHOD(Teds_BitVector, pushBits);
ZEND_METHOD(Teds_BitVector, first);
ZEND_METHOD(Teds_BitVector, last);
ZEND_METHOD(Teds_BitVector, pop);
ZEND_METHOD(Teds_BitVector, shift);
ZEND_METHOD(Teds_BitVector, toArray);
ZEND_METHOD(Teds_BitVector, clear);
ZEND_METHOD(Teds_BitVector, get);
ZEND_METHOD(Teds_BitVector, set);
ZEND_METHOD(Teds_BitVector, setBit);
ZEND_METHOD(Teds_BitVector, getInt8);
ZEND_METHOD(Teds_BitVector, getUInt8);
ZEND_METHOD(Teds_BitVector, setInt8);
ZEND_METHOD(Teds_BitVector, getInt16);
ZEND_METHOD(Teds_BitVector, getUInt16);
ZEND_METHOD(Teds_BitVector, setInt16);
ZEND_METHOD(Teds_BitVector, getInt32);
ZEND_METHOD(Teds_BitVector, getUInt32);
ZEND_METHOD(Teds_BitVector, setInt32);
ZEND_METHOD(Teds_BitVector, getInt64);
ZEND_METHOD(Teds_BitVector, setInt64);
ZEND_METHOD(Teds_BitVector, offsetGet);
ZEND_METHOD(Teds_BitVector, offsetExists);
ZEND_METHOD(Teds_BitVector, containsKey);
ZEND_METHOD(Teds_BitVector, offsetSet);
ZEND_METHOD(Teds_BitVector, offsetUnset);
ZEND_METHOD(Teds_BitVector, indexOf);
ZEND_METHOD(Teds_BitVector, contains);
ZEND_METHOD(Teds_BitVector, setSize);


static const zend_function_entry class_Teds_BitVector_methods[] = {
	ZEND_ME(Teds_BitVector, __construct, arginfo_class_Teds_BitVector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getIterator, arginfo_class_Teds_BitVector_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, count, arginfo_class_Teds_BitVector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, isEmpty, arginfo_class_Teds_BitVector_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, capacity, arginfo_class_Teds_BitVector_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, __serialize, arginfo_class_Teds_BitVector___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, __unserialize, arginfo_class_Teds_BitVector___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, serialize, arginfo_class_Teds_BitVector_serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, unserialize, arginfo_class_Teds_BitVector_unserialize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_BitVector, toBinaryString, arginfo_class_Teds_BitVector_toBinaryString, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, fromBinaryString, arginfo_class_Teds_BitVector_fromBinaryString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_BitVector, __set_state, arginfo_class_Teds_BitVector___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_BitVector, push, arginfo_class_Teds_BitVector_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, unshift, arginfo_class_Teds_BitVector_unshift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, insert, arginfo_class_Teds_BitVector_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, pushBits, arginfo_class_Teds_BitVector_pushBits, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, first, arginfo_class_Teds_BitVector_first, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, last, arginfo_class_Teds_BitVector_last, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, pop, arginfo_class_Teds_BitVector_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, shift, arginfo_class_Teds_BitVector_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, toArray, arginfo_class_Teds_BitVector_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_BitVector, values, toArray, arginfo_class_Teds_BitVector_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, clear, arginfo_class_Teds_BitVector_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, get, arginfo_class_Teds_BitVector_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, set, arginfo_class_Teds_BitVector_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, setBit, arginfo_class_Teds_BitVector_setBit, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getInt8, arginfo_class_Teds_BitVector_getInt8, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getUInt8, arginfo_class_Teds_BitVector_getUInt8, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, setInt8, arginfo_class_Teds_BitVector_setInt8, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getInt16, arginfo_class_Teds_BitVector_getInt16, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getUInt16, arginfo_class_Teds_BitVector_getUInt16, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, setInt16, arginfo_class_Teds_BitVector_setInt16, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getInt32, arginfo_class_Teds_BitVector_getInt32, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getUInt32, arginfo_class_Teds_BitVector_getUInt32, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, setInt32, arginfo_class_Teds_BitVector_setInt32, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, getInt64, arginfo_class_Teds_BitVector_getInt64, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, setInt64, arginfo_class_Teds_BitVector_setInt64, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, offsetGet, arginfo_class_Teds_BitVector_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, offsetExists, arginfo_class_Teds_BitVector_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, containsKey, arginfo_class_Teds_BitVector_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, offsetSet, arginfo_class_Teds_BitVector_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, offsetUnset, arginfo_class_Teds_BitVector_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, indexOf, arginfo_class_Teds_BitVector_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, contains, arginfo_class_Teds_BitVector_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_BitVector, jsonSerialize, toArray, arginfo_class_Teds_BitVector_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_BitVector, setSize, arginfo_class_Teds_BitVector_setSize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_BitVector(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Sequence, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "BitVector", class_Teds_BitVector_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Sequence, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\BitVector", class_entry);

	return class_entry;
}
