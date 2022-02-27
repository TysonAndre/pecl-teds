/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4bf165d6bf04621c2aff13d8055f2ed4a93c1e31 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_toArray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_EmptySequence_values arginfo_class_Teds_EmptySequence_toArray

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_get, 0, 1, IS_NEVER, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_set, 0, 2, IS_NEVER, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_insert, 0, 1, IS_NEVER, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_push, 0, 0, IS_NEVER, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_pop, 0, 0, IS_NEVER, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_EmptySequence_unshift arginfo_class_Teds_EmptySequence_push

#define arginfo_class_Teds_EmptySequence_shift arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptySequence_first arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptySequence_last arginfo_class_Teds_EmptySequence_pop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_EmptySequence_containsKey arginfo_class_Teds_EmptySequence_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySequence_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_EmptySequence_map, 0, 1, Teds\\EmptySequence, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_EmptySequence_filter, 0, 0, Teds\\EmptySequence, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_EmptySequence_jsonSerialize arginfo_class_Teds_EmptySequence_toArray

#define arginfo_class_Teds_EmptySequence_valid arginfo_class_Teds_EmptySequence_isEmpty

#define arginfo_class_Teds_EmptySequence_current arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptySequence_key arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptySequence_next arginfo_class_Teds_EmptySequence_clear

#define arginfo_class_Teds_EmptySequence_rewind arginfo_class_Teds_EmptySequence_clear

#define arginfo_class_Teds_EmptyMap_count arginfo_class_Teds_EmptySequence_count

#define arginfo_class_Teds_EmptyMap_isEmpty arginfo_class_Teds_EmptySequence_isEmpty

#define arginfo_class_Teds_EmptyMap_clear arginfo_class_Teds_EmptySequence_clear

#define arginfo_class_Teds_EmptyMap_toArray arginfo_class_Teds_EmptySequence_toArray

#define arginfo_class_Teds_EmptyMap_values arginfo_class_Teds_EmptySequence_toArray

#define arginfo_class_Teds_EmptyMap_keys arginfo_class_Teds_EmptySequence_toArray

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptyMap_offsetGet, 0, 1, IS_NEVER, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptyMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptyMap_offsetSet, 0, 2, IS_NEVER, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptyMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_EmptyMap_contains arginfo_class_Teds_EmptySequence_contains

#define arginfo_class_Teds_EmptyMap_containsKey arginfo_class_Teds_EmptyMap_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptyMap_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_EmptyMap_jsonSerialize arginfo_class_Teds_EmptySequence_toArray

#define arginfo_class_Teds_EmptyMap_valid arginfo_class_Teds_EmptySequence_isEmpty

#define arginfo_class_Teds_EmptyMap_current arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptyMap_key arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptyMap_next arginfo_class_Teds_EmptySequence_clear

#define arginfo_class_Teds_EmptyMap_rewind arginfo_class_Teds_EmptySequence_clear

#define arginfo_class_Teds_EmptySet_count arginfo_class_Teds_EmptySequence_count

#define arginfo_class_Teds_EmptySet_isEmpty arginfo_class_Teds_EmptySequence_isEmpty

#define arginfo_class_Teds_EmptySet_clear arginfo_class_Teds_EmptySequence_clear

#define arginfo_class_Teds_EmptySet_toArray arginfo_class_Teds_EmptySequence_toArray

#define arginfo_class_Teds_EmptySet_values arginfo_class_Teds_EmptySequence_toArray

#define arginfo_class_Teds_EmptySet_contains arginfo_class_Teds_EmptySequence_contains

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_EmptySet_add, 0, 1, IS_NEVER, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_EmptySet_remove arginfo_class_Teds_EmptySequence_contains

#define arginfo_class_Teds_EmptySet_jsonSerialize arginfo_class_Teds_EmptySequence_toArray

#define arginfo_class_Teds_EmptySet_valid arginfo_class_Teds_EmptySequence_isEmpty

#define arginfo_class_Teds_EmptySet_current arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptySet_key arginfo_class_Teds_EmptySequence_pop

#define arginfo_class_Teds_EmptySet_next arginfo_class_Teds_EmptySequence_clear

#define arginfo_class_Teds_EmptySet_rewind arginfo_class_Teds_EmptySequence_clear


ZEND_METHOD(Teds_EmptySequence, count);
ZEND_METHOD(Teds_EmptySequence, isEmpty);
ZEND_METHOD(Teds_EmptySequence, toArray);
ZEND_METHOD(Teds_EmptySequence, get);
ZEND_METHOD(Teds_EmptySequence, set);
ZEND_METHOD(Teds_EmptySequence, insert);
ZEND_METHOD(Teds_EmptySequence, push);
ZEND_METHOD(Teds_EmptySequence, pop);
ZEND_METHOD(Teds_EmptySequence, first);
ZEND_METHOD(Teds_StrictMinHeap, rewind);
ZEND_METHOD(Teds_EmptySequence, offsetGet);
ZEND_METHOD(Teds_EmptySequence, offsetExists);
ZEND_METHOD(Teds_EmptySequence, offsetSet);
ZEND_METHOD(Teds_EmptySequence, offsetUnset);
ZEND_METHOD(Teds_EmptySequence, indexOf);
ZEND_METHOD(Teds_EmptySequence, map);
ZEND_METHOD(Teds_EmptySequence, filter);
ZEND_METHOD(Teds_EmptySequence, valid);
ZEND_METHOD(Teds_EmptyMap, offsetGet);
ZEND_METHOD(Teds_EmptySequence, contains);
ZEND_METHOD(Teds_EmptyMap, get);


