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
#include "teds_sortedstrictset_arginfo.h"
#include "teds_sortedstrictset.h"
#include "teds_util.h"
#include "teds_interfaces.h"
#include "teds.h"
// #include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"

#include <stdbool.h>

#define RBTREE_INVARIANT_ASSERT(cond) ZEND_ASSERT((cond))
#define DEBUG_PRINTF(...) do { } while (0)

zend_object_handlers teds_handler_SortedStrictSet;
zend_class_entry *teds_ce_SortedStrictSet;

static zend_always_inline teds_sortedstrictset_node *teds_sortedstrictset_tree_find_key(const teds_sortedstrictset_tree *tree, zval *key)
{
	teds_sortedstrictset_node *it = tree->root;
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

/*
static void teds_sortedstrictset_node_debug_check_linked_to_parent(const teds_sortedstrictset_node *node) {
#if ZEND_DEBUG
	if (!node) {
		return;
	}

	DEBUG_PRINTF("teds_sortedstrictset_node_debug_check_linked_to_parent %p\n", node);
	if (node->left) {
		ZEND_ASSERT(node->left->parent == node);
		teds_sortedstrictset_node_debug_check_linked_to_parent(node->left);
	}
	if (node->right) {
		ZEND_ASSERT(node->right->parent == node);
		teds_sortedstrictset_node_debug_check_linked_to_parent(node->right);
	}
#endif
}
*/

static zend_always_inline void teds_sortedstrictset_tree_remove_node(teds_sortedstrictset_tree *array, teds_sortedstrictset_node *const node, bool free_zvals);

static zend_always_inline teds_sortedstrictset_node *teds_sortedstrictset_node_alloc(const zval *key, teds_sortedstrictset_node *parent) {
	ZEND_ASSERT(!Z_ISUNDEF_P(key));
	teds_sortedstrictset_node *n = emalloc(sizeof(teds_sortedstrictset_node));
	n->parent = parent;
	ZVAL_COPY(&n->key, key);
	TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(n) = 1;
	TEDS_SORTEDSTRICTSET_NODE_COLOR(n) = TEDS_NODE_RED;
	return n;
}

/* Given a node, rotate it so that the child node becomes the parent of that node.
 * teds_sortedstrictset_tree_rotate_dir_root should be used if node might be the root node. */
static zend_always_inline void teds_sortedstrictset_node_rotate_dir(teds_sortedstrictset_node *const node, int dir) {
	teds_sortedstrictset_node *const c = node->children[dir];
	teds_sortedstrictset_node *const parent = node->parent;
	teds_sortedstrictset_node *const transfer = c->children[1 - dir];

	ZEND_ASSERT(transfer == NULL || transfer->parent == c);
	ZEND_ASSERT(parent != NULL);
	if (parent->children[dir] == node) {
		parent->children[dir] = c;
	} else {
		ZEND_ASSERT(parent->children[1 - dir] == node);
		parent->children[1 - dir] = c;
	}
	c->parent = parent;

	c->children[1 - dir] = node;
	node->parent = c;

	node->children[dir] = transfer;
	if (transfer != NULL) {
		transfer->parent = node;
	}
}

static zend_always_inline void teds_sortedstrictset_tree_rotate_dir_root(teds_sortedstrictset_tree *const tree, teds_sortedstrictset_node *const node, int dir) {
	teds_sortedstrictset_node *const c = node->children[dir];
	ZEND_ASSERT(c != NULL);
	teds_sortedstrictset_node *const parent = node->parent;
	teds_sortedstrictset_node *const transfer = c->children[1 - dir];
	ZEND_ASSERT(transfer == NULL || transfer->parent == c);

	if (parent != NULL) {
		if (parent->children[dir] == node) {
			parent->children[dir] = c;
		} else {
			ZEND_ASSERT(parent->children[1 - dir] == node);
			parent->children[1 - dir] = c;
		}
	} else {
		ZEND_ASSERT(tree->root == node);
		tree->root = c;
	}
	c->parent = parent;

	c->children[1 - dir] = node;
	node->parent = c;

	node->children[dir] = transfer;
	if (transfer != NULL) {
		transfer->parent = node;
	}
}

static zend_always_inline void teds_sortedstrictset_entries_rebalance_after_insert(teds_sortedstrictset_tree *tree, teds_sortedstrictset_node *node) {
	while (true) {
		teds_sortedstrictset_node *parent = node->parent;
		// ZEND_ASSERT(TEDS_SORTEDSTRICTSET_NODE_COLOR(node) == TEDS_NODE_RED);

		/* Based on https://en.wikipedia.org/wiki/Red%E2%80%93black_tree */
		if (parent == NULL || TEDS_SORTEDSTRICTSET_NODE_COLOR(parent) == TEDS_NODE_BLACK) {
			return;
		}

		teds_sortedstrictset_node *const grandparent = parent->parent;
		if (grandparent == NULL) {
			TEDS_SORTEDSTRICTSET_NODE_COLOR(parent) = TEDS_NODE_BLACK;
			return;
		}
		/* In a valid tree, the grandparent node would be black if parent was red */

		const int dir = grandparent->right == parent ? 1 : 0;
		ZEND_ASSERT(grandparent->children[dir] == parent);
		teds_sortedstrictset_node *uncle;
		if (dir) {
			uncle = grandparent->left;
		} else {
			uncle = grandparent->right;
		}

		if (uncle && TEDS_SORTEDSTRICTSET_NODE_COLOR(uncle) == TEDS_NODE_RED) {
			TEDS_SORTEDSTRICTSET_NODE_COLOR(uncle) = TEDS_NODE_BLACK;
			TEDS_SORTEDSTRICTSET_NODE_COLOR(parent) = TEDS_NODE_BLACK;
			TEDS_SORTEDSTRICTSET_NODE_COLOR(grandparent) = TEDS_NODE_RED;
			node = grandparent;
			continue;
		}
		/* Parent node is red but the uncle node is black. */
		/* In a valid tree, the grandparent node would be black */
		if (node == parent->children[1 - dir]) {
			teds_sortedstrictset_node_rotate_dir(parent, 1 - dir);
			teds_sortedstrictset_node *orig_node = node;
			node = parent;
			parent = orig_node;
		}
		ZEND_ASSERT(grandparent->children[dir] == parent);
		ZEND_ASSERT(parent->children[dir] == node);
		ZEND_ASSERT(parent->parent == grandparent);
		ZEND_ASSERT(node->parent == parent);

		teds_sortedstrictset_tree_rotate_dir_root(tree, grandparent, dir);
		TEDS_SORTEDSTRICTSET_NODE_COLOR(parent) = TEDS_NODE_BLACK;
		TEDS_SORTEDSTRICTSET_NODE_COLOR(grandparent) = TEDS_NODE_RED;
		return;
	}
}

static zend_always_inline void teds_sortedstrictset_node_release(teds_sortedstrictset_node *node) {
	ZEND_ASSERT(node != NULL);
	ZEND_ASSERT(TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(node) > 0);
	if (EXPECTED(TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(node) == 1)) {
		ZEND_ASSERT(Z_ISUNDEF(node->key));
		efree_size(node, sizeof(teds_sortedstrictset_node));
	} else {
		--TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(node);
	}
}

/* Returns true if a new entry was added to the set, false if updated. Based on _zend_hash_add_or_update_i. */
static zend_always_inline bool teds_sortedstrictset_tree_insert(teds_sortedstrictset_tree *tree, zval *key, bool add_new)
{
	ZEND_ASSERT(Z_TYPE_P(key) != IS_UNDEF);

	/* TODO optimize */
	teds_sortedstrictset_node *it = tree->root;
	if (it == NULL) {
		/* Initialize this tree as a new binary search tree of size 1 */
		it = teds_sortedstrictset_node_alloc(key, NULL);
		tree->root = it;
		it->left = NULL;
		it->right = NULL;
		it->prev = NULL;
		it->next = NULL;
		tree->nNumOfElements++;
		return true;
	}
	/* c must be declared in outer scope for goto to work. */
	teds_sortedstrictset_node *c;
	while (true) {
		const int comparison = teds_stable_compare(key, &it->key);
		if (comparison > 0) {
			if (it->right == NULL) {
				c = teds_sortedstrictset_node_alloc(key, it);
				teds_sortedstrictset_node *const next = it->next;

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
				teds_sortedstrictset_entries_rebalance_after_insert(tree, c);

				return true;
			}
			it = it->right;
		} else if ((add_new && !ZEND_DEBUG) || comparison < 0) {
			if (it->left == NULL) {
				c = teds_sortedstrictset_node_alloc(key, it);
				teds_sortedstrictset_node *const prev = it->prev;

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
			/* Already exists */
			return false;
		}
	}
}

static void teds_sortedstrictset_tree_clear(teds_sortedstrictset_tree *array);

/* Used by InternalIterator returned by SortedStrictSet->getIterator() */
typedef struct _teds_sortedstrictset_it {
	zend_object_iterator       intern;
	teds_sortedstrictset_node *node;
} teds_sortedstrictset_it;

static zend_always_inline teds_sortedstrictset *teds_sortedstrictset_from_obj(zend_object *obj)
{
	return (teds_sortedstrictset*)((char*)(obj) - XtOffsetOf(teds_sortedstrictset, std));
}

#define Z_SORTEDSTRICTSET_P(zv)  teds_sortedstrictset_from_obj(Z_OBJ_P((zv)))

static zend_always_inline bool teds_sortedstrictset_tree_uninitialized(teds_sortedstrictset_tree *array)
{
	if (array->initialized) {
		return false;
	}
	ZEND_ASSERT(array->root == NULL);
	ZEND_ASSERT(array->nNumOfElements == 0);
	return true;
}

static zend_always_inline void teds_sortedstrictset_tree_set_empty_tree(teds_sortedstrictset_tree *array)
{
	array->root = NULL;
	array->nNumOfElements = 0;
	array->initialized = true;
}

static teds_sortedstrictset_node *teds_sortedstrictset_node_build_tree_from_sorted_nodes_helper(teds_sortedstrictset_node **nodes, const uint32_t n, teds_sortedstrictset_node *left_parent, teds_sortedstrictset_node *right_parent, int leaf_depth)
{
	ZEND_ASSERT(n > 0);
	const uint32_t mid = n/2;
	teds_sortedstrictset_node *const root = nodes[mid];
	ZEND_ASSERT(leaf_depth >= 0);
	TEDS_SORTEDSTRICTSET_NODE_COLOR(root) = (leaf_depth == 0 ? TEDS_NODE_RED : TEDS_NODE_BLACK);
	leaf_depth--;
	{
		if (mid > 0) {
			teds_sortedstrictset_node *const left = teds_sortedstrictset_node_build_tree_from_sorted_nodes_helper(nodes, mid, left_parent, root, leaf_depth);
			root->left = left;
			left->parent = root;
			ZEND_ASSERT(root != left);
		} else {
			root->left = NULL;
			/* This is the first node after left_parent */
			root->prev = left_parent;
			if (left_parent) {
				left_parent->next = root;
			}
		}
	}

	{
		const uint32_t right_count = n - mid - 1;
		if (right_count > 0) {
			teds_sortedstrictset_node *const right = teds_sortedstrictset_node_build_tree_from_sorted_nodes_helper(nodes + mid + 1, right_count, root, right_parent, leaf_depth);
			root->right = right;
			ZEND_ASSERT(root != right);
			right->parent = root;
		} else {
			root->right = NULL;
			/* This is the last node before right_parent */
			root->next = right_parent;
			if (right_parent) {
				right_parent->prev = root;
			}
		}
	}
	return root;
}

static teds_sortedstrictset_node *teds_sortedstrictset_node_build_tree_from_sorted_nodes(teds_sortedstrictset_node **nodes, const uint32_t n)
{
	ZEND_ASSERT(n >= 1);
	int leaf_depth = 1;
	uint32_t i = n + 1;
	/* for n = 1..2, leaf_depth is 1 (first layer is black) */
	/* for n = 3..6, leaf_depth is 2 (first 2 layers are black) */
	/* for n = 7..14, leaf_depth is 3 */
	while (i >= 3) {
		leaf_depth++;
		i >>= 1;
	}
	return teds_sortedstrictset_node_build_tree_from_sorted_nodes_helper(nodes, n, NULL, NULL, leaf_depth);
}


void teds_sortedstrictset_tree_init_from_array(teds_sortedstrictset_tree *array, zend_array *values)
{
	zval *val;

	teds_sortedstrictset_tree_set_empty_tree(array);
	if (values->nNumOfElements == 0) {
		return;
	}

	/* Optimization for constructing this from a sorted array */
	teds_sortedstrictset_node **nodes = emalloc(values->nNumOfElements * sizeof(teds_sortedstrictset_node*));
	teds_sortedstrictset_node *prev = NULL;
	uint32_t i = 0;

	ZEND_HASH_FOREACH_VAL(values, val)  {
		ZVAL_DEREF(val);

		if (nodes != NULL) {
			if (i == 0 || teds_stable_compare(val, &prev->key) > 0) {
				prev = teds_sortedstrictset_node_alloc(val, NULL);
				nodes[i] = prev;
				i++;
				continue;
			}
			array->root = teds_sortedstrictset_node_build_tree_from_sorted_nodes(nodes, i);
			array->nNumOfElements = i;
			efree(nodes);
			nodes = NULL;
		}
		teds_sortedstrictset_tree_insert(array, val, false);
	} ZEND_HASH_FOREACH_END();

	if (nodes != NULL) {
		array->root = teds_sortedstrictset_node_build_tree_from_sorted_nodes(nodes, i);
		array->nNumOfElements = i;
		efree(nodes);
	}
}

void teds_sortedstrictset_tree_init_from_traversable(teds_sortedstrictset_tree *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_sortedstrictset_tree_set_empty_tree(array);
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

		ZVAL_DEREF(value);
		const bool created_new_entry = teds_sortedstrictset_tree_insert(array, value, false);
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
		teds_sortedstrictset_tree_clear(array);
	}
}

static teds_sortedstrictset_node *teds_sortedstrictset_node_copy_ctor_recursive(const teds_sortedstrictset_node *from, teds_sortedstrictset_node *parent, teds_sortedstrictset_node *left_parent_node, teds_sortedstrictset_node *right_parent_node) {
	ZEND_ASSERT(from != NULL);
	teds_sortedstrictset_node *copy = teds_sortedstrictset_node_alloc(&from->key, parent);
	if (from->left) {
		copy->left = teds_sortedstrictset_node_copy_ctor_recursive(from->left, copy, left_parent_node, copy);
	} else {
		copy->left = NULL;
		/* This is the first node after left_parent_node */
		copy->prev = left_parent_node;
		if (left_parent_node) {
			left_parent_node->next = copy;
		}
	}
	if (from->right) {
		copy->right = teds_sortedstrictset_node_copy_ctor_recursive(from->right, copy, copy, right_parent_node);
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

static void teds_sortedstrictset_tree_copy_ctor(teds_sortedstrictset_tree *to, teds_sortedstrictset_tree *from)
{
	teds_sortedstrictset_tree_set_empty_tree(to);
	/* Copy the original tree structure. It will be balanced if the original tree is balanced. */
	to->nNumOfElements = from->nNumOfElements;
	to->initialized = true;
	if (!teds_sortedstrictset_tree_empty_size(from)) {
		to->root = teds_sortedstrictset_node_copy_ctor_recursive(from->root, NULL, NULL, NULL);
	} else {
		to->root = NULL;
	}
}

static void teds_sortedstrictset_node_dtor(teds_sortedstrictset_node *node)
{
	/* Free keys in sorted order */
	while (node != NULL) {
		teds_sortedstrictset_node_dtor(node->left);
		teds_sortedstrictset_node *right = node->right;
		zval_ptr_dtor(&node->key);
		ZVAL_UNDEF(&node->key);
		teds_sortedstrictset_node_release(node);
		node = right;
	}
}

/* Destructs and frees contents and the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
void teds_sortedstrictset_tree_dtor(teds_sortedstrictset_tree *array)
{
	if (teds_sortedstrictset_tree_empty_size(array)) {
		return;
	}
	teds_sortedstrictset_node *root = array->root;
	teds_sortedstrictset_tree_set_empty_tree(array);
	teds_sortedstrictset_node_dtor(root);
}

static HashTable* teds_sortedstrictset_get_gc(zend_object *obj, zval **table, int *table_count)
{
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (intern->array.nNumOfElements > 0) {
		zval *key;

		TEDS_SORTEDSTRICTSET_FOREACH_KEY(&intern->array, key) {
			zend_get_gc_buffer_add_zval(gc_buffer, key);
		} TEDS_SORTEDSTRICTSET_FOREACH_END();

	}
	/* Overwrites table and table_count. The caller does not initialize these. */
	zend_get_gc_buffer_use(gc_buffer, table, table_count);

	// Returning the object's properties is redundant if dynamic properties are not allowed,
	// and this can't be subclassed.
	return NULL;
}

static HashTable* teds_sortedstrictset_get_properties(zend_object *obj)
{
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(obj);
	const uint32_t len = intern->array.nNumOfElements;
	HashTable *ht = zend_std_get_properties(obj);
	uint32_t old_length = zend_hash_num_elements(ht);
	/* Initialize properties array */
	uint32_t i = 0;
	zval *key;
	TEDS_SORTEDSTRICTSET_FOREACH_KEY(&intern->array, key) {
		Z_TRY_ADDREF_P(key);
		zend_hash_index_update(ht, i, key);
		i++;
	} TEDS_SORTEDSTRICTSET_FOREACH_END();

	ZEND_ASSERT(i == len);

	for (size_t i = len; i < old_length; i++) {
		zend_hash_index_del(ht, i);
	}

	return ht;
}

static void teds_sortedstrictset_free_storage(zend_object *object)
{
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(object);
	teds_sortedstrictset_tree_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_sortedstrictset_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_sortedstrictset *intern;

	intern = zend_object_alloc(sizeof(teds_sortedstrictset), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_SortedStrictSet);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_SortedStrictSet;

	if (orig && clone_orig) {
		teds_sortedstrictset *other = teds_sortedstrictset_from_obj(orig);
		teds_sortedstrictset_tree_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.root = NULL;
	}

	return &intern->std;
}

static zend_object *teds_sortedstrictset_new(zend_class_entry *class_type)
{
	return teds_sortedstrictset_new_ex(class_type, NULL, 0);
}


static zend_object *teds_sortedstrictset_clone(zend_object *old_object)
{
	zend_object *new_object = teds_sortedstrictset_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_sortedstrictset_count_elements(zend_object *object, zend_long *count)
{
	teds_sortedstrictset *intern;

	intern = teds_sortedstrictset_from_obj(object);
	*count = intern->array.nNumOfElements;
	return SUCCESS;
}

/* Get number of entries in this SortedStrictSet */
PHP_METHOD(Teds_SortedStrictSet, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(object);
	RETURN_LONG(intern->array.nNumOfElements);
}

/* Get whether this SortedStrictSet is empty */
PHP_METHOD(Teds_SortedStrictSet, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(object);
	RETURN_BOOL(intern->array.nNumOfElements == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_SortedStrictSet, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);

	if (UNEXPECTED(!teds_sortedstrictset_tree_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\SortedStrictSet::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		teds_sortedstrictset_tree_set_empty_tree(&intern->array);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_sortedstrictset_tree_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_sortedstrictset_tree_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_SortedStrictSet, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_sortedstrictset_it_dtor(zend_object_iterator *iter)
{
	teds_sortedstrictset_node *node = ((teds_sortedstrictset_it*)iter)->node;
	if (node) {
		teds_sortedstrictset_node_release(node);
		((teds_sortedstrictset_it*)iter)->node = NULL;
	}
	zval_ptr_dtor(&iter->data);
}

static void teds_sortedstrictset_it_rewind(zend_object_iterator *iter)
{
	teds_sortedstrictset *object   = Z_SORTEDSTRICTSET_P(&iter->data);
	teds_sortedstrictset_node *const orig_node = ((teds_sortedstrictset_it*)iter)->node;
	teds_sortedstrictset_node *const new_node = teds_sortedstrictset_tree_get_first(&object->array);
	if (new_node == orig_node) {
		return;
	}
	((teds_sortedstrictset_it*)iter)->node = new_node;
	if (new_node != NULL) {
		TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(new_node)++;
	}
	if (orig_node != NULL) {
		teds_sortedstrictset_node_release(orig_node);
	}
}

static zend_always_inline bool teds_sortedstrictset_node_valid(teds_sortedstrictset_node *node) {
	/* TODO: Mark key as invalid when removing? */
	return node != NULL && !Z_ISUNDEF(node->key);
}

static int teds_sortedstrictset_it_valid(zend_object_iterator *iter)
{
	teds_sortedstrictset_it     *iterator = (teds_sortedstrictset_it*)iter;
	return teds_sortedstrictset_node_valid(iterator->node) ? SUCCESS : FAILURE;
}

static zval *teds_sortedstrictset_it_get_current_data(zend_object_iterator *iter)
{
	teds_sortedstrictset_node *node = ((teds_sortedstrictset_it*)iter)->node;
	if (teds_sortedstrictset_node_valid(node)) {
		return &node->key;
	} else {
		return &EG(uninitialized_zval);
	}
}

static void teds_sortedstrictset_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_sortedstrictset_node *node = ((teds_sortedstrictset_it*)iter)->node;
	if (teds_sortedstrictset_node_valid(node)) {
		ZVAL_COPY(key, &node->key);
	} else {
		ZVAL_NULL(key);
	}
}

static void teds_sortedstrictset_it_move_forward(zend_object_iterator *iter)
{
	teds_sortedstrictset_node *const node = ((teds_sortedstrictset_it*)iter)->node;
	if (!teds_sortedstrictset_node_valid(node)) {
		return;
	}
	teds_sortedstrictset_node *const next = node->next;
	((teds_sortedstrictset_it*)iter)->node = next;
	if (next) {
		TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(next)++;
	}
	teds_sortedstrictset_node_release(node);
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_sortedstrictset_it_funcs = {
	teds_sortedstrictset_it_dtor,
	teds_sortedstrictset_it_valid,
	teds_sortedstrictset_it_get_current_data,
	teds_sortedstrictset_it_get_current_key,
	teds_sortedstrictset_it_move_forward,
	teds_sortedstrictset_it_rewind,
	NULL,
	NULL, /* get_gc */
};


zend_object_iterator *teds_sortedstrictset_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_sortedstrictset_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_sortedstrictset_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &teds_sortedstrictset_it_funcs;
	teds_sortedstrictset_node *node = teds_sortedstrictset_tree_get_first(&Z_SORTEDSTRICTSET_P(object)->array);
	if (node) {
		TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(node)++;
	}
	iterator->node = node;
	(void) ce;

	return &iterator->intern;
}

PHP_METHOD(Teds_SortedStrictSet, __unserialize)
{
	HashTable *raw_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	teds_sortedstrictset *const intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	teds_sortedstrictset_tree *const array = &intern->array;
	if (UNEXPECTED(!teds_sortedstrictset_tree_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	teds_sortedstrictset_tree_set_empty_tree(array);
	if (raw_data->nNumOfElements == 0) {
		return;
	}

	zend_string *str;
	zval *val;

	teds_sortedstrictset_node **nodes = emalloc(raw_data->nNumOfElements * sizeof(teds_sortedstrictset_node*));
	teds_sortedstrictset_node *prev = NULL;
	uint32_t sorted_nodes_count = 0;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_sortedstrictset_tree_clear(array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\SortedStrictSet::__unserialize saw unexpected string key, expected sequence of values", 0);
			if (nodes != NULL) {
				while (sorted_nodes_count > 0) {
					teds_sortedstrictset_node *n = nodes[--sorted_nodes_count];
					zval_ptr_dtor(&n->key);
					ZVAL_UNDEF(&n->key);
					teds_sortedstrictset_node_release(n);
				}
				efree(nodes);
			}
			RETURN_THROWS();
		}

		if (nodes != NULL) {
			if (sorted_nodes_count == 0 || teds_stable_compare(val, &prev->key) > 0) {
				prev = teds_sortedstrictset_node_alloc(val, NULL);
				nodes[sorted_nodes_count] = prev;
				sorted_nodes_count++;
				continue;
			}
			array->root = teds_sortedstrictset_node_build_tree_from_sorted_nodes(nodes, sorted_nodes_count);
			array->nNumOfElements = sorted_nodes_count;
			efree(nodes);
			nodes = NULL;
		}
		teds_sortedstrictset_tree_insert(array, val, false);
	} ZEND_HASH_FOREACH_END();

	if (nodes != NULL) {
		array->root = teds_sortedstrictset_node_build_tree_from_sorted_nodes(nodes, sorted_nodes_count);
		array->nNumOfElements = sorted_nodes_count;
		efree(nodes);
	}
}

PHP_METHOD(Teds_SortedStrictSet, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_sortedstrictset_new(teds_ce_SortedStrictSet);
	teds_sortedstrictset *intern = teds_sortedstrictset_from_obj(object);
	teds_sortedstrictset_tree_init_from_array(&intern->array, array_ht);

	RETURN_OBJ(object);
}

#define IMPLEMENT_READ_POSITION_PHP_METHOD(methodName, pos) \
PHP_METHOD(Teds_SortedStrictSet, methodName) \
{ \
       ZEND_PARSE_PARAMETERS_NONE(); \
       const teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS); \
       if (teds_sortedstrictset_tree_empty_size(&intern->array)) { \
               zend_throw_exception(spl_ce_UnderflowException, "Cannot read " # methodName " of empty SortedStrictSet", 0); \
               RETURN_THROWS(); \
       } \
       teds_sortedstrictset_node *node = teds_sortedstrictset_tree_get_ ## pos(&intern->array); \
       RETVAL_COPY(&node->key); \
}

IMPLEMENT_READ_POSITION_PHP_METHOD(bottom, first)
IMPLEMENT_READ_POSITION_PHP_METHOD(top, last)

#define IMPLEMENT_REMOVE_POSITION_PHP_METHOD(methodName, pos) \
PHP_METHOD(Teds_SortedStrictSet, methodName) \
{ \
	ZEND_PARSE_PARAMETERS_NONE(); \
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS); \
	if (teds_sortedstrictset_tree_empty_size(&intern->array)) { \
		zend_throw_exception(spl_ce_UnderflowException, "Cannot " # methodName " from empty SortedStrictSet", 0); \
		RETURN_THROWS(); \
	} \
	teds_sortedstrictset_node *node = teds_sortedstrictset_tree_get_ ## pos(&intern->array); \
	RETVAL_COPY_VALUE(&node->key); \
	teds_sortedstrictset_tree_remove_node(&intern->array, node, false); \
}

IMPLEMENT_REMOVE_POSITION_PHP_METHOD(pop, last)
IMPLEMENT_REMOVE_POSITION_PHP_METHOD(shift, first)

PHP_METHOD(Teds_SortedStrictSet, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
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
		TEDS_SORTEDSTRICTSET_FOREACH_KEY(&intern->array, value) {
			Z_TRY_ADDREF_P(value);
			ZEND_HASH_FILL_ADD(value);
		} TEDS_SORTEDSTRICTSET_FOREACH_END();
	} ZEND_HASH_FILL_END();
	RETURN_ARR(values);
}

static void teds_sortedstrictset_tree_replace_node_with_child(teds_sortedstrictset_tree *tree, teds_sortedstrictset_node *removed_node, teds_sortedstrictset_node *child_node) {
	teds_sortedstrictset_node *const parent = removed_node->parent;
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

static zend_always_inline teds_sortedstrictset_node *teds_sortedstrictset_node_remove_leftmost(teds_sortedstrictset_node *node) {
	while (true) {
		ZEND_ASSERT(node != NULL);
		ZEND_ASSERT(node->parent != NULL);
		ZEND_ASSERT(node->parent != node);
		if (node->left) {
			ZEND_ASSERT(node->left->parent == node);
			node = node->left;
			continue;
		}
		teds_sortedstrictset_node *const parent = node->parent;
		teds_sortedstrictset_node *const right = node->right;
		if (right) {
			ZEND_ASSERT(right->parent == node);
			RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTSET_NODE_COLOR(right) == TEDS_NODE_RED);
			right->parent = parent;
			node->right = NULL;
		}
		if (parent->left == node) {
			parent->left = right;
		} else {
			ZEND_ASSERT(parent->right == node);
			parent->right = right;
		}
		return node;
	}
}

static void teds_sortedstrictset_tree_rebalance_after_removal(teds_sortedstrictset_tree *tree, teds_sortedstrictset_node *p) {
	/* Based on https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Removal:_simple_cases */
	/* p is the parent of the removed node n. Before being removed, n was a black node with 2 children. */
	ZEND_ASSERT(p != NULL);
	/* Side of the parent on which the value with no children was removed */
	teds_sortedstrictset_node *n = NULL;
	teds_sortedstrictset_node *root = tree->root;
	ZEND_ASSERT(root != NULL);
	DEBUG_PRINTF("Delete from tree\n");
	do {
		const int dir = (p->right == n) ? 1 : 0;
		DEBUG_PRINTF("Delete from tree step n=%p dir=%d\n", n, dir);
		if (dir) {
			if (p->left == n) {
				if (n == NULL) {
					DEBUG_PRINTF("Delete saw unexpected null siblings\n");
					// RBTREE_INVARIANT_ASSERT(n != NULL);
					return;
				} else {
					ZEND_ASSERT(false && "bad tree");
					return;
				}
			}
		} else {
			ZEND_ASSERT(p->left == n);
			RBTREE_INVARIANT_ASSERT(p->right != n);
			if (UNEXPECTED(p->right == n)) {
				return;
			}
		}
		ZEND_ASSERT(p->children[dir] == n);
		/* sibling - in a valid tree this would be non-null because n was black before removal */
		teds_sortedstrictset_node *s = p->children[1 - dir];
		RBTREE_INVARIANT_ASSERT(s != NULL);
		if (UNEXPECTED(s == NULL)) {
			return;
		}
		ZEND_ASSERT(s->parent == p);
		/* close sibling on same side of sibling as node n is of parent p */
		teds_sortedstrictset_node *c = s->children[    dir];
		ZEND_ASSERT(!c || c->parent == s);
		/* distant sibling on opposite side of sibling as node n is of parent p */
		teds_sortedstrictset_node *d = s->children[1 - dir];
		ZEND_ASSERT(!d || d->parent == s);

		const bool s_black = TEDS_SORTEDSTRICTSET_NODE_COLOR(s) != TEDS_NODE_RED;
		const bool c_black = TEDS_SORTEDSTRICTSET_NODE_COLOR_NULLABLE(c) != TEDS_NODE_RED;
		const bool d_black = TEDS_SORTEDSTRICTSET_NODE_COLOR_NULLABLE(d) != TEDS_NODE_RED;
		const bool p_black = TEDS_SORTEDSTRICTSET_NODE_COLOR(p) != TEDS_NODE_RED;
		if (s_black) {
			if (!d_black) {
				/* Delete case 6 - s is black, d is red */
				DEBUG_PRINTF("Delete case 6\n");
delete_case_6:
				DEBUG_PRINTF("Delete case 6 inner\n");
				teds_sortedstrictset_tree_rotate_dir_root(tree, p, 1 - dir);
				TEDS_SORTEDSTRICTSET_NODE_COLOR(s) = TEDS_SORTEDSTRICTSET_NODE_COLOR(p);
				TEDS_SORTEDSTRICTSET_NODE_COLOR(p) = TEDS_NODE_BLACK;
				TEDS_SORTEDSTRICTSET_NODE_COLOR(d) = TEDS_NODE_BLACK;
				return;
			}
			if (c_black) {
				if (p_black) {
					/* Delete case 1 - change sibling to red node and recurse on black node parent*/
					n = p;
					p = p->parent;
					TEDS_SORTEDSTRICTSET_NODE_COLOR(s) = TEDS_NODE_RED;
					DEBUG_PRINTF("Delete case 1\n");
					continue;
				} else {
					/* Delete case 4 - change parent to black node and sibling to red node, making path counts balanced. */
					DEBUG_PRINTF("Delete case 4\n");
					TEDS_SORTEDSTRICTSET_NODE_COLOR(p) = TEDS_NODE_BLACK;
					TEDS_SORTEDSTRICTSET_NODE_COLOR(s) = TEDS_NODE_RED;
					return;
				}
			} else {
				DEBUG_PRINTF("Delete case 5\n");
				/* Delete case 5: c is red, s is black, d is black */
				ZEND_ASSERT(tree->root != s);
				teds_sortedstrictset_node_rotate_dir(s, dir);
				TEDS_SORTEDSTRICTSET_NODE_COLOR(s) = TEDS_NODE_RED;
				TEDS_SORTEDSTRICTSET_NODE_COLOR(c) = TEDS_NODE_BLACK;
				d = s;
				s = c;
				/* c is not used */
				/* Case 6 */
				goto delete_case_6;
			}
		} else {
			/* s is red - delete case 3 */
			DEBUG_PRINTF("Delete case 3\n");
			if (UNEXPECTED(!(p_black && c_black && d_black))) {
				RBTREE_INVARIANT_ASSERT(false);
				return;
			}
			ZEND_ASSERT(c != NULL);
			teds_sortedstrictset_tree_rotate_dir_root(tree, p, 1 - dir);
			TEDS_SORTEDSTRICTSET_NODE_COLOR(p) = TEDS_NODE_RED;
			TEDS_SORTEDSTRICTSET_NODE_COLOR(s) = TEDS_NODE_BLACK;
			continue;
		}

		/* Unimplemented case */
#define DEBUG_COLOR(x_black) ((x_black) ? "black" : "red")
		DEBUG_PRINTF("TODO implement removal case p=%s c=%s s=%s d=%s\n", DEBUG_COLOR(p_black), DEBUG_COLOR(c_black), DEBUG_COLOR(s_black), DEBUG_COLOR(d_black));
		return;
	} while (p != NULL);

	DEBUG_PRINTF("Delete case 2 - n is root\n");
	ZEND_ASSERT(p == NULL);
	ZEND_ASSERT(n != NULL);
	ZEND_ASSERT(tree->root == n);
}

static zend_always_inline void teds_sortedstrictset_tree_replace_node_with_descendant_and_rebalance(teds_sortedstrictset_tree *tree, teds_sortedstrictset_node *node) {
	teds_sortedstrictset_node *const replacement = teds_sortedstrictset_node_remove_leftmost(node->right);
	teds_sortedstrictset_node *const parent = node->parent;
	ZEND_ASSERT(replacement->parent != NULL);
	/* parent_of_removed_node is non-null if replacement is a black node with no children. */
	teds_sortedstrictset_node *parent_of_removed_node;
	if (replacement->right == NULL && TEDS_SORTEDSTRICTSET_NODE_COLOR(replacement) == TEDS_NODE_BLACK) {
		/* Note that the removed node's parent is the node then the new parent is the replacement itself. */
		parent_of_removed_node = replacement->parent != node ? replacement->parent : replacement;
		ZEND_ASSERT(parent_of_removed_node != NULL);
	} else {
		parent_of_removed_node = NULL;
	}
	ZEND_ASSERT(replacement->left == NULL);
	ZEND_ASSERT(replacement != node);

	if (node->left) {
		ZEND_ASSERT(node->left != replacement);
		node->left->parent = replacement;
	}
	replacement->left = node->left;

	if (node->right) {
		ZEND_ASSERT(node->right != replacement);
		node->right->parent = replacement;
	}
	replacement->right = node->right;

	TEDS_SORTEDSTRICTSET_NODE_COLOR(replacement) = TEDS_SORTEDSTRICTSET_NODE_COLOR(node);
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
	/* teds_sortedstrictset_node_debug_check_linked_to_parent(tree->root); */

	if (parent_of_removed_node) {
		/* Paths going through the replaced replacement node (black with both nil children)
		 * now have one less black node than all other paths and this needs to be rebalanced. */
		teds_sortedstrictset_tree_rebalance_after_removal(tree, parent_of_removed_node);
		/* replacement was a black node with no children */
	}
}

static zend_always_inline void teds_sortedstrictset_tree_remove_node(teds_sortedstrictset_tree *tree, teds_sortedstrictset_node *const node, bool free_zvals) {
	teds_sortedstrictset_node *const prev = node->prev;
	teds_sortedstrictset_node *const next = node->next;
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}
	node->prev = NULL;
	node->next = NULL;

	if (!node->left) {
		/* If a valid red-black tree has only 1 child,
		 * then that child is guaranteed to be red, so that the number of black nodes on paths on both sides are equal. */
		teds_sortedstrictset_node *const right = node->right;
		ZEND_ASSERT(right == NULL || right->parent == node);
		teds_sortedstrictset_tree_replace_node_with_child(tree, node, right);

		if (right == NULL) {
			if (TEDS_SORTEDSTRICTSET_NODE_COLOR(node) == TEDS_NODE_BLACK) {
				if (node->parent != NULL) {
					/* This was a black node with no children. The tree needs to be rebalanced if it was not empty. */
					teds_sortedstrictset_tree_rebalance_after_removal(tree, node->parent);
				} else {
					ZEND_ASSERT(tree->root == NULL);
				}
			}
		} else {
			/* Change right node's color to black to maintain the invariant */
			RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTSET_NODE_COLOR(node) == TEDS_NODE_BLACK);
			RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTSET_NODE_COLOR(right) == TEDS_NODE_RED);
			TEDS_SORTEDSTRICTSET_NODE_COLOR(right) = TEDS_NODE_BLACK;
		}
	} else if (!node->right) {
		ZEND_ASSERT(node->left->parent == node);
		teds_sortedstrictset_tree_replace_node_with_child(tree, node, node->left);
		/* Change left node's color to black to maintain the invariant */
		RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTSET_NODE_COLOR(node) == TEDS_NODE_BLACK);
		RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTSET_NODE_COLOR(node->left) == TEDS_NODE_RED);
		TEDS_SORTEDSTRICTSET_NODE_COLOR(node->left) = TEDS_NODE_BLACK;
	} else {
		ZEND_ASSERT(node->left->parent == node);
		ZEND_ASSERT(node->right->parent == node);

		teds_sortedstrictset_tree_replace_node_with_descendant_and_rebalance(tree, node);
	}

	ZEND_ASSERT(tree->nNumOfElements > 0);
	tree->nNumOfElements--;
	if (free_zvals) {
		zval_ptr_dtor(&node->key);
	}
	ZVAL_UNDEF(&node->key);
	teds_sortedstrictset_node_release(node);
}

