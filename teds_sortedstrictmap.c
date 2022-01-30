/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* This is a binary search tree. */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_teds.h"
#include "teds_sortedstrictmap_arginfo.h"
#include "teds_sortedstrictmap.h"
#include "teds_util.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

zend_object_handlers teds_handler_SortedStrictMap;
zend_class_entry *teds_ce_SortedStrictMap;

static zend_always_inline teds_sortedstrictmap_node *teds_sortedstrictmap_tree_find_key(const teds_sortedstrictmap_tree *tree, zval *key)
{
	teds_sortedstrictmap_node *it = tree->root;
	while (it != NULL) {
		const int comparison = teds_stable_compare(key, &it->key);
		if (comparison > 0) {
			it = it->right;
		} else if (comparison < 0) {
			it = it->left;
		} else {
			return it;
		}
	}
	return NULL;
}

static zend_always_inline void teds_sortedstrictmap_tree_remove_node(teds_sortedstrictmap_tree *array, teds_sortedstrictmap_node *const node, bool free_zvals);

static zend_always_inline teds_sortedstrictmap_node *teds_sortedstrictmap_node_alloc(const zval *key, const zval *value, teds_sortedstrictmap_node *parent) {
	teds_sortedstrictmap_node *n = emalloc(sizeof(teds_sortedstrictmap_node));
	n->parent = parent;
	ZVAL_COPY(&n->key, key);
	ZVAL_COPY(&n->value, value);
	TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(n) = 1;
	TEDS_SORTEDSTRICTMAP_NODE_COLOR(n) = TEDS_NODE_RED;
	return n;
}

/* Given a node, rotate it so that the child node becomes the parent of that node */
static zend_always_inline void teds_sortedstrictmap_node_rotate_dir(teds_sortedstrictmap_node *const node, int dir) {
	teds_sortedstrictmap_node *const c = node->children[dir];
	teds_sortedstrictmap_node *const parent = node->parent;
	teds_sortedstrictmap_node *const transfer = c->children[1 - dir];

	if (parent != NULL) {
		if (parent->children[dir] == node) {
			parent->children[dir] = c;
		} else {
			ZEND_ASSERT(parent->children[1 - dir] == node);
			parent->children[1 - dir] = c;
		}
	}
	c->parent = parent;

	c->children[1 - dir] = node;
	node->parent = c;

	node->children[dir] = transfer;
	if (transfer != NULL) {
		transfer->parent = node;
	}
}

static zend_always_inline void teds_sortedstrictmap_entries_rebalance_after_insert(teds_sortedstrictmap_tree *tree, teds_sortedstrictmap_node *node) {
	while (true) {
		teds_sortedstrictmap_node *parent = node->parent;
		// ZEND_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_COLOR(node) == TEDS_NODE_RED);

		/* Based on https://en.wikipedia.org/wiki/Red%E2%80%93black_tree */
		if (parent == NULL || TEDS_SORTEDSTRICTMAP_NODE_COLOR(parent) == TEDS_NODE_BLACK) {
			return;
		}

		teds_sortedstrictmap_node *const grandparent = parent->parent;
		if (grandparent == NULL) {
			TEDS_SORTEDSTRICTMAP_NODE_COLOR(parent) = TEDS_NODE_BLACK;
			return;
		}
		/* In a valid tree, the grandparent node would be black if parent was red */

		const int dir = grandparent->right == parent ? 1 : 0;
		ZEND_ASSERT(grandparent->children[dir] == parent);
		teds_sortedstrictmap_node *uncle;
		if (dir) {
			uncle = grandparent->left;
		} else {
			uncle = grandparent->right;
		}

		if (uncle && TEDS_SORTEDSTRICTMAP_NODE_COLOR(uncle) == TEDS_NODE_RED) {
			TEDS_SORTEDSTRICTMAP_NODE_COLOR(uncle) = TEDS_NODE_BLACK;
			TEDS_SORTEDSTRICTMAP_NODE_COLOR(parent) = TEDS_NODE_BLACK;
			TEDS_SORTEDSTRICTMAP_NODE_COLOR(grandparent) = TEDS_NODE_RED;
			node = grandparent;
			continue;
		}
		/* Parent node is red but the uncle node is black. */
		/* In a valid tree, the grandparent node would be black */
		if (node == parent->children[1 - dir]) {
			teds_sortedstrictmap_node_rotate_dir(parent, 1 - dir);
			teds_sortedstrictmap_node *orig_node = node;
			node = parent;
			parent = orig_node;
		}
		ZEND_ASSERT(grandparent->children[dir] == parent);
		ZEND_ASSERT(parent->children[dir] == node);
		ZEND_ASSERT(parent->parent == grandparent);
		ZEND_ASSERT(node->parent == parent);

		teds_sortedstrictmap_node_rotate_dir(grandparent, dir);
		TEDS_SORTEDSTRICTMAP_NODE_COLOR(parent) = TEDS_NODE_BLACK;
		TEDS_SORTEDSTRICTMAP_NODE_COLOR(grandparent) = TEDS_NODE_RED;

		if (grandparent == tree->root) {
			tree->root = parent;
		}
		return;
	}
}

