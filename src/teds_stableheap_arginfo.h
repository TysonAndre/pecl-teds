/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5a3b7b0ede35ff126cd2c87c2605de5606212be9 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_StableMinHeap___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, values, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap_add, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap_top, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableMinHeap_extract arginfo_class_Teds_StableMinHeap_top

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableMinHeap_current arginfo_class_Teds_StableMinHeap_top

#define arginfo_class_Teds_StableMinHeap_key arginfo_class_Teds_StableMinHeap_top

#define arginfo_class_Teds_StableMinHeap_next arginfo_class_Teds_StableMinHeap_rewind

#define arginfo_class_Teds_StableMinHeap_valid arginfo_class_Teds_StableMinHeap_isEmpty

#define arginfo_class_Teds_StableMinHeap_clear arginfo_class_Teds_StableMinHeap_rewind

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StableMinHeap___set_state, 0, 1, Teds\\StableMinHeap, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap_values, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableMinHeap_toArray arginfo_class_Teds_StableMinHeap_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableMinHeap___serialize arginfo_class_Teds_StableMinHeap_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_StableMinHeap___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableMaxHeap___construct arginfo_class_Teds_StableMinHeap___construct

#define arginfo_class_Teds_StableMaxHeap_add arginfo_class_Teds_StableMinHeap_add

#define arginfo_class_Teds_StableMaxHeap_top arginfo_class_Teds_StableMinHeap_top

#define arginfo_class_Teds_StableMaxHeap_extract arginfo_class_Teds_StableMinHeap_top

#define arginfo_class_Teds_StableMaxHeap_count arginfo_class_Teds_StableMinHeap_count

#define arginfo_class_Teds_StableMaxHeap_isEmpty arginfo_class_Teds_StableMinHeap_isEmpty

#define arginfo_class_Teds_StableMaxHeap_rewind arginfo_class_Teds_StableMinHeap_rewind

#define arginfo_class_Teds_StableMaxHeap_current arginfo_class_Teds_StableMinHeap_top

#define arginfo_class_Teds_StableMaxHeap_key arginfo_class_Teds_StableMinHeap_top

#define arginfo_class_Teds_StableMaxHeap_next arginfo_class_Teds_StableMinHeap_rewind

#define arginfo_class_Teds_StableMaxHeap_valid arginfo_class_Teds_StableMinHeap_isEmpty

#define arginfo_class_Teds_StableMaxHeap_clear arginfo_class_Teds_StableMinHeap_rewind

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_StableMaxHeap___set_state, 0, 1, Teds\\StableMaxHeap, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_StableMaxHeap_values arginfo_class_Teds_StableMinHeap_values

#define arginfo_class_Teds_StableMaxHeap_toArray arginfo_class_Teds_StableMinHeap_values

#define arginfo_class_Teds_StableMaxHeap_contains arginfo_class_Teds_StableMinHeap_contains

#define arginfo_class_Teds_StableMaxHeap___serialize arginfo_class_Teds_StableMinHeap_values

#define arginfo_class_Teds_StableMaxHeap___unserialize arginfo_class_Teds_StableMinHeap___unserialize


ZEND_METHOD(Teds_StableMinHeap, __construct);
ZEND_METHOD(Teds_StableMinHeap, add);
ZEND_METHOD(Teds_StableMinHeap, top);
ZEND_METHOD(Teds_StableMinHeap, extract);
ZEND_METHOD(Teds_StableMinHeap, count);
ZEND_METHOD(Teds_StableMinHeap, isEmpty);
ZEND_METHOD(Teds_StableMinHeap, rewind);
ZEND_METHOD(Teds_StableMinHeap, next);
ZEND_METHOD(Teds_StableMinHeap, valid);
ZEND_METHOD(Teds_StableMinHeap, clear);
ZEND_METHOD(Teds_StableMinHeap, __set_state);
ZEND_METHOD(Teds_StableMinHeap, values);
ZEND_METHOD(Teds_StableMinHeap, toArray);
ZEND_METHOD(Teds_StableMinHeap, contains);
ZEND_METHOD(Teds_StableMinHeap, __unserialize);
ZEND_METHOD(Teds_StableMaxHeap, __construct);
ZEND_METHOD(Teds_StableMaxHeap, add);
ZEND_METHOD(Teds_StableMaxHeap, extract);
ZEND_METHOD(Teds_StableMaxHeap, next);
ZEND_METHOD(Teds_StableMaxHeap, __set_state);
ZEND_METHOD(Teds_StableMaxHeap, __unserialize);


static const zend_function_entry class_Teds_StableMinHeap_methods[] = {
	ZEND_ME(Teds_StableMinHeap, __construct, arginfo_class_Teds_StableMinHeap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, add, arginfo_class_Teds_StableMinHeap_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, top, arginfo_class_Teds_StableMinHeap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, extract, arginfo_class_Teds_StableMinHeap_extract, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, count, arginfo_class_Teds_StableMinHeap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, isEmpty, arginfo_class_Teds_StableMinHeap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, rewind, arginfo_class_Teds_StableMinHeap_rewind, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, current, top, arginfo_class_Teds_StableMinHeap_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, key, top, arginfo_class_Teds_StableMinHeap_key, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, next, arginfo_class_Teds_StableMinHeap_next, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, valid, arginfo_class_Teds_StableMinHeap_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, clear, arginfo_class_Teds_StableMinHeap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, __set_state, arginfo_class_Teds_StableMinHeap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_StableMinHeap, values, arginfo_class_Teds_StableMinHeap_values, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, toArray, arginfo_class_Teds_StableMinHeap_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, contains, arginfo_class_Teds_StableMinHeap_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, __serialize, values, arginfo_class_Teds_StableMinHeap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMinHeap, __unserialize, arginfo_class_Teds_StableMinHeap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Teds_StableMaxHeap_methods[] = {
	ZEND_ME(Teds_StableMaxHeap, __construct, arginfo_class_Teds_StableMaxHeap___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMaxHeap, add, arginfo_class_Teds_StableMaxHeap_add, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, top, top, arginfo_class_Teds_StableMaxHeap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMaxHeap, extract, arginfo_class_Teds_StableMaxHeap_extract, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, count, count, arginfo_class_Teds_StableMaxHeap_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, isEmpty, isEmpty, arginfo_class_Teds_StableMaxHeap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, rewind, rewind, arginfo_class_Teds_StableMaxHeap_rewind, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, current, top, arginfo_class_Teds_StableMaxHeap_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, key, top, arginfo_class_Teds_StableMaxHeap_key, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMaxHeap, next, arginfo_class_Teds_StableMaxHeap_next, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, valid, valid, arginfo_class_Teds_StableMaxHeap_valid, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, clear, clear, arginfo_class_Teds_StableMaxHeap_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMaxHeap, __set_state, arginfo_class_Teds_StableMaxHeap___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Teds_StableMinHeap, values, values, arginfo_class_Teds_StableMaxHeap_values, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, toArray, toArray, arginfo_class_Teds_StableMaxHeap_toArray, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, contains, contains, arginfo_class_Teds_StableMaxHeap_contains, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Teds_StableMinHeap, __serialize, values, arginfo_class_Teds_StableMaxHeap___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_StableMaxHeap, __unserialize, arginfo_class_Teds_StableMaxHeap___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_StableMinHeap(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Teds_Collection)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StableMinHeap", class_Teds_StableMinHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Teds_Collection);

	return class_entry;
}

static zend_class_entry *register_class_Teds_StableMaxHeap(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Teds_Collection)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "StableMaxHeap", class_Teds_StableMaxHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Teds_Collection);

	return class_entry;
}