static bool teds_sortedstrictset_tree_remove_key(teds_sortedstrictset_tree *tree, zval *key)
{
	/* FIXME implement binary tree removal */
	teds_sortedstrictset_node *const node = teds_sortedstrictset_tree_find_key(tree, key);
	if (node == NULL) {
		/* Nothing to remove */
		return false;
	}
	teds_sortedstrictset_tree_remove_node(tree, node, true);
	return true;
}

PHP_METHOD(Teds_SortedStrictSet, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	RETURN_BOOL(teds_sortedstrictset_tree_insert(&intern->array, value, false));
}

PHP_METHOD(Teds_SortedStrictSet, remove)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	RETURN_BOOL(teds_sortedstrictset_tree_remove_key(&intern->array, key));
}

PHP_METHOD(Teds_SortedStrictSet, contains)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	if (intern->array.nNumOfElements > 0) {
		teds_sortedstrictset_node *entry = teds_sortedstrictset_tree_find_key(&intern->array, key);
		RETURN_BOOL(entry != NULL);
	}
	RETURN_FALSE;
}

static ZEND_COLD void teds_sortedstrictset_node_debug_representation(zval *return_value, teds_sortedstrictset_node *node, zend_string *strings[4]) {
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

	zval tmp;
	ZVAL_BOOL(&tmp, TEDS_SORTEDSTRICTSET_NODE_COLOR(node) == TEDS_NODE_RED);
	zend_hash_add_new(ht, strings[1], &tmp);

	teds_sortedstrictset_node_debug_representation(&tmp, node->left, strings);
	zend_hash_add_new(ht, strings[2], &tmp);

	teds_sortedstrictset_node_debug_representation(&tmp, node->right, strings);
	zend_hash_add_new(ht, strings[3], &tmp);
	RETURN_ARR(ht);
}

