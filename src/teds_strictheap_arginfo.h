/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5a3b7b0ede35ff126cd2c87c2605de5606212be9 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StrictMinHeap___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, values, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap_add, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap_top, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMinHeap_extract arginfo_class_Teds_StrictMinHeap_top

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMinHeap_current arginfo_class_Teds_StrictMinHeap_top

#define arginfo_class_Teds_StrictMinHeap_key arginfo_class_Teds_StrictMinHeap_top

#define arginfo_class_Teds_StrictMinHeap_next arginfo_class_Teds_StrictMinHeap_rewind

#define arginfo_class_Teds_StrictMinHeap_valid arginfo_class_Teds_StrictMinHeap_isEmpty

#define arginfo_class_Teds_StrictMinHeap_clear arginfo_class_Teds_StrictMinHeap_rewind

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictMinHeap___set_state, 0, 1, Teds\\StrictMinHeap, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap_values, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMinHeap_toArray arginfo_class_Teds_StrictMinHeap_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMinHeap___serialize arginfo_class_Teds_StrictMinHeap_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StrictMinHeap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMaxHeap___construct arginfo_class_Teds_StrictMinHeap___construct

#define arginfo_class_Teds_StrictMaxHeap_add arginfo_class_Teds_StrictMinHeap_add

#define arginfo_class_Teds_StrictMaxHeap_top arginfo_class_Teds_StrictMinHeap_top

#define arginfo_class_Teds_StrictMaxHeap_extract arginfo_class_Teds_StrictMinHeap_top

#define arginfo_class_Teds_StrictMaxHeap_count arginfo_class_Teds_StrictMinHeap_count

#define arginfo_class_Teds_StrictMaxHeap_isEmpty arginfo_class_Teds_StrictMinHeap_isEmpty

#define arginfo_class_Teds_StrictMaxHeap_rewind arginfo_class_Teds_StrictMinHeap_rewind

#define arginfo_class_Teds_StrictMaxHeap_current arginfo_class_Teds_StrictMinHeap_top

#define arginfo_class_Teds_StrictMaxHeap_key arginfo_class_Teds_StrictMinHeap_top

#define arginfo_class_Teds_StrictMaxHeap_next arginfo_class_Teds_StrictMinHeap_rewind

#define arginfo_class_Teds_StrictMaxHeap_valid arginfo_class_Teds_StrictMinHeap_isEmpty

#define arginfo_class_Teds_StrictMaxHeap_clear arginfo_class_Teds_StrictMinHeap_rewind

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StrictMaxHeap___set_state, 0, 1, Teds\\StrictMaxHeap, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StrictMaxHeap_values arginfo_class_Teds_StrictMinHeap_values

#define arginfo_class_Teds_StrictMaxHeap_toArray arginfo_class_Teds_StrictMinHeap_values

#define arginfo_class_Teds_StrictMaxHeap_contains arginfo_class_Teds_StrictMinHeap_contains

#define arginfo_class_Teds_StrictMaxHeap___serialize arginfo_class_Teds_StrictMinHeap_values

#define arginfo_class_Teds_StrictMaxHeap___unserialize arginfo_class_Teds_StrictMinHeap___unserialize


ZEND_METHOD(Teds_StrictMinHeap, __construct);
ZEND_METHOD(Teds_StrictMinHeap, add);
ZEND_METHOD(Teds_StrictMinHeap, top);
ZEND_METHOD(Teds_StrictMinHeap, extract);
ZEND_METHOD(Teds_StrictMinHeap, count);
ZEND_METHOD(Teds_StrictMinHeap, isEmpty);
ZEND_METHOD(Teds_StrictMinHeap, rewind);
ZEND_METHOD(Teds_StrictMinHeap, next);
ZEND_METHOD(Teds_StrictMinHeap, valid);
ZEND_METHOD(Teds_StrictMinHeap, clear);
ZEND_METHOD(Teds_StrictMinHeap, __set_state);
ZEND_METHOD(Teds_StrictMinHeap, values);
ZEND_METHOD(Teds_StrictMinHeap, toArray);
ZEND_METHOD(Teds_StrictMinHeap, contains);
ZEND_METHOD(Teds_StrictMinHeap, __unserialize);
ZEND_METHOD(Teds_StrictMaxHeap, __construct);
ZEND_METHOD(Teds_StrictMaxHeap, add);
ZEND_METHOD(Teds_StrictMaxHeap, extract);
ZEND_METHOD(Teds_StrictMaxHeap, next);
ZEND_METHOD(Teds_StrictMaxHeap, __set_state);
ZEND_METHOD(Teds_StrictMaxHeap, __unserialize);


