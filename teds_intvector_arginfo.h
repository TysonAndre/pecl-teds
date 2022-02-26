/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e7a2e4f36e7560096e20279277c34a14910d3d76 */

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_serialize, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_IntVector_unserialize, 0, 1, Teds\\IntVector, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
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

#define arginfo_class_Teds_IntVector_first arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_IntVector_last arginfo_class_Teds_IntVector_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_IntVector_unshift arginfo_class_Teds_IntVector_push

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_IntVector_insert, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_IntVector_shift arginfo_class_Teds_IntVector_count

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
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_IntVector_jsonSerialize arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_SortedIntVectorSet___construct arginfo_class_Teds_IntVector___construct

#define arginfo_class_Teds_SortedIntVectorSet_getIterator arginfo_class_Teds_IntVector_getIterator

#define arginfo_class_Teds_SortedIntVectorSet_count arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_SortedIntVectorSet_isEmpty arginfo_class_Teds_IntVector_isEmpty

#define arginfo_class_Teds_SortedIntVectorSet_capacity arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_SortedIntVectorSet___serialize arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_SortedIntVectorSet___unserialize arginfo_class_Teds_IntVector___unserialize

#define arginfo_class_Teds_SortedIntVectorSet___set_state arginfo_class_Teds_IntVector___set_state

#define arginfo_class_Teds_SortedIntVectorSet_pop arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_SortedIntVectorSet_add arginfo_class_Teds_IntVector_contains

#define arginfo_class_Teds_SortedIntVectorSet_remove arginfo_class_Teds_IntVector_contains

#define arginfo_class_Teds_SortedIntVectorSet_clear arginfo_class_Teds_IntVector_clear

#define arginfo_class_Teds_SortedIntVectorSet_shift arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_SortedIntVectorSet_first arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_SortedIntVectorSet_last arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_SortedIntVectorSet_toArray arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_SortedIntVectorSet_values arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_SortedIntVectorSet_get arginfo_class_Teds_IntVector_get

#define arginfo_class_Teds_SortedIntVectorSet_indexOf arginfo_class_Teds_IntVector_indexOf

#define arginfo_class_Teds_SortedIntVectorSet_contains arginfo_class_Teds_IntVector_contains

#define arginfo_class_Teds_SortedIntVectorSet_jsonSerialize arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_ImmutableSortedIntSet___construct arginfo_class_Teds_IntVector___construct

#define arginfo_class_Teds_ImmutableSortedIntSet_getIterator arginfo_class_Teds_IntVector_getIterator

#define arginfo_class_Teds_ImmutableSortedIntSet_count arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_ImmutableSortedIntSet_isEmpty arginfo_class_Teds_IntVector_isEmpty

#define arginfo_class_Teds_ImmutableSortedIntSet___serialize arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_ImmutableSortedIntSet___unserialize arginfo_class_Teds_IntVector___unserialize

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSortedIntSet___set_state, 0, 1, Teds\\ImmutableSortedIntSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSortedIntSet_add arginfo_class_Teds_IntVector_contains

#define arginfo_class_Teds_ImmutableSortedIntSet_remove arginfo_class_Teds_IntVector_contains

#define arginfo_class_Teds_ImmutableSortedIntSet_clear arginfo_class_Teds_IntVector_clear

#define arginfo_class_Teds_ImmutableSortedIntSet_first arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_ImmutableSortedIntSet_last arginfo_class_Teds_IntVector_count

#define arginfo_class_Teds_ImmutableSortedIntSet_toArray arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_ImmutableSortedIntSet_values arginfo_class_Teds_IntVector___serialize

#define arginfo_class_Teds_ImmutableSortedIntSet_get arginfo_class_Teds_IntVector_get

#define arginfo_class_Teds_ImmutableSortedIntSet_indexOf arginfo_class_Teds_IntVector_indexOf