ZEND_COLD PHP_METHOD(Teds_SortedStrictSet, debugGetTreeRepresentation)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	if (intern->array.nNumOfElements == 0) {
		RETURN_EMPTY_ARRAY();
	}
	zend_string *strings[4] = {
		zend_string_init_interned("key", sizeof("key") - 1, 0),
		zend_string_init_interned("red", sizeof("red") - 1, 0),
		zend_string_init_interned("left", sizeof("left") - 1, 0),
		zend_string_init_interned("right", sizeof("right") - 1, 0),
	};
	teds_sortedstrictset_node_debug_representation(return_value, intern->array.root, strings);
}

/* Returns -1 for unbalanced tree */
static ZEND_COLD int teds_sortedstrictset_node_is_balanced(const teds_sortedstrictset_node *node) {
	if (!node) {
		return 0;
	}

	ZEND_ASSERT(!node->left || node->left->parent == node);
	int left_result = teds_sortedstrictset_node_is_balanced(node->left);
	if (left_result < 0) {
		return left_result;
	}
	ZEND_ASSERT(!node->right || node->right->parent == node);
	int right_result = teds_sortedstrictset_node_is_balanced(node->right);
	if (right_result < 0) {
		return right_result;
	}
	if (left_result != right_result) {
		return -1;
	}
	return left_result + ((TEDS_SORTEDSTRICTSET_NODE_COLOR(node) == TEDS_NODE_BLACK) ? 1 : 0);
}