static const zend_function_entry class_Teds_StrictMinHeap_methods[] = {
	ZEND_ME(Teds_StrictMinHeap, __construct, arginfo_class_Teds_StrictMinHeap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, add, arginfo_class_Teds_StrictMinHeap_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, top, arginfo_class_Teds_StrictMinHeap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, extract, arginfo_class_Teds_StrictMinHeap_extract, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, count, arginfo_class_Teds_StrictMinHeap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, isEmpty, arginfo_class_Teds_StrictMinHeap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, rewind, arginfo_class_Teds_StrictMinHeap_rewind, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, current, top, arginfo_class_Teds_StrictMinHeap_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, key, top, arginfo_class_Teds_StrictMinHeap_key, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, next, arginfo_class_Teds_StrictMinHeap_next, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, valid, arginfo_class_Teds_StrictMinHeap_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, clear, arginfo_class_Teds_StrictMinHeap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, __set_state, arginfo_class_Teds_StrictMinHeap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StrictMinHeap, values, arginfo_class_Teds_StrictMinHeap_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, toArray, arginfo_class_Teds_StrictMinHeap_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, contains, arginfo_class_Teds_StrictMinHeap_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, __serialize, values, arginfo_class_Teds_StrictMinHeap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMinHeap, __unserialize, arginfo_class_Teds_StrictMinHeap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_StrictMaxHeap_methods[] = {
	ZEND_ME(Teds_StrictMaxHeap, __construct, arginfo_class_Teds_StrictMaxHeap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMaxHeap, add, arginfo_class_Teds_StrictMaxHeap_add, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, top, top, arginfo_class_Teds_StrictMaxHeap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMaxHeap, extract, arginfo_class_Teds_StrictMaxHeap_extract, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, count, count, arginfo_class_Teds_StrictMaxHeap_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, isEmpty, isEmpty, arginfo_class_Teds_StrictMaxHeap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, rewind, rewind, arginfo_class_Teds_StrictMaxHeap_rewind, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, current, top, arginfo_class_Teds_StrictMaxHeap_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, key, top, arginfo_class_Teds_StrictMaxHeap_key, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMaxHeap, next, arginfo_class_Teds_StrictMaxHeap_next, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, valid, valid, arginfo_class_Teds_StrictMaxHeap_valid, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, clear, clear, arginfo_class_Teds_StrictMaxHeap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMaxHeap, __set_state, arginfo_class_Teds_StrictMaxHeap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Teds_StrictMinHeap, values, values, arginfo_class_Teds_StrictMaxHeap_values, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, toArray, toArray, arginfo_class_Teds_StrictMaxHeap_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, contains, contains, arginfo_class_Teds_StrictMaxHeap_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StrictMinHeap, __serialize, values, arginfo_class_Teds_StrictMaxHeap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StrictMaxHeap, __unserialize, arginfo_class_Teds_StrictMaxHeap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StrictMinHeap(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Teds_Collection)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictMinHeap", class_Teds_StrictMinHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Teds_Collection);

	return class_entry;
}

static zend_class_entry *register_class_Teds_StrictMaxHeap(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Teds_Collection)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StrictMaxHeap", class_Teds_StrictMaxHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Teds_Collection);

	return class_entry;
}
