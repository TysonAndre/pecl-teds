/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d8840a8fff92b44ddd7480325e6517a113a26ed8 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_serialize, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_unserialize, 0, 1, Teds\\ImmutableSortedStringSet, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet___set_state, 0, 1, Teds\\ImmutableSortedStringSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_add, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSortedStringSet_remove arginfo_class_Teds_ImmutableSortedStringSet_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSortedStringSet_first arginfo_class_Teds_ImmutableSortedStringSet_serialize

#define arginfo_class_Teds_ImmutableSortedStringSet_last arginfo_class_Teds_ImmutableSortedStringSet_serialize

#define arginfo_class_Teds_ImmutableSortedStringSet_toArray arginfo_class_Teds_ImmutableSortedStringSet___serialize

#define arginfo_class_Teds_ImmutableSortedStringSet_values arginfo_class_Teds_ImmutableSortedStringSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_get, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSortedStringSet_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSortedStringSet_contains arginfo_class_Teds_ImmutableSortedStringSet_add

#define arginfo_class_Teds_ImmutableSortedStringSet_jsonSerialize arginfo_class_Teds_ImmutableSortedStringSet___serialize


ZEND_METHOD(Teds_ImmutableSortedStringSet, __construct);
ZEND_METHOD(Teds_ImmutableSortedStringSet, getIterator);
ZEND_METHOD(Teds_ImmutableSortedStringSet, count);
ZEND_METHOD(Teds_ImmutableSortedStringSet, isEmpty);
ZEND_METHOD(Teds_ImmutableSortedStringSet, __serialize);
ZEND_METHOD(Teds_ImmutableSortedStringSet, __unserialize);
ZEND_METHOD(Teds_ImmutableSortedStringSet, serialize);
ZEND_METHOD(Teds_ImmutableSortedStringSet, unserialize);
ZEND_METHOD(Teds_ImmutableSortedStringSet, __set_state);
ZEND_METHOD(Teds_ImmutableSortedStringSet, add);
ZEND_METHOD(Teds_ImmutableSortedStringSet, clear);
ZEND_METHOD(Teds_ImmutableSortedStringSet, first);
ZEND_METHOD(Teds_ImmutableSortedStringSet, last);
ZEND_METHOD(Teds_ImmutableSortedStringSet, toArray);
ZEND_METHOD(Teds_ImmutableSortedStringSet, get);
ZEND_METHOD(Teds_ImmutableSortedStringSet, indexOf);
ZEND_METHOD(Teds_ImmutableSortedStringSet, contains);


static const zend_function_entry class_Teds_ImmutableSortedStringSet_methods[] = {
	ZEND_ME(Teds_ImmutableSortedStringSet, __construct, arginfo_class_Teds_ImmutableSortedStringSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, getIterator, arginfo_class_Teds_ImmutableSortedStringSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, count, arginfo_class_Teds_ImmutableSortedStringSet_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, isEmpty, arginfo_class_Teds_ImmutableSortedStringSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, __serialize, arginfo_class_Teds_ImmutableSortedStringSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, __unserialize, arginfo_class_Teds_ImmutableSortedStringSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, serialize, arginfo_class_Teds_ImmutableSortedStringSet_serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, unserialize, arginfo_class_Teds_ImmutableSortedStringSet_unserialize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, __set_state, arginfo_class_Teds_ImmutableSortedStringSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, add, arginfo_class_Teds_ImmutableSortedStringSet_add, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSortedStringSet, remove, add, arginfo_class_Teds_ImmutableSortedStringSet_remove, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, clear, arginfo_class_Teds_ImmutableSortedStringSet_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, first, arginfo_class_Teds_ImmutableSortedStringSet_first, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, last, arginfo_class_Teds_ImmutableSortedStringSet_last, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, toArray, arginfo_class_Teds_ImmutableSortedStringSet_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSortedStringSet, values, toArray, arginfo_class_Teds_ImmutableSortedStringSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, get, arginfo_class_Teds_ImmutableSortedStringSet_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, indexOf, arginfo_class_Teds_ImmutableSortedStringSet_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSortedStringSet, contains, arginfo_class_Teds_ImmutableSortedStringSet_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_ImmutableSortedStringSet, jsonSerialize, toArray, arginfo_class_Teds_ImmutableSortedStringSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_ImmutableSortedStringSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Set, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "ImmutableSortedStringSet", class_Teds_ImmutableSortedStringSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Set, class_entry_JsonSerializable);

	return class_entry;
}