static zend_always_inline void teds_sortedstrictmap_node_release(teds_sortedstrictmap_node *node) {
	ZEND_ASSERT(node != NULL);
	ZEND_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(node) > 0);
	if (--TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(node) == 0) {
		efree_size(node, sizeof(teds_sortedstrictmap_node));
	}
}

/* Returns true if a new entry was added to the map, false if updated. Based on _zend_hash_add_or_update_i. */
static zend_always_inline bool teds_sortedstrictmap_tree_insert(teds_sortedstrictmap_tree *tree, zval *key, zval *value, bool add_new)
{
	ZEND_ASSERT(Z_TYPE_P(key) != IS_UNDEF);
	ZEND_ASSERT(Z_TYPE_P(value) != IS_UNDEF);

	/* TODO optimize */
	teds_sortedstrictmap_node *it = tree->root;
	if (it == NULL) {
		/* Initialize this tree as a new binary search tree of size 1 */
		it = teds_sortedstrictmap_node_alloc(key, value, NULL);
		tree->root = it;
		it->left = NULL;
		it->right = NULL;
		it->prev = NULL;
		it->next = NULL;
		tree->nNumOfElements++;
		return true;
	}
	/* c must be declared in outer scope for goto to work. */
	teds_sortedstrictmap_node *c;
	while (true) {
		const int comparison = teds_stable_compare(key, &it->key);
		if (comparison > 0) {
			if (it->right == NULL) {
				c = teds_sortedstrictmap_node_alloc(key, value, it);
				teds_sortedstrictmap_node *const next = it->next;

				c->left = NULL;
				c->right = NULL;
				c->prev = it;
				c->next = next;
				it->next = c;
				it->right = c;
				if (next) {
					next->prev = c;
				}
finish_insert:
				tree->nNumOfElements++;
				teds_sortedstrictmap_entries_rebalance_after_insert(tree, c);

				return true;
			}
			it = it->right;
		} else if ((add_new && !ZEND_DEBUG) || comparison < 0) {
			if (it->left == NULL) {
				c = teds_sortedstrictmap_node_alloc(key, value, it);
				teds_sortedstrictmap_node *const prev = it->prev;

				c->left = NULL;
				c->right = NULL;
				c->prev = prev;
				c->next = it;
				it->prev = c;
				it->left = c;
				if (prev) {
					prev->next = c;
				}
				goto finish_insert;
			}
			it = it->left;
		} else {
			ZEND_ASSERT(!add_new);
			/* Overwrite the existing entry in the tree */
			zval old_value;
			ZVAL_COPY_VALUE(&old_value, &it->value);
			ZVAL_COPY(&it->value, value);
			zval_ptr_dtor(&old_value);
			return false;
		}
	}
}

static void teds_sortedstrictmap_tree_clear(teds_sortedstrictmap_tree *array);

/* Used by InternalIterator returned by SortedStrictMap->getIterator() */
typedef struct _teds_sortedstrictmap_it {
	zend_object_iterator       intern;
	teds_sortedstrictmap_node *node;
} teds_sortedstrictmap_it;

static zend_always_inline teds_sortedstrictmap *teds_sortedstrictmap_from_obj(zend_object *obj)
{
	return (teds_sortedstrictmap*)((char*)(obj) - XtOffsetOf(teds_sortedstrictmap, std));
}

#define Z_SORTEDSTRICTMAP_P(zv)  teds_sortedstrictmap_from_obj(Z_OBJ_P((zv)))

static bool teds_sortedstrictmap_tree_uninitialized(teds_sortedstrictmap_tree *array)
{
	if (array->initialized) {
		return false;
	}
	ZEND_ASSERT(array->root == NULL);
	ZEND_ASSERT(array->nNumOfElements == 0);
	return true;
}

static void teds_sortedstrictmap_tree_set_empty_tree(teds_sortedstrictmap_tree *array)
{
	array->root = NULL;
	array->nNumOfElements = 0;
	array->initialized = true;
}

void teds_sortedstrictmap_tree_init_from_array(teds_sortedstrictmap_tree *array, zend_array *values)
{
	zend_long nkey;
	zend_string *skey;
	zval *val;

	teds_sortedstrictmap_tree_set_empty_tree(array);
	ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
		zval key;
		if (skey) {
			ZVAL_STR(&key, skey);
		} else {
			ZVAL_LONG(&key, nkey);
		}
		ZVAL_DEREF(val);
		bool created = teds_sortedstrictmap_tree_insert(array, &key, val, true);
		ZEND_ASSERT(created);
	} ZEND_HASH_FOREACH_END();
}

