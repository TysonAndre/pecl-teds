/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: cedd6a22a30d665f4f98ed12d736077c28499d64 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictSet___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSet_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictSet___set_state, 0, 1, Teds\\StrictSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSet_values arginfo_class_Teds_StrictSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSet_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSet_add arginfo_class_Teds_StrictSet_contains

#define arginfo_class_Teds_StrictSet_remove arginfo_class_Teds_StrictSet_contains

#define arginfo_class_Teds_StrictSet_jsonSerialize arginfo_class_Teds_StrictSet___serialize


ZEND_METHOD(Teds_StrictSet, __construct);
ZEND_METHOD(Teds_StrictSet, getIterator);
ZEND_METHOD(Teds_StrictSet, count);
ZEND_METHOD(Teds_StrictSet, isEmpty);
ZEND_METHOD(Teds_StrictSet, clear);
ZEND_METHOD(Teds_StrictSet, values);
ZEND_METHOD(Teds_StrictSet, __unserialize);
ZEND_METHOD(Teds_StrictSet, __set_state);
ZEND_METHOD(Teds_StrictSet, contains);
ZEND_METHOD(Teds_StrictSet, add);
ZEND_METHOD(Teds_StrictSet, remove);


static const zend_function_entry class_Teds_StrictSet_methods[] = {
	ZEND_ME(Teds_StrictSet, __construct, arginfo_class_Teds_StrictSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, getIterator, arginfo_class_Teds_StrictSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, count, arginfo_class_Teds_StrictSet_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, isEmpty, arginfo_class_Teds_StrictSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, clear, arginfo_class_Teds_StrictSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictSet, __serialize, values, arginfo_class_Teds_StrictSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, __unserialize, arginfo_class_Teds_StrictSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, __set_state, arginfo_class_Teds_StrictSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictSet, values, arginfo_class_Teds_StrictSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, contains, arginfo_class_Teds_StrictSet_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, add, arginfo_class_Teds_StrictSet_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSet, remove, arginfo_class_Teds_StrictSet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictSet, jsonSerialize, values, arginfo_class_Teds_StrictSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictSet", class_Teds_StrictSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable);

	return class_entry;
}
