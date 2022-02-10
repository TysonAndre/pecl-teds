/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 513808a4a71346a47ef5d79a3a3904b7b1c72249 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Values_values, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Values_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Values_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Collection_toArray arginfo_class_Teds_Values_values

#define arginfo_class_Teds_Collection_containsKey arginfo_class_Teds_Values_contains

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ListInterface_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ListInterface_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ListInterface_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ListInterface_pop, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()




static const zend_function_entry class_Teds_Values_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Values, values, arginfo_class_Teds_Values_values, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Values, isEmpty, arginfo_class_Teds_Values_isEmpty, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Values, contains, arginfo_class_Teds_Values_contains, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_Collection_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Collection, toArray, arginfo_class_Teds_Collection_toArray, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Collection, containsKey, arginfo_class_Teds_Collection_containsKey, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_ListInterface_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_ListInterface, get, arginfo_class_Teds_ListInterface_get, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_ListInterface, set, arginfo_class_Teds_ListInterface_set, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_ListInterface, push, arginfo_class_Teds_ListInterface_push, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_ListInterface, pop, arginfo_class_Teds_ListInterface_pop, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_Values(zend_class_entry *class_entry_Traversable, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Values", class_Teds_Values_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 2, class_entry_Traversable, class_entry_Countable);

	return class_entry;
}

static zend_class_entry *register_class_Teds_Collection(zend_class_entry *class_entry_Teds_Values, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Collection", class_Teds_Collection_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 2, class_entry_Teds_Values, class_entry_ArrayAccess);

	return class_entry;
}

static zend_class_entry *register_class_Teds_ListInterface(zend_class_entry *class_entry_Teds_Collection)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "ListInterface", class_Teds_ListInterface_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Teds_Collection);

	return class_entry;
}
