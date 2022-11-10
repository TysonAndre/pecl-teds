/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5b54601586b4c56aeb4376325c512912ca81d291 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Collection_values, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Collection_toArray arginfo_class_Teds_Collection_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Collection_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Collection_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Collection_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Sequence_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Sequence_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Sequence_insert, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Sequence_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Sequence_pop, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Sequence_unshift arginfo_class_Teds_Sequence_push

#define arginfo_class_Teds_Sequence_shift arginfo_class_Teds_Sequence_pop

#define arginfo_class_Teds_Sequence_first arginfo_class_Teds_Sequence_pop

#define arginfo_class_Teds_Sequence_last arginfo_class_Teds_Sequence_pop

#define arginfo_class_Teds_Map_containsKey arginfo_class_Teds_Collection_contains

#define arginfo_class_Teds_Set_add arginfo_class_Teds_Collection_contains

#define arginfo_class_Teds_Set_remove arginfo_class_Teds_Collection_contains




static const zend_function_entry class_Teds_Collection_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Collection, values, arginfo_class_Teds_Collection_values, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Collection, toArray, arginfo_class_Teds_Collection_toArray, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Collection, isEmpty, arginfo_class_Teds_Collection_isEmpty, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Collection, contains, arginfo_class_Teds_Collection_contains, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Collection, clear, arginfo_class_Teds_Collection_clear, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_Sequence_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, get, arginfo_class_Teds_Sequence_get, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, set, arginfo_class_Teds_Sequence_set, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, insert, arginfo_class_Teds_Sequence_insert, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, push, arginfo_class_Teds_Sequence_push, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, pop, arginfo_class_Teds_Sequence_pop, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, unshift, arginfo_class_Teds_Sequence_unshift, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, shift, arginfo_class_Teds_Sequence_shift, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, first, arginfo_class_Teds_Sequence_first, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Sequence, last, arginfo_class_Teds_Sequence_last, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_Map_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Map, containsKey, arginfo_class_Teds_Map_containsKey, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_Set_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Set, add, arginfo_class_Teds_Set_add, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Teds_Set, remove, arginfo_class_Teds_Set_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_Collection(zend_class_entry *class_entry_Traversable, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Collection", class_Teds_Collection_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 2, class_entry_Traversable, class_entry_Countable);
	zend_register_class_alias("Teds\\Values", class_entry);

	return class_entry;
}

static zend_class_entry *register_class_Teds_Sequence(zend_class_entry *class_entry_Teds_Collection, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Sequence", class_Teds_Sequence_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 2, class_entry_Teds_Collection, class_entry_ArrayAccess);

	return class_entry;
}

static zend_class_entry *register_class_Teds_Map(zend_class_entry *class_entry_Teds_Collection, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Map", class_Teds_Map_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 2, class_entry_Teds_Collection, class_entry_ArrayAccess);

	return class_entry;
}

static zend_class_entry *register_class_Teds_Set(zend_class_entry *class_entry_Teds_Collection)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Set", class_Teds_Set_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Teds_Collection);

	return class_entry;
}
