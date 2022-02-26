/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e26a35fe06208ce66ba1da278adf6c4436f12cf7 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_ImmutableSequence___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSequence_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSequence___set_state, 0, 1, Teds\\ImmutableSequence, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSequence_toArray arginfo_class_Teds_ImmutableSequence___serialize

#define arginfo_class_Teds_ImmutableSequence_values arginfo_class_Teds_ImmutableSequence___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_insert, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_pop, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSequence_unshift arginfo_class_Teds_ImmutableSequence_push

#define arginfo_class_Teds_ImmutableSequence_shift arginfo_class_Teds_ImmutableSequence_pop

#define arginfo_class_Teds_ImmutableSequence_first arginfo_class_Teds_ImmutableSequence_pop

#define arginfo_class_Teds_ImmutableSequence_last arginfo_class_Teds_ImmutableSequence_pop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSequence_containsKey arginfo_class_Teds_ImmutableSequence_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSequence_map, 0, 1, Teds\\ImmutableSequence, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSequence_filter, 0, 0, Teds\\ImmutableSequence, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSequence_jsonSerialize arginfo_class_Teds_ImmutableSequence___serialize


ZEND_METHOD(Teds_ImmutableSequence, __construct);
ZEND_METHOD(Teds_ImmutableSequence, getIterator);
ZEND_METHOD(Teds_ImmutableSequence, count);
ZEND_METHOD(Teds_ImmutableSequence, isEmpty);
ZEND_METHOD(Teds_ImmutableSequence, toArray);
ZEND_METHOD(Teds_ImmutableSequence, __unserialize);
ZEND_METHOD(Teds_ImmutableSequence, __set_state);
ZEND_METHOD(Teds_ImmutableSequence, get);
ZEND_METHOD(Teds_ImmutableSequence, set);
ZEND_METHOD(Teds_ImmutableSequence, insert);
ZEND_METHOD(Teds_ImmutableSequence, push);
ZEND_METHOD(Teds_ImmutableSequence, pop);
ZEND_METHOD(Teds_ImmutableSequence, first);
ZEND_METHOD(Teds_ImmutableSequence, last);
ZEND_METHOD(Teds_ImmutableSequence, clear);
ZEND_METHOD(Teds_ImmutableSequence, offsetGet);
ZEND_METHOD(Teds_ImmutableSequence, offsetExists);
ZEND_METHOD(Teds_ImmutableSequence, containsKey);
ZEND_METHOD(Teds_ImmutableSequence, offsetSet);
ZEND_METHOD(Teds_ImmutableSequence, offsetUnset);
ZEND_METHOD(Teds_ImmutableSequence, indexOf);
ZEND_METHOD(Teds_ImmutableSequence, contains);
ZEND_METHOD(Teds_ImmutableSequence, map);
ZEND_METHOD(Teds_ImmutableSequence, filter);


static const zend_function_entry class_Teds_ImmutableSequence_methods[] = {
	ZEND_ME(Teds_ImmutableSequence, __construct, arginfo_class_Teds_ImmutableSequence___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, getIterator, arginfo_class_Teds_ImmutableSequence_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, count, arginfo_class_Teds_ImmutableSequence_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, isEmpty, arginfo_class_Teds_ImmutableSequence_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSequence, __serialize, toArray, arginfo_class_Teds_ImmutableSequence___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, __unserialize, arginfo_class_Teds_ImmutableSequence___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, __set_state, arginfo_class_Teds_ImmutableSequence___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableSequence, toArray, arginfo_class_Teds_ImmutableSequence_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSequence, values, toArray, arginfo_class_Teds_ImmutableSequence_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, get, arginfo_class_Teds_ImmutableSequence_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, set, arginfo_class_Teds_ImmutableSequence_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, insert, arginfo_class_Teds_ImmutableSequence_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, push, arginfo_class_Teds_ImmutableSequence_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, pop, arginfo_class_Teds_ImmutableSequence_pop, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSequence, unshift, push, arginfo_class_Teds_ImmutableSequence_unshift, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSequence, shift, pop, arginfo_class_Teds_ImmutableSequence_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, first, arginfo_class_Teds_ImmutableSequence_first, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, last, arginfo_class_Teds_ImmutableSequence_last, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, clear, arginfo_class_Teds_ImmutableSequence_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetGet, arginfo_class_Teds_ImmutableSequence_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetExists, arginfo_class_Teds_ImmutableSequence_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, containsKey, arginfo_class_Teds_ImmutableSequence_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetSet, arginfo_class_Teds_ImmutableSequence_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetUnset, arginfo_class_Teds_ImmutableSequence_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, indexOf, arginfo_class_Teds_ImmutableSequence_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, contains, arginfo_class_Teds_ImmutableSequence_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, map, arginfo_class_Teds_ImmutableSequence_map, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, filter, arginfo_class_Teds_ImmutableSequence_filter, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSequence, jsonSerialize, toArray, arginfo_class_Teds_ImmutableSequence_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_ImmutableSequence(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Sequence, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "ImmutableSequence", class_Teds_ImmutableSequence_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Sequence, class_entry_JsonSerializable);

	return class_entry;
}
