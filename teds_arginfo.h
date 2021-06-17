/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 02c710982b5ce3451d9392ba0445ea265a245ded */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Teds_any, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, iterable, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_Teds_all arginfo_Teds_any


ZEND_FUNCTION(any);
ZEND_FUNCTION(all);


static const zend_function_entry ext_functions[] = {
	ZEND_NS_FE("Teds", any, arginfo_Teds_any)
	ZEND_NS_FE("Teds", all, arginfo_Teds_all)
	ZEND_FE_END
};
