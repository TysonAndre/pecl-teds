/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4048ea9420ec8083a06618d08f190695ac7a6b68 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StableSortedListSet___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StableSortedListSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListSet_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StableSortedListSet___set_state, 0, 1, Teds\\StableSortedListSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableSortedListSet_values arginfo_class_Teds_StableSortedListSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListSet_bottom, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableSortedListSet_top arginfo_class_Teds_StableSortedListSet_bottom

#define arginfo_class_Teds_StableSortedListSet_pop arginfo_class_Teds_StableSortedListSet_bottom

#define arginfo_class_Teds_StableSortedListSet_shift arginfo_class_Teds_StableSortedListSet_bottom

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableSortedListSet_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableSortedListSet_add arginfo_class_Teds_StableSortedListSet_contains

#define arginfo_class_Teds_StableSortedListSet_remove arginfo_class_Teds_StableSortedListSet_contains

#define arginfo_class_Teds_StableSortedListSet_jsonSerialize arginfo_class_Teds_StableSortedListSet___serialize


ZEND_METHOD(Teds_StableSortedListSet, __construct);
ZEND_METHOD(Teds_StableSortedListSet, getIterator);
ZEND_METHOD(Teds_StableSortedListSet, count);
ZEND_METHOD(Teds_StableSortedListSet, isEmpty);
ZEND_METHOD(Teds_StableSortedListSet, clear);
ZEND_METHOD(Teds_StableSortedListSet, values);
ZEND_METHOD(Teds_StableSortedListSet, __unserialize);
ZEND_METHOD(Teds_StableSortedListSet, __set_state);
ZEND_METHOD(Teds_StableSortedListSet, bottom);
ZEND_METHOD(Teds_StableSortedListSet, top);
ZEND_METHOD(Teds_StableSortedListSet, pop);
ZEND_METHOD(Teds_StableSortedListSet, shift);
ZEND_METHOD(Teds_StableSortedListSet, contains);
ZEND_METHOD(Teds_StableSortedListSet, add);
ZEND_METHOD(Teds_StableSortedListSet, remove);


static const zend_function_entry class_Teds_StableSortedListSet_methods[] = {
	ZEND_ME(Teds_StableSortedListSet, __construct, arginfo_class_Teds_StableSortedListSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, getIterator, arginfo_class_Teds_StableSortedListSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, count, arginfo_class_Teds_StableSortedListSet_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, isEmpty, arginfo_class_Teds_StableSortedListSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, clear, arginfo_class_Teds_StableSortedListSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableSortedListSet, __serialize, values, arginfo_class_Teds_StableSortedListSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, __unserialize, arginfo_class_Teds_StableSortedListSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, __set_state, arginfo_class_Teds_StableSortedListSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StableSortedListSet, values, arginfo_class_Teds_StableSortedListSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, bottom, arginfo_class_Teds_StableSortedListSet_bottom, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, top, arginfo_class_Teds_StableSortedListSet_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, pop, arginfo_class_Teds_StableSortedListSet_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, shift, arginfo_class_Teds_StableSortedListSet_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, contains, arginfo_class_Teds_StableSortedListSet_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, add, arginfo_class_Teds_StableSortedListSet_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableSortedListSet, remove, arginfo_class_Teds_StableSortedListSet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableSortedListSet, jsonSerialize, values, arginfo_class_Teds_StableSortedListSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StableSortedListSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Values, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StableSortedListSet", class_Teds_StableSortedListSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Values, class_entry_JsonSerializable);

	return class_entry;
}
