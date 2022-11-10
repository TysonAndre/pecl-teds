/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2313b06f0da996785ea6d93252c86500d90d8a26 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictTreeSet___construct, 0, 0, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterator, Traversable, MAY_BE_ARRAY, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictTreeSet_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeSet_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeSet_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeSet_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeSet___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeSet___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictTreeSet___set_state, 0, 1, Teds\\StrictTreeSet, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeSet_values arginfo_class_Teds_StrictTreeSet___serialize

#define arginfo_class_Teds_StrictTreeSet_toArray arginfo_class_Teds_StrictTreeSet___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeSet_first, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeSet_bottom arginfo_class_Teds_StrictTreeSet_first

#define arginfo_class_Teds_StrictTreeSet_last arginfo_class_Teds_StrictTreeSet_first

#define arginfo_class_Teds_StrictTreeSet_top arginfo_class_Teds_StrictTreeSet_first

#define arginfo_class_Teds_StrictTreeSet_pop arginfo_class_Teds_StrictTreeSet_first

#define arginfo_class_Teds_StrictTreeSet_shift arginfo_class_Teds_StrictTreeSet_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictTreeSet_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictTreeSet_add arginfo_class_Teds_StrictTreeSet_contains

#define arginfo_class_Teds_StrictTreeSet_remove arginfo_class_Teds_StrictTreeSet_contains

#define arginfo_class_Teds_StrictTreeSet_jsonSerialize arginfo_class_Teds_StrictTreeSet___serialize

#define arginfo_class_Teds_StrictTreeSet_debugGetTreeRepresentation arginfo_class_Teds_StrictTreeSet___serialize

#define arginfo_class_Teds_StrictTreeSet_debugIsBalanced arginfo_class_Teds_StrictTreeSet_isEmpty


ZEND_METHOD(Teds_StrictTreeSet, __construct);
ZEND_METHOD(Teds_StrictTreeSet, getIterator);
ZEND_METHOD(Teds_StrictTreeSet, count);
ZEND_METHOD(Teds_StrictTreeSet, isEmpty);
ZEND_METHOD(Teds_StrictTreeSet, clear);
ZEND_METHOD(Teds_StrictTreeSet, values);
ZEND_METHOD(Teds_StrictTreeSet, __unserialize);
ZEND_METHOD(Teds_StrictTreeSet, __set_state);
ZEND_METHOD(Teds_StrictTreeSet, toArray);
ZEND_METHOD(Teds_StrictTreeSet, first);
ZEND_METHOD(Teds_StrictTreeSet, last);
ZEND_METHOD(Teds_StrictTreeSet, pop);
ZEND_METHOD(Teds_StrictTreeSet, shift);
ZEND_METHOD(Teds_StrictTreeSet, contains);
ZEND_METHOD(Teds_StrictTreeSet, add);
ZEND_METHOD(Teds_StrictTreeSet, remove);
ZEND_METHOD(Teds_StrictTreeSet, debugGetTreeRepresentation);
ZEND_METHOD(Teds_StrictTreeSet, debugIsBalanced);


static const zend_function_entry class_Teds_StrictTreeSet_methods[] = {
	ZEND_ME(Teds_StrictTreeSet, __construct, arginfo_class_Teds_StrictTreeSet___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, getIterator, arginfo_class_Teds_StrictTreeSet_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, count, arginfo_class_Teds_StrictTreeSet_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, isEmpty, arginfo_class_Teds_StrictTreeSet_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, clear, arginfo_class_Teds_StrictTreeSet_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeSet, __serialize, values, arginfo_class_Teds_StrictTreeSet___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, __unserialize, arginfo_class_Teds_StrictTreeSet___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, __set_state, arginfo_class_Teds_StrictTreeSet___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictTreeSet, values, arginfo_class_Teds_StrictTreeSet_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, toArray, arginfo_class_Teds_StrictTreeSet_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, first, arginfo_class_Teds_StrictTreeSet_first, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeSet, bottom, first, arginfo_class_Teds_StrictTreeSet_bottom, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, last, arginfo_class_Teds_StrictTreeSet_last, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeSet, top, last, arginfo_class_Teds_StrictTreeSet_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, pop, arginfo_class_Teds_StrictTreeSet_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, shift, arginfo_class_Teds_StrictTreeSet_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, contains, arginfo_class_Teds_StrictTreeSet_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, add, arginfo_class_Teds_StrictTreeSet_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, remove, arginfo_class_Teds_StrictTreeSet_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictTreeSet, jsonSerialize, values, arginfo_class_Teds_StrictTreeSet_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, debugGetTreeRepresentation, arginfo_class_Teds_StrictTreeSet_debugGetTreeRepresentation, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictTreeSet, debugIsBalanced, arginfo_class_Teds_StrictTreeSet_debugIsBalanced, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictTreeSet(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Teds_Set, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictTreeSet", class_Teds_StrictTreeSet_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Teds_Set, class_entry_JsonSerializable);
	zend_register_class_alias("Teds\\StableSortedSet", class_entry);

	return class_entry;
}