ZEND_COLD PHP_METHOD(Teds_SortedStrictSet, debugIsBalanced)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	int result = teds_sortedstrictset_node_is_balanced(intern->array.root);
	RETURN_BOOL(result >= 0);
}

static void teds_sortedstrictset_tree_clear(teds_sortedstrictset_tree *array) {
	if (teds_sortedstrictset_tree_empty_size(array)) {
		return;
	}
	teds_sortedstrictset_tree array_copy = *array;

	teds_sortedstrictset_tree_set_empty_tree(array);

	teds_sortedstrictset_tree_dtor(&array_copy);
	/* Could call teds_sortedstrictset_get_properties but properties array is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_SortedStrictSet, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictset *intern = Z_SORTEDSTRICTSET_P(ZEND_THIS);
	teds_sortedstrictset_tree_clear(&intern->array);
	TEDS_RETURN_VOID();
}

PHP_MINIT_FUNCTION(teds_sortedstrictset)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_SortedStrictSet = register_class_Teds_SortedStrictSet(zend_ce_aggregate, teds_ce_Values, php_json_serializable_ce);
	teds_ce_SortedStrictSet->create_object = teds_sortedstrictset_new;

	memcpy(&teds_handler_SortedStrictSet, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_SortedStrictSet.offset          = XtOffsetOf(teds_sortedstrictset, std);
	teds_handler_SortedStrictSet.clone_obj       = teds_sortedstrictset_clone;
	teds_handler_SortedStrictSet.count_elements  = teds_sortedstrictset_count_elements;
	teds_handler_SortedStrictSet.get_properties  = teds_sortedstrictset_get_properties;
	teds_handler_SortedStrictSet.get_gc          = teds_sortedstrictset_get_gc;
	teds_handler_SortedStrictSet.dtor_obj        = zend_objects_destroy_object;
	teds_handler_SortedStrictSet.free_obj        = teds_sortedstrictset_free_storage;

	teds_ce_SortedStrictSet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_SortedStrictSet->get_iterator = teds_sortedstrictset_get_iterator;

	return SUCCESS;
}
