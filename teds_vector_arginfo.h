/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3ad8b294d6f44d056665ec3e33ad4440d4b92ea3 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_Vector___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserveKeys, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_Vector_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Vector_capacity arginfo_class_Teds_Vector_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_shrinkToFit, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_reserve, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, capacity, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Vector_clear arginfo_class_Teds_Vector_shrinkToFit

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_setSize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_Vector___set_state, 0, 1, Teds\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_push, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_pop, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Vector_toArray arginfo_class_Teds_Vector___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_valueAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_setValueAt, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_Vector_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_Vector_map, 0, 1, Teds\\Vector, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_Vector_filter, 0, 0, Teds\\Vector, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_Vector_jsonSerialize arginfo_class_Teds_Vector___serialize


ZEND_METHOD(Teds_Vector, __construct);
ZEND_METHOD(Teds_Vector, getIterator);
ZEND_METHOD(Teds_Vector, count);
ZEND_METHOD(Teds_Vector, capacity);
ZEND_METHOD(Teds_Vector, shrinkToFit);
ZEND_METHOD(Teds_Vector, reserve);
ZEND_METHOD(Teds_Vector, clear);
ZEND_METHOD(Teds_Vector, setSize);
ZEND_METHOD(Teds_Vector, __serialize);
ZEND_METHOD(Teds_Vector, __unserialize);
ZEND_METHOD(Teds_Vector, __set_state);
ZEND_METHOD(Teds_Vector, push);
ZEND_METHOD(Teds_Vector, pop);
ZEND_METHOD(Teds_Vector, toArray);
ZEND_METHOD(Teds_Vector, valueAt);
ZEND_METHOD(Teds_Vector, setValueAt);
ZEND_METHOD(Teds_Vector, offsetGet);
ZEND_METHOD(Teds_Vector, offsetExists);
ZEND_METHOD(Teds_Vector, offsetSet);
ZEND_METHOD(Teds_Vector, offsetUnset);
ZEND_METHOD(Teds_Vector, indexOf);
ZEND_METHOD(Teds_Vector, contains);
ZEND_METHOD(Teds_Vector, map);
ZEND_METHOD(Teds_Vector, filter);
ZEND_METHOD(Teds_Vector, jsonSerialize);


static const zend_function_entry class_Teds_Vector_methods[] = {
	ZEND_ME(Teds_Vector, __construct, arginfo_class_Teds_Vector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, getIterator, arginfo_class_Teds_Vector_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, count, arginfo_class_Teds_Vector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, capacity, arginfo_class_Teds_Vector_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, shrinkToFit, arginfo_class_Teds_Vector_shrinkToFit, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, reserve, arginfo_class_Teds_Vector_reserve, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, clear, arginfo_class_Teds_Vector_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, setSize, arginfo_class_Teds_Vector_setSize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, __serialize, arginfo_class_Teds_Vector___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, __unserialize, arginfo_class_Teds_Vector___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, __set_state, arginfo_class_Teds_Vector___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_Vector, push, arginfo_class_Teds_Vector_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, pop, arginfo_class_Teds_Vector_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, toArray, arginfo_class_Teds_Vector_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, valueAt, arginfo_class_Teds_Vector_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, setValueAt, arginfo_class_Teds_Vector_setValueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, offsetGet, arginfo_class_Teds_Vector_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, offsetExists, arginfo_class_Teds_Vector_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, offsetSet, arginfo_class_Teds_Vector_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, offsetUnset, arginfo_class_Teds_Vector_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, indexOf, arginfo_class_Teds_Vector_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, contains, arginfo_class_Teds_Vector_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, map, arginfo_class_Teds_Vector_map, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, filter, arginfo_class_Teds_Vector_filter, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_Vector, jsonSerialize, arginfo_class_Teds_Vector_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_Vector(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "Vector", class_Teds_Vector_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable, class_entry_ArrayAccess);

	return class_entry;
}