#define arginfo_class_Teds_ImmutableSortedIntSet_contains arginfo_class_Teds_IntVector_contains

#define arginfo_class_Teds_ImmutableSortedIntSet_jsonSerialize arginfo_class_Teds_IntVector___serialize


ZEND_METHOD(Teds_IntVector, __construct);
ZEND_METHOD(Teds_IntVector, getIterator);
ZEND_METHOD(Teds_IntVector, count);
ZEND_METHOD(Teds_IntVector, isEmpty);
ZEND_METHOD(Teds_IntVector, capacity);
ZEND_METHOD(Teds_IntVector, __serialize);
ZEND_METHOD(Teds_IntVector, __unserialize);
ZEND_METHOD(Teds_IntVector, serialize);
ZEND_METHOD(Teds_IntVector, unserialize);
ZEND_METHOD(Teds_IntVector, __set_state);
ZEND_METHOD(Teds_IntVector, push);
ZEND_METHOD(Teds_IntVector, pushInts);
ZEND_METHOD(Teds_IntVector, pop);
ZEND_METHOD(Teds_IntVector, first);
ZEND_METHOD(Teds_IntVector, last);
ZEND_METHOD(Teds_IntVector, clear);
ZEND_METHOD(Teds_IntVector, unshift);
ZEND_METHOD(Teds_IntVector, insert);
ZEND_METHOD(Teds_IntVector, shift);
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
ZEND_METHOD(Teds_SortedIntVectorSet, __construct);
ZEND_METHOD(Teds_SortedIntVectorSet, __unserialize);
ZEND_METHOD(Teds_SortedIntVectorSet, __set_state);
ZEND_METHOD(Teds_SortedIntVectorSet, add);
ZEND_METHOD(Teds_SortedIntVectorSet, remove);
ZEND_METHOD(Teds_SortedIntVectorSet, indexOf);
ZEND_METHOD(Teds_SortedIntVectorSet, contains);
ZEND_METHOD(Teds_ImmutableSortedIntSet, __construct);
ZEND_METHOD(Teds_ImmutableSortedIntSet, __serialize);
ZEND_METHOD(Teds_ImmutableSortedIntSet, __unserialize);
ZEND_METHOD(Teds_ImmutableSortedIntSet, add);
ZEND_METHOD(Teds_ImmutableSortedIntSet, clear);