static const zend_function_entry class_Teds_EmptySequence_methods[] = {
	ZEND_ME(Teds_EmptySequence, count, arginfo_class_Teds_EmptySequence_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, isEmpty, arginfo_class_Teds_EmptySequence_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, toArray, arginfo_class_Teds_EmptySequence_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, values, toArray, arginfo_class_Teds_EmptySequence_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, get, arginfo_class_Teds_EmptySequence_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, set, arginfo_class_Teds_EmptySequence_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, insert, arginfo_class_Teds_EmptySequence_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, push, arginfo_class_Teds_EmptySequence_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, pop, arginfo_class_Teds_EmptySequence_pop, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, unshift, push, arginfo_class_Teds_EmptySequence_unshift, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, shift, pop, arginfo_class_Teds_EmptySequence_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, first, arginfo_class_Teds_EmptySequence_first, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, last, first, arginfo_class_Teds_EmptySequence_last, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, clear, rewind, arginfo_class_Teds_EmptySequence_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, offsetGet, arginfo_class_Teds_EmptySequence_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, offsetExists, arginfo_class_Teds_EmptySequence_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, containsKey, offsetExists, arginfo_class_Teds_EmptySequence_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, offsetSet, arginfo_class_Teds_EmptySequence_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, offsetUnset, arginfo_class_Teds_EmptySequence_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, indexOf, arginfo_class_Teds_EmptySequence_indexOf, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, contains, offsetExists, arginfo_class_Teds_EmptySequence_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, map, arginfo_class_Teds_EmptySequence_map, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, filter, arginfo_class_Teds_EmptySequence_filter, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, jsonSerialize, toArray, arginfo_class_Teds_EmptySequence_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptySequence, valid, arginfo_class_Teds_EmptySequence_valid, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, current, first, arginfo_class_Teds_EmptySequence_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, key, first, arginfo_class_Teds_EmptySequence_key, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, next, rewind, arginfo_class_Teds_EmptySequence_next, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, rewind, rewind, arginfo_class_Teds_EmptySequence_rewind, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_EmptyMap_methods[] = {
	ZEND_MALIAS(Teds_EmptySequence, count, count, arginfo_class_Teds_EmptyMap_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, isEmpty, isEmpty, arginfo_class_Teds_EmptyMap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, clear, rewind, arginfo_class_Teds_EmptyMap_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, toArray, toArray, arginfo_class_Teds_EmptyMap_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, values, toArray, arginfo_class_Teds_EmptyMap_values, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, keys, toArray, arginfo_class_Teds_EmptyMap_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptyMap, offsetGet, arginfo_class_Teds_EmptyMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, offsetExists, contains, arginfo_class_Teds_EmptyMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, offsetSet, offsetSet, arginfo_class_Teds_EmptyMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, offsetUnset, rewind, arginfo_class_Teds_EmptyMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, contains, contains, arginfo_class_Teds_EmptyMap_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, containsKey, contains, arginfo_class_Teds_EmptyMap_containsKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_EmptyMap, get, arginfo_class_Teds_EmptyMap_get, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, jsonSerialize, toArray, arginfo_class_Teds_EmptyMap_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, valid, valid, arginfo_class_Teds_EmptyMap_valid, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, current, first, arginfo_class_Teds_EmptyMap_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, key, first, arginfo_class_Teds_EmptyMap_key, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, next, rewind, arginfo_class_Teds_EmptyMap_next, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, rewind, rewind, arginfo_class_Teds_EmptyMap_rewind, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_EmptySet_methods[] = {
	ZEND_MALIAS(Teds_EmptySequence, count, count, arginfo_class_Teds_EmptySet_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, isEmpty, isEmpty, arginfo_class_Teds_EmptySet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, clear, rewind, arginfo_class_Teds_EmptySet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, toArray, toArray, arginfo_class_Teds_EmptySet_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, values, toArray, arginfo_class_Teds_EmptySet_values, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, contains, offsetExists, arginfo_class_Teds_EmptySet_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, add, offsetUnset, arginfo_class_Teds_EmptySet_add, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, remove, contains, arginfo_class_Teds_EmptySet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, jsonSerialize, toArray, arginfo_class_Teds_EmptySet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, valid, valid, arginfo_class_Teds_EmptySet_valid, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, current, first, arginfo_class_Teds_EmptySet_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_EmptySequence, key, first, arginfo_class_Teds_EmptySet_key, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, next, rewind, arginfo_class_Teds_EmptySet_next, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, rewind, rewind, arginfo_class_Teds_EmptySet_rewind, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_EmptySequence(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Teds_Sequence, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Teds\\EmptySequence", IS_UNDEF, class_Teds_EmptySequence_methods);
	zend_class_implements(class_entry, 3, class_entry_Iterator, class_entry_Teds_Sequence, class_entry_JsonSerializable);

	zend_enum_add_case_cstr(class_entry, "INSTANCE", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Teds_EmptyMap(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Teds_Map, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Teds\\EmptyMap", IS_UNDEF, class_Teds_EmptyMap_methods);
	zend_class_implements(class_entry, 3, class_entry_Iterator, class_entry_Teds_Map, class_entry_JsonSerializable);

	zend_enum_add_case_cstr(class_entry, "INSTANCE", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Teds_EmptySet(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Teds_Set, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Teds\\EmptySet", IS_UNDEF, class_Teds_EmptySet_methods);
	zend_class_implements(class_entry, 3, class_entry_Iterator, class_entry_Teds_Set, class_entry_JsonSerializable);

	zend_enum_add_case_cstr(class_entry, "INSTANCE", NULL);

	return class_entry;
}
