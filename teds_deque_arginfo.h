/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f9e35dfb5c7f55365fa3b20e42e7b53cd09d9456 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_Deque___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_Deque_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Deque_capacity arginfo_class_Teds_Deque_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_Deque___set_state, 0, 1, Teds\\Deque, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_pushBack, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Deque_pushFront arginfo_class_Teds_Deque_pushBack

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_popBack, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Deque_popFront arginfo_class_Teds_Deque_popBack

#define arginfo_class_Teds_Deque_toArray arginfo_class_Teds_Deque___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_valueAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_setValueAt, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Deque_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Deque_jsonSerialize arginfo_class_Teds_Deque___serialize


ZEND_METHOD(Teds_Deque, __construct);
ZEND_METHOD(Teds_Deque, getIterator);
ZEND_METHOD(Teds_Deque, count);
ZEND_METHOD(Teds_Deque, capacity);
ZEND_METHOD(Teds_Deque, clear);
ZEND_METHOD(Teds_Deque, __serialize);
ZEND_METHOD(Teds_Deque, __unserialize);
ZEND_METHOD(Teds_Deque, __set_state);
ZEND_METHOD(Teds_Deque, pushBack);
ZEND_METHOD(Teds_Deque, pushFront);
ZEND_METHOD(Teds_Deque, popBack);
ZEND_METHOD(Teds_Deque, popFront);
ZEND_METHOD(Teds_Deque, toArray);
ZEND_METHOD(Teds_Deque, valueAt);
ZEND_METHOD(Teds_Deque, setValueAt);
ZEND_METHOD(Teds_Deque, offsetGet);
ZEND_METHOD(Teds_Deque, offsetExists);
ZEND_METHOD(Teds_Deque, offsetSet);
ZEND_METHOD(Teds_Deque, offsetUnset);
ZEND_METHOD(Teds_Deque, indexOf);
ZEND_METHOD(Teds_Deque, contains);
ZEND_METHOD(Teds_Deque, jsonSerialize);


static const zend_function_entry class_Teds_Deque_methods[] = {
	ZEND_ME(Teds_Deque, __construct, arginfo_class_Teds_Deque___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, getIterator, arginfo_class_Teds_Deque_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, count, arginfo_class_Teds_Deque_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, capacity, arginfo_class_Teds_Deque_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, clear, arginfo_class_Teds_Deque_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, __serialize, arginfo_class_Teds_Deque___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, __unserialize, arginfo_class_Teds_Deque___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, __set_state, arginfo_class_Teds_Deque___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_Deque, pushBack, arginfo_class_Teds_Deque_pushBack, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, pushFront, arginfo_class_Teds_Deque_pushFront, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, popBack, arginfo_class_Teds_Deque_popBack, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, popFront, arginfo_class_Teds_Deque_popFront, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, toArray, arginfo_class_Teds_Deque_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, valueAt, arginfo_class_Teds_Deque_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, setValueAt, arginfo_class_Teds_Deque_setValueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, offsetGet, arginfo_class_Teds_Deque_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, offsetExists, arginfo_class_Teds_Deque_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, offsetSet, arginfo_class_Teds_Deque_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, offsetUnset, arginfo_class_Teds_Deque_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, indexOf, arginfo_class_Teds_Deque_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, contains, arginfo_class_Teds_Deque_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Deque, jsonSerialize, arginfo_class_Teds_Deque_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_Deque(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Deque", class_Teds_Deque_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable, class_entry_ArrayAccess);

	return class_entry;
}