void teds_sortedstrictmap_tree_init_from_traversable(teds_sortedstrictmap_tree *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_sortedstrictmap_tree_set_empty_tree(array);
	zval tmp_obj;
	ZVAL_OBJ(&tmp_obj, obj);
	iter = ce->get_iterator(ce, &tmp_obj, 0);

	if (UNEXPECTED(EG(exception))) {
		return;
	}

	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (funcs->rewind) {
		funcs->rewind(iter);
		if (UNEXPECTED(EG(exception))) {
			return;
		}
	}

	while (funcs->valid(iter) == SUCCESS) {
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception)) || value == NULL) {
			break;
		}
		zval key;
		if (funcs->get_current_key) {
			funcs->get_current_key(iter, &key);
		} else {
			ZVAL_NULL(&key);
		}

		ZVAL_DEREF(value);
		/* The key's reference count was already increased by get_current_key. We need to free it after attempting to update the entry */
		const bool created_new_entry = teds_sortedstrictmap_tree_insert(array, &key, value, false);
		zval_ptr_dtor(&key);
		if (!created_new_entry) {
			if (UNEXPECTED(EG(exception))) {
				break;
			}
		}

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
	}

	if (iter) {
		zend_iterator_dtor(iter);
	}
	if (UNEXPECTED(EG(exception))) {
		teds_sortedstrictmap_tree_clear(array);
	}
}

static teds_sortedstrictmap_node *teds_sortedstrictmap_node_copy_ctor_recursive(const teds_sortedstrictmap_node *from, teds_sortedstrictmap_node *parent, teds_sortedstrictmap_node *left_parent_node, teds_sortedstrictmap_node *right_parent_node) {
	ZEND_ASSERT(from != NULL);
	teds_sortedstrictmap_node *copy = teds_sortedstrictmap_node_alloc(&from->key, &from->value, parent);
	if (from->left) {
		copy->left = teds_sortedstrictmap_node_copy_ctor_recursive(from->left, copy, left_parent_node, copy);
	} else {
		copy->left = NULL;
		/* This is the first node after left_parent_node */
		copy->prev = left_parent_node;
		if (left_parent_node) {
			left_parent_node->next = copy;
		}
	}
	if (from->right) {
		copy->right = teds_sortedstrictmap_node_copy_ctor_recursive(from->right, copy, copy, right_parent_node);
	} else {
		copy->right = NULL;
		/* This is the last node before right_parent_node */
		copy->next = right_parent_node;
		if (right_parent_node) {
			right_parent_node->prev = copy;
		}
	}
	return copy;
}

static void teds_sortedstrictmap_tree_copy_ctor(teds_sortedstrictmap_tree *to, teds_sortedstrictmap_tree *from)
{
	teds_sortedstrictmap_tree_set_empty_tree(to);
	/* Copy the original tree structure. It will be balanced if the original tree is balanced. */
	to->nNumOfElements = from->nNumOfElements;
	to->initialized = true;
	if (!teds_sortedstrictmap_tree_empty_size(from)) {
		to->root = teds_sortedstrictmap_node_copy_ctor_recursive(from->root, NULL, NULL, NULL);
	} else {
		to->root = NULL;
	}
}

