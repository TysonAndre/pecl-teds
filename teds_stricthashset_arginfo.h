/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7e16928238e2f1f94c53d9e4bd54eb5009999a08 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictHashSet___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictHashSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashSet_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictHashSet___set_state, 0, 1, Teds\\StrictHashSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictHashSet_values arginfo_class_Teds_StrictHashSet___serialize

#define arginfo_class_Teds_StrictHashSet_toArray arginfo_class_Teds_StrictHashSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictHashSet_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictHashSet_add arginfo_class_Teds_StrictHashSet_contains

#define arginfo_class_Teds_StrictHashSet_remove arginfo_class_Teds_StrictHashSet_contains

#define arginfo_class_Teds_StrictHashSet_jsonSerialize arginfo_class_Teds_StrictHashSet___serialize


ZEND_METHOD(Teds_StrictHashSet, __construct);
ZEND_METHOD(Teds_StrictHashSet, getIterator);
ZEND_METHOD(Teds_StrictHashSet, count);
ZEND_METHOD(Teds_StrictHashSet, isEmpty);
ZEND_METHOD(Teds_StrictHashSet, clear);
ZEND_METHOD(Teds_StrictHashSet, values);
ZEND_METHOD(Teds_StrictHashSet, __unserialize);
ZEND_METHOD(Teds_StrictHashSet, __set_state);
ZEND_METHOD(Teds_StrictHashSet, toArray);
ZEND_METHOD(Teds_StrictHashSet, contains);
ZEND_METHOD(Teds_StrictHashSet, add);
ZEND_METHOD(Teds_StrictHashSet, remove);


static const zend_function_entry class_Teds_StrictHashSet_methods[] = {
	ZEND_ME(Teds_StrictHashSet, __construct, arginfo_class_Teds_StrictHashSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, getIterator, arginfo_class_Teds_StrictHashSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, count, arginfo_class_Teds_StrictHashSet_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, isEmpty, arginfo_class_Teds_StrictHashSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, clear, arginfo_class_Teds_StrictHashSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictHashSet, __serialize, values, arginfo_class_Teds_StrictHashSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, __unserialize, arginfo_class_Teds_StrictHashSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, __set_state, arginfo_class_Teds_StrictHashSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictHashSet, values, arginfo_class_Teds_StrictHashSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, toArray, arginfo_class_Teds_StrictHashSet_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, contains, arginfo_class_Teds_StrictHashSet_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, add, arginfo_class_Teds_StrictHashSet_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictHashSet, remove, arginfo_class_Teds_StrictHashSet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictHashSet, jsonSerialize, values, arginfo_class_Teds_StrictHashSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictHashSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Set, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictHashSet", class_Teds_StrictHashSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Set, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\StrictSet", class_entry);

	return class_entry;
}
