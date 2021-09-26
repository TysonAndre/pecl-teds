/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3c20ef137f7b6251c6b986b05af9383fce6de7d5 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Teds_ImmutableSequence___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iterator, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSequence_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Teds_ImmutableSequence___set_state, 0, 1, Teds\\ImmutableSequence, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSequence_toArray arginfo_class_Teds_ImmutableSequence___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Teds_ImmutableSequence_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Teds_ImmutableSequence_jsonSerialize arginfo_class_Teds_ImmutableSequence___serialize


ZEND_METHOD(Teds_ImmutableSequence, __construct);
ZEND_METHOD(Teds_ImmutableSequence, getIterator);
ZEND_METHOD(Teds_ImmutableSequence, count);
ZEND_METHOD(Teds_ImmutableSequence, isEmpty);
ZEND_METHOD(Teds_ImmutableSequence, __serialize);
ZEND_METHOD(Teds_ImmutableSequence, __unserialize);
ZEND_METHOD(Teds_ImmutableSequence, __set_state);
ZEND_METHOD(Teds_ImmutableSequence, toArray);
ZEND_METHOD(Teds_ImmutableSequence, get);
ZEND_METHOD(Teds_ImmutableSequence, offsetGet);
ZEND_METHOD(Teds_ImmutableSequence, offsetExists);
ZEND_METHOD(Teds_ImmutableSequence, offsetSet);
ZEND_METHOD(Teds_ImmutableSequence, offsetUnset);
ZEND_METHOD(Teds_ImmutableSequence, indexOf);
ZEND_METHOD(Teds_ImmutableSequence, contains);
ZEND_METHOD(Teds_ImmutableSequence, jsonSerialize);


static const zend_function_entry class_Teds_ImmutableSequence_methods[] = {
	ZEND_ME(Teds_ImmutableSequence, __construct, arginfo_class_Teds_ImmutableSequence___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, getIterator, arginfo_class_Teds_ImmutableSequence_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, count, arginfo_class_Teds_ImmutableSequence_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, isEmpty, arginfo_class_Teds_ImmutableSequence_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, __serialize, arginfo_class_Teds_ImmutableSequence___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, __unserialize, arginfo_class_Teds_ImmutableSequence___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, __set_state, arginfo_class_Teds_ImmutableSequence___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Teds_ImmutableSequence, toArray, arginfo_class_Teds_ImmutableSequence_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, get, arginfo_class_Teds_ImmutableSequence_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetGet, arginfo_class_Teds_ImmutableSequence_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetExists, arginfo_class_Teds_ImmutableSequence_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetSet, arginfo_class_Teds_ImmutableSequence_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, offsetUnset, arginfo_class_Teds_ImmutableSequence_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, indexOf, arginfo_class_Teds_ImmutableSequence_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, contains, arginfo_class_Teds_ImmutableSequence_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Teds_ImmutableSequence, jsonSerialize, arginfo_class_Teds_ImmutableSequence_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Teds_ImmutableSequence(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Teds", "ImmutableSequence", class_Teds_ImmutableSequence_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable, class_entry_ArrayAccess);

	return class_entry;
}