static const zend_function_entry class_Teds_IntVector_methods[] = {
	ZEND_ME(Teds_IntVector, __construct, arginfo_class_Teds_IntVector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, getIterator, arginfo_class_Teds_IntVector_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, count, arginfo_class_Teds_IntVector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, isEmpty, arginfo_class_Teds_IntVector_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, capacity, arginfo_class_Teds_IntVector_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, __serialize, arginfo_class_Teds_IntVector___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, __unserialize, arginfo_class_Teds_IntVector___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, serialize, arginfo_class_Teds_IntVector_serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, unserialize, arginfo_class_Teds_IntVector_unserialize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_IntVector, __set_state, arginfo_class_Teds_IntVector___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_IntVector, push, arginfo_class_Teds_IntVector_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, pushInts, arginfo_class_Teds_IntVector_pushInts, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, pop, arginfo_class_Teds_IntVector_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, first, arginfo_class_Teds_IntVector_first, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, last, arginfo_class_Teds_IntVector_last, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, clear, arginfo_class_Teds_IntVector_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, unshift, arginfo_class_Teds_IntVector_unshift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, insert, arginfo_class_Teds_IntVector_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_IntVector, shift, arginfo_class_Teds_IntVector_shift, ZEND_ACC_PUBLIC)
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


static const zend_function_entry class_Teds_SortedIntVectorSet_methods[] = {
	ZEND_ME(Teds_SortedIntVectorSet, __construct, arginfo_class_Teds_SortedIntVectorSet___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, getIterator, getIterator, arginfo_class_Teds_SortedIntVectorSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, count, count, arginfo_class_Teds_SortedIntVectorSet_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, isEmpty, isEmpty, arginfo_class_Teds_SortedIntVectorSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, capacity, capacity, arginfo_class_Teds_SortedIntVectorSet_capacity, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, __serialize, __serialize, arginfo_class_Teds_SortedIntVectorSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedIntVectorSet, __unserialize, arginfo_class_Teds_SortedIntVectorSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedIntVectorSet, __set_state, arginfo_class_Teds_SortedIntVectorSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Teds_IntVector, pop, pop, arginfo_class_Teds_SortedIntVectorSet_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedIntVectorSet, add, arginfo_class_Teds_SortedIntVectorSet_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedIntVectorSet, remove, arginfo_class_Teds_SortedIntVectorSet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, clear, clear, arginfo_class_Teds_SortedIntVectorSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, shift, shift, arginfo_class_Teds_SortedIntVectorSet_shift, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, first, first, arginfo_class_Teds_SortedIntVectorSet_first, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, last, last, arginfo_class_Teds_SortedIntVectorSet_last, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, toArray, toArray, arginfo_class_Teds_SortedIntVectorSet_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, values, toArray, arginfo_class_Teds_SortedIntVectorSet_values, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, get, get, arginfo_class_Teds_SortedIntVectorSet_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedIntVectorSet, indexOf, arginfo_class_Teds_SortedIntVectorSet_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedIntVectorSet, contains, arginfo_class_Teds_SortedIntVectorSet_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, jsonSerialize, toArray, arginfo_class_Teds_SortedIntVectorSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_ImmutableSortedIntSet_methods[] = {
	ZEND_ME(Teds_ImmutableSortedIntSet, __construct, arginfo_class_Teds_ImmutableSortedIntSet___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, getIterator, getIterator, arginfo_class_Teds_ImmutableSortedIntSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, count, count, arginfo_class_Teds_ImmutableSortedIntSet_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, isEmpty, isEmpty, arginfo_class_Teds_ImmutableSortedIntSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedIntSet, __serialize, arginfo_class_Teds_ImmutableSortedIntSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedIntSet, __unserialize, arginfo_class_Teds_ImmutableSortedIntSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_SortedIntVectorSet, __set_state, __set_state, arginfo_class_Teds_ImmutableSortedIntSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableSortedIntSet, add, arginfo_class_Teds_ImmutableSortedIntSet_add, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSortedIntSet, remove, add, arginfo_class_Teds_ImmutableSortedIntSet_remove, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedIntSet, clear, arginfo_class_Teds_ImmutableSortedIntSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, first, first, arginfo_class_Teds_ImmutableSortedIntSet_first, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, last, last, arginfo_class_Teds_ImmutableSortedIntSet_last, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, toArray, toArray, arginfo_class_Teds_ImmutableSortedIntSet_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, values, toArray, arginfo_class_Teds_ImmutableSortedIntSet_values, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, get, get, arginfo_class_Teds_ImmutableSortedIntSet_get, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_SortedIntVectorSet, indexOf, indexOf, arginfo_class_Teds_ImmutableSortedIntSet_indexOf, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_SortedIntVectorSet, contains, contains, arginfo_class_Teds_ImmutableSortedIntSet_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_IntVector, jsonSerialize, toArray, arginfo_class_Teds_ImmutableSortedIntSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_IntVector(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Sequence, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "IntVector", class_Teds_IntVector_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Sequence, class_entry_JsonSerializable);

	return class_entry;
}

static zend_class_entry *register_class_Teds_SortedIntVectorSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Set, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "SortedIntVectorSet", class_Teds_SortedIntVectorSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Set, class_entry_JsonSerializable);

	return class_entry;
}

static zend_class_entry *register_class_Teds_ImmutableSortedIntSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Set, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "ImmutableSortedIntSet", class_Teds_ImmutableSortedIntSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Set, class_entry_JsonSerializable);

	return class_entry;
}
