/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0e3b9ccfccb544b1a63ee1ea4f2a892bf6a7cf44 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet___set_state, 0, 1, Teds\\StrictSortedVectorSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSortedVectorSet_values arginfo_class_Teds_StrictSortedVectorSet___serialize

#define arginfo_class_Teds_StrictSortedVectorSet_toArray arginfo_class_Teds_StrictSortedVectorSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet_first, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSortedVectorSet_bottom arginfo_class_Teds_StrictSortedVectorSet_first

#define arginfo_class_Teds_StrictSortedVectorSet_last arginfo_class_Teds_StrictSortedVectorSet_first

#define arginfo_class_Teds_StrictSortedVectorSet_top arginfo_class_Teds_StrictSortedVectorSet_first

#define arginfo_class_Teds_StrictSortedVectorSet_pop arginfo_class_Teds_StrictSortedVectorSet_first

#define arginfo_class_Teds_StrictSortedVectorSet_shift arginfo_class_Teds_StrictSortedVectorSet_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictSortedVectorSet_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictSortedVectorSet_add arginfo_class_Teds_StrictSortedVectorSet_contains

#define arginfo_class_Teds_StrictSortedVectorSet_remove arginfo_class_Teds_StrictSortedVectorSet_contains

#define arginfo_class_Teds_StrictSortedVectorSet_jsonSerialize arginfo_class_Teds_StrictSortedVectorSet___serialize


ZEND_METHOD(Teds_StrictSortedVectorSet, __construct);
ZEND_METHOD(Teds_StrictSortedVectorSet, getIterator);
ZEND_METHOD(Teds_Vector, count);
ZEND_METHOD(Teds_Vector, isEmpty);
ZEND_METHOD(Teds_Vector, clear);
ZEND_METHOD(Teds_Vector, toArray);
ZEND_METHOD(Teds_StrictSortedVectorSet, __unserialize);
ZEND_METHOD(Teds_StrictSortedVectorSet, __set_state);
ZEND_METHOD(Teds_StrictSortedVectorSet, toArray);
ZEND_METHOD(Teds_Vector, first);
ZEND_METHOD(Teds_Vector, last);
ZEND_METHOD(Teds_Vector, pop);
ZEND_METHOD(Teds_Vector, shift);
ZEND_METHOD(Teds_StrictSortedVectorSet, contains);
ZEND_METHOD(Teds_StrictSortedVectorSet, add);
ZEND_METHOD(Teds_StrictSortedVectorSet, remove);


static const zend_function_entry class_Teds_StrictSortedVectorSet_methods[] = {
	ZEND_ME(Teds_StrictSortedVectorSet, __construct, arginfo_class_Teds_StrictSortedVectorSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorSet, getIterator, arginfo_class_Teds_StrictSortedVectorSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, count, count, arginfo_class_Teds_StrictSortedVectorSet_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, isEmpty, isEmpty, arginfo_class_Teds_StrictSortedVectorSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, clear, clear, arginfo_class_Teds_StrictSortedVectorSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, __serialize, toArray, arginfo_class_Teds_StrictSortedVectorSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorSet, __unserialize, arginfo_class_Teds_StrictSortedVectorSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorSet, __set_state, arginfo_class_Teds_StrictSortedVectorSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Teds_Vector, values, toArray, arginfo_class_Teds_StrictSortedVectorSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorSet, toArray, arginfo_class_Teds_StrictSortedVectorSet_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, first, first, arginfo_class_Teds_StrictSortedVectorSet_first, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, bottom, first, arginfo_class_Teds_StrictSortedVectorSet_bottom, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, last, last, arginfo_class_Teds_StrictSortedVectorSet_last, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, top, last, arginfo_class_Teds_StrictSortedVectorSet_top, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, pop, pop, arginfo_class_Teds_StrictSortedVectorSet_pop, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, shift, shift, arginfo_class_Teds_StrictSortedVectorSet_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorSet, contains, arginfo_class_Teds_StrictSortedVectorSet_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorSet, add, arginfo_class_Teds_StrictSortedVectorSet_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictSortedVectorSet, remove, arginfo_class_Teds_StrictSortedVectorSet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_Vector, jsonSerialize, toArray, arginfo_class_Teds_StrictSortedVectorSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictSortedVectorSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Set, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictSortedVectorSet", class_Teds_StrictSortedVectorSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Set, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\StableSortedListMap", class_entry);

	return class_entry;
}
