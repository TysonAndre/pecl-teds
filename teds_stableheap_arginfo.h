/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 35fe1ccb726fe6f69fa37ed0c80df7377464b493 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMaxHeap_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableMinHeap_compare arginfo_class_Teds_StableMaxHeap_compare


ZEND_METHOD(Teds_StableMaxHeap, compare);
ZEND_METHOD(Teds_StableMinHeap, compare);


static const zend_function_entry class_Teds_StableMaxHeap_methods[] = {
	ZEND_ME(Teds_StableMaxHeap, compare, arginfo_class_Teds_StableMaxHeap_compare, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_StableMinHeap_methods[] = {
	ZEND_ME(Teds_StableMinHeap, compare, arginfo_class_Teds_StableMinHeap_compare, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StableMaxHeap(zend_class_entry *class_entry_Teds_SplHeap)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StableMaxHeap", class_Teds_StableMaxHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Teds_SplHeap);

	return class_entry;
}

static zend_class_entry *register_class_Teds_StableMinHeap(zend_class_entry *class_entry_Teds_SplHeap)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StableMinHeap", class_Teds_StableMinHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Teds_SplHeap);

	return class_entry;
}
