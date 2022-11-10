/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 161741d7e05588bbc22373d2805699c2c5159ecd */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_any, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterable, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_Teds_all arginfo_Teds_any

#define arginfo_Teds_none arginfo_Teds_any

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_includes_value, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterable, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_fold, 0, 3, IS_MIXED, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterable, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, initial, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_find, 0, 2, IS_MIXED, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterable, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_unique_values, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterable, Traversable, MAY_BE_ARRAY, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_array_value_first, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_Teds_array_value_last arginfo_Teds_array_value_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_stable_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, v1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, v2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_strict_hash, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_strict_equals, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, v1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, v2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_binary_search, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, comparer, IS_CALLABLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useKey, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_is_same_array_handle, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, array1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array2, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(Teds_any);
ZEND_FUNCTION(Teds_all);
ZEND_FUNCTION(Teds_none);
ZEND_FUNCTION(Teds_includes_value);
ZEND_FUNCTION(Teds_fold);
ZEND_FUNCTION(Teds_find);
ZEND_FUNCTION(Teds_unique_values);
ZEND_FUNCTION(Teds_array_value_first);
ZEND_FUNCTION(Teds_array_value_last);
ZEND_FUNCTION(Teds_stable_compare);
ZEND_FUNCTION(Teds_strict_hash);
ZEND_FUNCTION(Teds_strict_equals);
ZEND_FUNCTION(Teds_binary_search);
ZEND_FUNCTION(Teds_is_same_array_handle);


static const zend_function_entry ext_functions[] = {
	ZEND_NS_FALIAS("Teds", any, Teds_any, arginfo_Teds_any)
	ZEND_NS_FALIAS("Teds", all, Teds_all, arginfo_Teds_all)
	ZEND_NS_FALIAS("Teds", none, Teds_none, arginfo_Teds_none)
	ZEND_NS_FALIAS("Teds", includes_value, Teds_includes_value, arginfo_Teds_includes_value)
	ZEND_NS_FALIAS("Teds", fold, Teds_fold, arginfo_Teds_fold)
	ZEND_NS_FALIAS("Teds", find, Teds_find, arginfo_Teds_find)
	ZEND_NS_FALIAS("Teds", unique_values, Teds_unique_values, arginfo_Teds_unique_values)
	ZEND_NS_FALIAS("Teds", array_value_first, Teds_array_value_first, arginfo_Teds_array_value_first)
	ZEND_NS_FALIAS("Teds", array_value_last, Teds_array_value_last, arginfo_Teds_array_value_last)
	ZEND_NS_FALIAS("Teds", stable_compare, Teds_stable_compare, arginfo_Teds_stable_compare)
	ZEND_NS_FALIAS("Teds", strict_hash, Teds_strict_hash, arginfo_Teds_strict_hash)
	ZEND_NS_FALIAS("Teds", strict_equals, Teds_strict_equals, arginfo_Teds_strict_equals)
	ZEND_NS_FALIAS("Teds", binary_search, Teds_binary_search, arginfo_Teds_binary_search)
	ZEND_NS_FALIAS("Teds", is_same_array_handle, Teds_is_same_array_handle, arginfo_Teds_is_same_array_handle)
	ZEND_FE_END
};
