/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f51c619663dc5b41a90a90ae12b9cb8a78a88818 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_any, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, iterable, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_Teds_all arginfo_Teds_any

#define arginfo_Teds_none arginfo_Teds_any

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_includes_value, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, iterable, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_fold, 0, 3, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, iterable, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, initial, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_find, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, iterable, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_array_value_first, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_Teds_array_value_last arginfo_Teds_array_value_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_stable_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, v1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, v2, IS_MIXED, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(any);
ZEND_FUNCTION(all);
ZEND_FUNCTION(none);
ZEND_FUNCTION(includes_value);
ZEND_FUNCTION(fold);
ZEND_FUNCTION(find);
ZEND_FUNCTION(array_value_first);
ZEND_FUNCTION(array_value_last);
ZEND_FUNCTION(stable_compare);


static const zend_function_entry ext_functions[] = {
	ZEND_NS_FE("Teds", any, arginfo_Teds_any)
	ZEND_NS_FE("Teds", all, arginfo_Teds_all)
	ZEND_NS_FE("Teds", none, arginfo_Teds_none)
	ZEND_NS_FE("Teds", includes_value, arginfo_Teds_includes_value)
	ZEND_NS_FE("Teds", fold, arginfo_Teds_fold)
	ZEND_NS_FE("Teds", find, arginfo_Teds_find)
	ZEND_NS_FE("Teds", array_value_first, arginfo_Teds_array_value_first)
	ZEND_NS_FE("Teds", array_value_last, arginfo_Teds_array_value_last)
	ZEND_NS_FE("Teds", stable_compare, arginfo_Teds_stable_compare)
	ZEND_FE_END
};