static void teds_sortedstrictmap_node_dtor(teds_sortedstrictmap_node *node)
{
	/* Free keys and values in sorted order */
	while (node != NULL) {
		teds_sortedstrictmap_node_dtor(node->left);
		zval_ptr_dtor(&node->key);
		ZVAL_UNDEF(&node->key);
		zval_ptr_dtor(&node->value);
		teds_sortedstrictmap_node *right = node->right;
		teds_sortedstrictmap_node_release(node);
		node = right;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
void teds_sortedstrictmap_tree_dtor(teds_sortedstrictmap_tree *array)
{
	if (teds_sortedstrictmap_tree_empty_size(array)) {
		return;
	}
	teds_sortedstrictmap_node *root = array->root;
	teds_sortedstrictmap_tree_set_empty_tree(array);
	teds_sortedstrictmap_node_dtor(root);
}

static HashTable* teds_sortedstrictmap_get_gc(zend_object *obj, zval **table, int *table_count)
{
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(obj);
	if (intern->array.nNumOfElements > 0) {
		zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
		zval *key, *val;

		TEDS_SORTEDSTRICTMAP_FOREACH_KEY_VAL(&intern->array, key, val) {
			zend_get_gc_buffer_add_zval(gc_buffer, key);
			zend_get_gc_buffer_add_zval(gc_buffer, val);
		} TEDS_SORTEDSTRICTMAP_FOREACH_END();

		zend_get_gc_buffer_use(gc_buffer, table, table_count);
	}

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_sortedstrictmap_get_properties(zend_object *obj)
{
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(obj);
	const uint32_t len = intern->array.nNumOfElements;
	HashTable *ht = zend_std_get_properties(obj);
	uint32_t old_length = zend_hash_num_elements(ht);
	/* Initialize properties array */
	uint32_t i = 0;
	zval *key, *value;
	TEDS_SORTEDSTRICTMAP_FOREACH_KEY_VAL(&intern->array, key, value) {
		zval tmp;
		Z_TRY_ADDREF_P(key);
		Z_TRY_ADDREF_P(value);
		ZVAL_ARR(&tmp, zend_new_pair(key, value));
		zend_hash_index_update(ht, i, &tmp);
		i++;
	} TEDS_SORTEDSTRICTMAP_FOREACH_END();

	ZEND_ASSERT(i == len);

	for (size_t i = len; i < old_length; i++) {
		zend_hash_index_del(ht, i);
	}

	return ht;
}

static void teds_sortedstrictmap_free_storage(zend_object *object)
{
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(object);
	teds_sortedstrictmap_tree_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_sortedstrictmap_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_sortedstrictmap *intern;

	intern = zend_object_alloc(sizeof(teds_sortedstrictmap), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_SortedStrictMap);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_SortedStrictMap;

	if (orig && clone_orig) {
		teds_sortedstrictmap *other = teds_sortedstrictmap_from_obj(orig);
		teds_sortedstrictmap_tree_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.root = NULL;
	}

	return &intern->std;
}

static zend_object *teds_sortedstrictmap_new(zend_class_entry *class_type)
{
	return teds_sortedstrictmap_new_ex(class_type, NULL, 0);
}


static zend_object *teds_sortedstrictmap_clone(zend_object *old_object)
{
	zend_object *new_object = teds_sortedstrictmap_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static int teds_sortedstrictmap_count_elements(zend_object *object, zend_long *count)
{
	teds_sortedstrictmap *intern;

	intern = teds_sortedstrictmap_from_obj(object);
	*count = intern->array.nNumOfElements;
	return SUCCESS;
}

/* Get number of entries in this SortedStrictMap */
PHP_METHOD(Teds_SortedStrictMap, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(object);
	RETURN_LONG(intern->array.nNumOfElements);
}

/* Get whether this SortedStrictMap is empty */
PHP_METHOD(Teds_SortedStrictMap, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(object);
	RETURN_BOOL(intern->array.nNumOfElements == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_SortedStrictMap, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);

	if (UNEXPECTED(!teds_sortedstrictmap_tree_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\SortedStrictMap::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		teds_sortedstrictmap_tree_set_empty_tree(&intern->array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_sortedstrictmap_tree_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_sortedstrictmap_tree_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_SortedStrictMap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_sortedstrictmap_it_dtor(zend_object_iterator *iter)
{
	teds_sortedstrictmap_node *node = ((teds_sortedstrictmap_it*)iter)->node;
	if (node) {
		teds_sortedstrictmap_node_release(node);
		((teds_sortedstrictmap_it*)iter)->node = NULL;
	}
	zval_ptr_dtor(&iter->data);
}

static void teds_sortedstrictmap_it_rewind(zend_object_iterator *iter)
{
	teds_sortedstrictmap *object   = Z_SORTEDSTRICTMAP_P(&iter->data);
	teds_sortedstrictmap_node *const orig_node = ((teds_sortedstrictmap_it*)iter)->node;
	teds_sortedstrictmap_node *const new_node = teds_sortedstrictmap_tree_get_first(&object->array);
	if (new_node == orig_node) {
		return;
	}
	((teds_sortedstrictmap_it*)iter)->node = new_node;
	if (new_node != NULL) {
		TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(new_node)++;
	}
	if (orig_node != NULL) {
		teds_sortedstrictmap_node_release(orig_node);
	}
}

static zend_always_inline bool teds_sortedstrictmap_node_valid(teds_sortedstrictmap_node *node) {
	/* TODO: Mark key as invalid when removing? */
	return node != NULL && !Z_ISUNDEF(node->key);
}

static int teds_sortedstrictmap_it_valid(zend_object_iterator *iter)
{
	teds_sortedstrictmap_it     *iterator = (teds_sortedstrictmap_it*)iter;
	return teds_sortedstrictmap_node_valid(iterator->node) ? SUCCESS : FAILURE;
}

static zval *teds_sortedstrictmap_it_get_current_data(zend_object_iterator *iter)
{
	teds_sortedstrictmap_node *node = ((teds_sortedstrictmap_it*)iter)->node;
	if (teds_sortedstrictmap_node_valid(node)) {
		return &node->value;
	} else {
		return &EG(uninitialized_zval);
	}
}

static void teds_sortedstrictmap_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_sortedstrictmap_node *node = ((teds_sortedstrictmap_it*)iter)->node;
	if (teds_sortedstrictmap_node_valid(node)) {
		ZVAL_COPY(key, &node->key);
	} else {
		ZVAL_NULL(key);
	}
}

static void teds_sortedstrictmap_it_move_forward(zend_object_iterator *iter)
{
	teds_sortedstrictmap_node *const node = ((teds_sortedstrictmap_it*)iter)->node;
	if (!teds_sortedstrictmap_node_valid(node)) {
		return;
	}
	teds_sortedstrictmap_node *const next = node->next;
	((teds_sortedstrictmap_it*)iter)->node = next;
	if (next) {
		TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(next)++;
	}
	teds_sortedstrictmap_node_release(node);
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_sortedstrictmap_it_funcs = {
	teds_sortedstrictmap_it_dtor,
	teds_sortedstrictmap_it_valid,
	teds_sortedstrictmap_it_get_current_data,
	teds_sortedstrictmap_it_get_current_key,
	teds_sortedstrictmap_it_move_forward,
	teds_sortedstrictmap_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_sortedstrictmap_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_sortedstrictmap_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_sortedstrictmap_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	zend_object *obj = Z_OBJ_P(object);
	ZVAL_OBJ_COPY(&iterator->intern.data, obj);
	iterator->intern.funcs = &teds_sortedstrictmap_it_funcs;
	teds_sortedstrictmap_node *node = teds_sortedstrictmap_tree_get_first(&Z_SORTEDSTRICTMAP_P(object)->array);
	if (node) {
		TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(node)++;
	}
	iterator->node = node;
	(void) ce;

	return &iterator->intern;
}

PHP_METHOD(Teds_SortedStrictMap, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	size_t raw_size = zend_hash_num_elements(raw_data);
	if (UNEXPECTED(raw_size % 2 != 0)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Odd number of elements", 0);
		RETURN_THROWS();
	}
	teds_sortedstrictmap *const intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_tree *const array = &intern->array;
	if (UNEXPECTED(!teds_sortedstrictmap_tree_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	teds_sortedstrictmap_tree_set_empty_tree(array);

	zend_string *str;
	zval key;
	bool is_key = true;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_sortedstrictmap_tree_clear(array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\SortedStrictMap::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
		if (!is_key) {
			teds_sortedstrictmap_tree_insert(array, &key, val, false);
		} else {
			ZVAL_COPY_VALUE(&key, val);
		}
		is_key = !is_key;
	} ZEND_HASH_FOREACH_END();
}

static bool teds_sortedstrictmap_tree_insert_from_pair(teds_sortedstrictmap_tree *array, zval *raw_val)
{
	ZVAL_DEREF(raw_val);
	if (UNEXPECTED(Z_TYPE_P(raw_val) != IS_ARRAY)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find pair in array but got non-array", 0);
		return false;
	}
	HashTable *ht = Z_ARRVAL_P(raw_val);
	zval *key = zend_hash_index_find(ht, 0);
	if (UNEXPECTED(!key)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find key at index 0", 0);
		return false;
	}
	zval *value = zend_hash_index_find(ht, 1);
	if (UNEXPECTED(!value)) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Expected to find value at index 1", 0);
		return false;
	}
	ZVAL_DEREF(key);
	ZVAL_DEREF(value);
	teds_sortedstrictmap_tree_insert(array, key, value, false);
	return true;
}

static void teds_sortedstrictmap_tree_init_from_array_pairs(teds_sortedstrictmap_tree *array, zend_array *raw_data)
{
	size_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		teds_sortedstrictmap_tree_set_empty_tree(array);
		return;
	}
	teds_sortedstrictmap_tree_set_empty_tree(array);
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		if (!teds_sortedstrictmap_tree_insert_from_pair(array, val)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();
}

static void teds_sortedstrictmap_tree_init_from_traversable_pairs(teds_sortedstrictmap_tree *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_sortedstrictmap_tree_set_empty_tree(array);
	zval tmp_obj;
	ZVAL_OBJ(&tmp_obj, obj);
	iter = ce->get_iterator(ce, &tmp_obj, 0);

	if (UNEXPECTED(EG(exception))) {
		return;
	}

	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (funcs->rewind) {
		funcs->rewind(iter);
		if (UNEXPECTED(EG(exception))) {
			return;
		}
	}
	while (funcs->valid(iter) == SUCCESS) {
		if (EG(exception)) {
			break;
		}
		zval *pair = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		if (!teds_sortedstrictmap_tree_insert_from_pair(array, pair)) {
			break;
		}

		iter->index++;
		funcs->move_forward(iter);
		if (EG(exception)) {
			break;
		}
	}

	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static zend_object* create_from_pairs(zval *iterable) {
	zend_object *object = teds_sortedstrictmap_new(teds_ce_SortedStrictMap);
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(object);
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_sortedstrictmap_tree_init_from_array_pairs(&intern->array, Z_ARRVAL_P(iterable));
			break;
		case IS_OBJECT:
			teds_sortedstrictmap_tree_init_from_traversable_pairs(&intern->array, Z_OBJ_P(iterable));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return object;
}

PHP_METHOD(Teds_SortedStrictMap, fromPairs)
{
	zval *iterable;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_OBJ(create_from_pairs(iterable));
}

PHP_METHOD(Teds_SortedStrictMap, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_sortedstrictmap_new(teds_ce_SortedStrictMap);
	teds_sortedstrictmap *intern = teds_sortedstrictmap_from_obj(object);
	teds_sortedstrictmap_tree_init_from_array_pairs(&intern->array, array_ht);

	RETURN_OBJ(object);
}

PHP_METHOD(Teds_SortedStrictMap, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);

	uint32_t len = intern->array.nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *flat_entries_array = zend_new_array(len * 2);
	/* Initialize return array */
	zend_hash_real_init_packed(flat_entries_array);

	/* Go through entries and add keys and values to the return array */
	zval *key, *value;
	TEDS_SORTEDSTRICTMAP_FOREACH_KEY_VAL(&intern->array, key, value) {
		Z_TRY_ADDREF_P(key);
		zend_hash_next_index_insert(flat_entries_array, key);
		Z_TRY_ADDREF_P(value);
		zend_hash_next_index_insert(flat_entries_array, value);
	} TEDS_SORTEDSTRICTMAP_FOREACH_END();

	RETURN_ARR(flat_entries_array);
}

#define IMPLEMENT_READ_POSITION_PHP_METHOD(methodName, pos, propName) \
PHP_METHOD(Teds_SortedStrictMap, methodName) \
{ \
       ZEND_PARSE_PARAMETERS_NONE(); \
       const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS); \
       if (teds_sortedstrictmap_tree_empty_size(&intern->array)) { \
               zend_throw_exception(spl_ce_UnderflowException, "Cannot read " # methodName " of empty SortedStrictMap", 0); \
               RETURN_THROWS(); \
       } \
       teds_sortedstrictmap_node *node = teds_sortedstrictmap_tree_get_ ## pos(&intern->array); \
       RETVAL_COPY(&node->propName); \
}

IMPLEMENT_READ_POSITION_PHP_METHOD(bottom, first, value)
IMPLEMENT_READ_POSITION_PHP_METHOD(bottomKey, first, key)
IMPLEMENT_READ_POSITION_PHP_METHOD(top, last, value)
IMPLEMENT_READ_POSITION_PHP_METHOD(topKey, last, key)

#define IMPLEMENT_REMOVE_POSITION_PHP_METHOD(methodName, pos) \
PHP_METHOD(Teds_SortedStrictMap, methodName) \
{ \
	ZEND_PARSE_PARAMETERS_NONE(); \
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS); \
	if (teds_sortedstrictmap_tree_empty_size(&intern->array)) { \
		zend_throw_exception(spl_ce_UnderflowException, "Cannot " # methodName " from empty SortedStrictMap", 0); \
		RETURN_THROWS(); \
	} \
	teds_sortedstrictmap_node *node = teds_sortedstrictmap_tree_get_ ## pos(&intern->array); \
	zend_array *result = zend_new_pair(&node->key, &node->value); \
	teds_sortedstrictmap_tree_remove_node(&intern->array, node, false); \
	RETVAL_ARR(result); \
}

IMPLEMENT_REMOVE_POSITION_PHP_METHOD(pop, last)
IMPLEMENT_REMOVE_POSITION_PHP_METHOD(shift, first)

PHP_METHOD(Teds_SortedStrictMap, keys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	uint32_t len = intern->array.nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *keys = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(keys);

	/* Go through keys and add values to the return array */
	ZEND_HASH_FILL_PACKED(keys) {
		zval *key;
		TEDS_SORTEDSTRICTMAP_FOREACH_KEY(&intern->array, key) {
			Z_TRY_ADDREF_P(key);
			ZEND_HASH_FILL_ADD(key);
		} TEDS_SORTEDSTRICTMAP_FOREACH_END();
	} ZEND_HASH_FILL_END();
	RETURN_ARR(keys);
}

PHP_METHOD(Teds_SortedStrictMap, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	size_t len = intern->array.nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		zval *value;
		TEDS_SORTEDSTRICTMAP_FOREACH_VAL(&intern->array, value) {
			Z_TRY_ADDREF_P(value);
			ZEND_HASH_FILL_ADD(value);
		} TEDS_SORTEDSTRICTMAP_FOREACH_END();
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

static teds_sortedstrictmap_node *teds_sortedstrictmap_tree_find_value(const teds_sortedstrictmap_tree *array, zval *value)
{
	teds_sortedstrictmap_node *it;
	TEDS_SORTEDSTRICTMAP_FOREACH_NODE(array, it) {
		if (zend_is_identical(value, &it->value)) {
			return it;
		}
	} TEDS_SORTEDSTRICTMAP_FOREACH_END();
	return NULL;
}

static void teds_sortedstrictmap_tree_replace_node_with_child(teds_sortedstrictmap_tree *tree, teds_sortedstrictmap_node *removed_node, teds_sortedstrictmap_node *child_node) {
	teds_sortedstrictmap_node *const parent = removed_node->parent;
	if (child_node) {
		child_node->parent = parent;
	}
	if (parent == NULL) {
		ZEND_ASSERT(removed_node == tree->root);
		tree->root = child_node;
		return;
	}
	if (parent->left == removed_node) {
		parent->left = child_node;
	} else {
		ZEND_ASSERT(parent->right == removed_node);
		parent->right = child_node;
	}
}

static zend_always_inline teds_sortedstrictmap_node *teds_sortedstrictmap_node_remove_leftmost(teds_sortedstrictmap_node *node) {
	while (true) {
		ZEND_ASSERT(node != NULL);
		ZEND_ASSERT(node->parent != NULL);
		ZEND_ASSERT(node->parent != node);
		if (node->left) {
			ZEND_ASSERT(node->left->parent == node);
			node = node->left;
			continue;
		}
		teds_sortedstrictmap_node *const parent = node->parent;
		teds_sortedstrictmap_node *const right = node->right;
		if (right) {
			ZEND_ASSERT(right->parent == node);
			right->parent = parent;
			node->right = NULL;
		}
		if (parent->left == node) {
			parent->left = right;
		} else {
			ZEND_ASSERT(parent->right == node);
			parent->right = right;
		}
		node->parent = NULL;
		return node;
	}
}

static zend_always_inline void teds_sortedstrictmap_tree_remove_node(teds_sortedstrictmap_tree *tree, teds_sortedstrictmap_node *const node, bool free_zvals) {
	teds_sortedstrictmap_node *const prev = node->prev;
	teds_sortedstrictmap_node *const next = node->next;
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}
	node->prev = NULL;
	node->next = NULL;

	if (!node->left) {
		teds_sortedstrictmap_tree_replace_node_with_child(tree, node, node->right);
	} else if (!node->right) {
		ZEND_ASSERT(node->left->parent == node);
		teds_sortedstrictmap_tree_replace_node_with_child(tree, node, node->left);
	} else {
		ZEND_ASSERT(node->right->parent == node);
		teds_sortedstrictmap_node *const replacement = teds_sortedstrictmap_node_remove_leftmost(node->right);
		teds_sortedstrictmap_node *const parent = node->parent;
		ZEND_ASSERT(replacement != node);
		replacement->left = node->left;
		if (node->left) {
			ZEND_ASSERT(node->left != replacement);
			node->left->parent = replacement;
		}
		if (node->right) {
			ZEND_ASSERT(node->right != replacement);
			node->right->parent = replacement;
		}
		replacement->right = node->right;
		replacement->parent = parent;
		if (parent == NULL) {
			ZEND_ASSERT(tree->root == node);
			tree->root = replacement;
		} else {
			if (parent->left == node) {
				parent->left = replacement;
			} else {
				ZEND_ASSERT(parent->right == node);
				parent->right = replacement;
			}
		}
	}

	ZEND_ASSERT(tree->nNumOfElements > 0);
	tree->nNumOfElements--;
	if (free_zvals) {
		zval_ptr_dtor(&node->key);
		zval_ptr_dtor(&node->value);
	}
	ZVAL_UNDEF(&node->key);
	teds_sortedstrictmap_node_release(node);
}

static bool teds_sortedstrictmap_tree_remove_key(teds_sortedstrictmap_tree *tree, zval *key)
{
	/* FIXME implement binary tree removal */
	teds_sortedstrictmap_node *const node = teds_sortedstrictmap_tree_find_key(tree, key);
	if (node == NULL) {
		/* Nothing to remove */
		return false;
	}
	teds_sortedstrictmap_tree_remove_node(tree, node, true);
	return true;
}

PHP_METHOD(Teds_SortedStrictMap, offsetExists)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (intern->array.nNumOfElements > 0) {
		teds_sortedstrictmap_node *entry = teds_sortedstrictmap_tree_find_key(&intern->array, key);
		if (entry) {
			RETURN_BOOL(Z_TYPE(entry->value) != IS_NULL);
		}
	}
	RETURN_FALSE;
}

PHP_METHOD(Teds_SortedStrictMap, offsetGet)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (intern->array.nNumOfElements > 0) {
		teds_sortedstrictmap_node *entry = teds_sortedstrictmap_tree_find_key(&intern->array, key);
		if (entry) {
			RETURN_COPY(&entry->value);
		}
	}
	zend_throw_exception(spl_ce_OutOfBoundsException, "Key not found", 0);
	RETURN_THROWS();
}

PHP_METHOD(Teds_SortedStrictMap, get)
{
	zval *key;
	zval *default_zv = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_zv)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (intern->array.nNumOfElements > 0) {
		teds_sortedstrictmap_node *entry = teds_sortedstrictmap_tree_find_key(&intern->array, key);
		if (entry) {
			RETURN_COPY(&entry->value);
		}
	}
	if (default_zv != NULL) {
		RETURN_COPY(default_zv);
	}
	zend_throw_exception(spl_ce_OutOfBoundsException, "Key not found", 0);
	RETURN_THROWS();
}

PHP_METHOD(Teds_SortedStrictMap, offsetSet)
{
	zval *key;
	zval *value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_tree_insert(&intern->array, key, value, false);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_SortedStrictMap, offsetUnset)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_tree_remove_key(&intern->array, key);
	TEDS_RETURN_VOID();
}

PHP_METHOD(Teds_SortedStrictMap, containsValue)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_node *entry = teds_sortedstrictmap_tree_find_value(&intern->array, value);
	RETURN_BOOL(entry != NULL);
}

PHP_METHOD(Teds_SortedStrictMap, containsKey)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (intern->array.nNumOfElements > 0) {
		teds_sortedstrictmap_node *entry = teds_sortedstrictmap_tree_find_key(&intern->array, key);
		RETURN_BOOL(entry != NULL);
	}
	RETURN_FALSE;
}

static void teds_sortedstrictmap_tree_return_pairs(teds_sortedstrictmap_tree *array, zval *return_value)
{
	uint32_t len = array->nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}

	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		zval *key, *val;
		TEDS_SORTEDSTRICTMAP_FOREACH_KEY_VAL(array, key, val) {
			zval tmp;
			Z_TRY_ADDREF_P(key);
			Z_TRY_ADDREF_P(val);
			ZVAL_ARR(&tmp, zend_new_pair(key, val));
			ZEND_HASH_FILL_ADD(&tmp);
		} TEDS_SORTEDSTRICTMAP_FOREACH_END();
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

PHP_METHOD(Teds_SortedStrictMap, toPairs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_tree_return_pairs(&intern->array, return_value);
}

static ZEND_COLD void teds_sortedstrictmap_node_debug_representation(zval *return_value, teds_sortedstrictmap_node *node, zend_string *strings[5]) {
	if (!node) {
		RETURN_EMPTY_ARRAY();
	}
	if (node->parent) {
		ZEND_ASSERT(node != node->parent);
		if (node->parent->parent) {
			ZEND_ASSERT(node != node->parent->parent);
		}
	}
	zend_array *ht = zend_new_array(5);
	Z_TRY_ADDREF(node->key);
	zend_hash_add_new(ht, strings[0], &node->key);

	Z_TRY_ADDREF(node->value);
	zend_hash_add_new(ht, strings[1], &node->value);

	zval tmp;
	ZVAL_BOOL(&tmp, TEDS_SORTEDSTRICTMAP_NODE_COLOR(node) == TEDS_NODE_RED);
	zend_hash_add_new(ht, strings[2], &tmp);

	teds_sortedstrictmap_node_debug_representation(&tmp, node->left, strings);
	zend_hash_add_new(ht, strings[3], &tmp);

	teds_sortedstrictmap_node_debug_representation(&tmp, node->right, strings);
	zend_hash_add_new(ht, strings[4], &tmp);
	RETURN_ARR(ht);
}

ZEND_COLD PHP_METHOD(Teds_SortedStrictMap, debugGetTreeRepresentation)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	if (intern->array.nNumOfElements == 0) {
		RETURN_EMPTY_ARRAY();
	}
	zend_string *strings[5] = {
		zend_string_init_interned("key", sizeof("key") - 1, 0),
		zend_string_init_interned("value", sizeof("value") - 1, 0),
		zend_string_init_interned("red", sizeof("red") - 1, 0),
		zend_string_init_interned("left", sizeof("left") - 1, 0),
		zend_string_init_interned("right", sizeof("right") - 1, 0),
	};
	teds_sortedstrictmap_node_debug_representation(return_value, intern->array.root, strings);
}

static void teds_sortedstrictmap_tree_clear(teds_sortedstrictmap_tree *array) {
	if (teds_sortedstrictmap_tree_empty_size(array)) {
		return;
	}
	teds_sortedstrictmap_tree array_copy = *array;

	teds_sortedstrictmap_tree_set_empty_tree(array);

	teds_sortedstrictmap_tree_dtor(&array_copy);
	/* Could call teds_sortedstrictmap_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_SortedStrictMap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	teds_sortedstrictmap_tree_clear(&intern->array);
	TEDS_RETURN_VOID();
}

PHP_MINIT_FUNCTION(teds_sortedstrictmap)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_SortedStrictMap = register_class_Teds_SortedStrictMap(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
	teds_ce_SortedStrictMap->create_object = teds_sortedstrictmap_new;

	memcpy(&teds_handler_SortedStrictMap, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_SortedStrictMap.offset          = XtOffsetOf(teds_sortedstrictmap, std);
	teds_handler_SortedStrictMap.clone_obj       = teds_sortedstrictmap_clone;
	teds_handler_SortedStrictMap.count_elements  = teds_sortedstrictmap_count_elements;
	teds_handler_SortedStrictMap.get_properties  = teds_sortedstrictmap_get_properties;
	teds_handler_SortedStrictMap.get_gc          = teds_sortedstrictmap_get_gc;
	teds_handler_SortedStrictMap.dtor_obj        = zend_objects_destroy_object;
	teds_handler_SortedStrictMap.free_obj        = teds_sortedstrictmap_free_storage;

	teds_ce_SortedStrictMap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_SortedStrictMap->get_iterator = teds_sortedstrictmap_get_iterator;

	return SUCCESS;
}
