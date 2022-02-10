/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2aa3f63ed94fd4f3827bcbca8c3dc6ee3601ff20 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_SortedStrictSet___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_SortedStrictSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictSet_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_SortedStrictSet___set_state, 0, 1, Teds\\SortedStrictSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_SortedStrictSet_values arginfo_class_Teds_SortedStrictSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictSet_bottom, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_SortedStrictSet_top arginfo_class_Teds_SortedStrictSet_bottom

#define arginfo_class_Teds_SortedStrictSet_pop arginfo_class_Teds_SortedStrictSet_bottom

#define arginfo_class_Teds_SortedStrictSet_shift arginfo_class_Teds_SortedStrictSet_bottom

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_SortedStrictSet_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_SortedStrictSet_add arginfo_class_Teds_SortedStrictSet_contains

#define arginfo_class_Teds_SortedStrictSet_remove arginfo_class_Teds_SortedStrictSet_contains

#define arginfo_class_Teds_SortedStrictSet_jsonSerialize arginfo_class_Teds_SortedStrictSet___serialize

#define arginfo_class_Teds_SortedStrictSet_debugGetTreeRepresentation arginfo_class_Teds_SortedStrictSet___serialize

#define arginfo_class_Teds_SortedStrictSet_debugIsBalanced arginfo_class_Teds_SortedStrictSet_isEmpty


ZEND_METHOD(Teds_SortedStrictSet, __construct);
ZEND_METHOD(Teds_SortedStrictSet, getIterator);
ZEND_METHOD(Teds_SortedStrictSet, count);
ZEND_METHOD(Teds_SortedStrictSet, isEmpty);
ZEND_METHOD(Teds_SortedStrictSet, clear);
ZEND_METHOD(Teds_SortedStrictSet, values);
ZEND_METHOD(Teds_SortedStrictSet, __unserialize);
ZEND_METHOD(Teds_SortedStrictSet, __set_state);
ZEND_METHOD(Teds_SortedStrictSet, bottom);
ZEND_METHOD(Teds_SortedStrictSet, top);
ZEND_METHOD(Teds_SortedStrictSet, pop);
ZEND_METHOD(Teds_SortedStrictSet, shift);
ZEND_METHOD(Teds_SortedStrictSet, contains);
ZEND_METHOD(Teds_SortedStrictSet, add);
ZEND_METHOD(Teds_SortedStrictSet, remove);
ZEND_METHOD(Teds_SortedStrictSet, debugGetTreeRepresentation);
ZEND_METHOD(Teds_SortedStrictSet, debugIsBalanced);


static const zend_function_entry class_Teds_SortedStrictSet_methods[] = {
	ZEND_ME(Teds_SortedStrictSet, __construct, arginfo_class_Teds_SortedStrictSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, getIterator, arginfo_class_Teds_SortedStrictSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, count, arginfo_class_Teds_SortedStrictSet_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, isEmpty, arginfo_class_Teds_SortedStrictSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, clear, arginfo_class_Teds_SortedStrictSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_SortedStrictSet, __serialize, values, arginfo_class_Teds_SortedStrictSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, __unserialize, arginfo_class_Teds_SortedStrictSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, __set_state, arginfo_class_Teds_SortedStrictSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_SortedStrictSet, values, arginfo_class_Teds_SortedStrictSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, bottom, arginfo_class_Teds_SortedStrictSet_bottom, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, top, arginfo_class_Teds_SortedStrictSet_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, pop, arginfo_class_Teds_SortedStrictSet_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, shift, arginfo_class_Teds_SortedStrictSet_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, contains, arginfo_class_Teds_SortedStrictSet_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, add, arginfo_class_Teds_SortedStrictSet_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, remove, arginfo_class_Teds_SortedStrictSet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_SortedStrictSet, jsonSerialize, values, arginfo_class_Teds_SortedStrictSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, debugGetTreeRepresentation, arginfo_class_Teds_SortedStrictSet_debugGetTreeRepresentation, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_SortedStrictSet, debugIsBalanced, arginfo_class_Teds_SortedStrictSet_debugIsBalanced, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_SortedStrictSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Values, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "SortedStrictSet", class_Teds_SortedStrictSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Values, class_entry_JsonSerializable);

	return class_entry;
}
