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

/*
static void teds_sortedstrictmap_node_debug_check_linked_to_parent(const teds_sortedstrictmap_node *node) {
#if ZEND_DEBUG
	if (!node) {
		return;
	}

	DEBUG_PRINTF("teds_sortedstrictmap_node_debug_check_linked_to_parent %p\n", node);
	if (node->left) {
		ZEND_ASSERT(node->left->parent == node);
		teds_sortedstrictmap_node_debug_check_linked_to_parent(node->left);
	}
	if (node->right) {
		ZEND_ASSERT(node->right->parent == node);
		teds_sortedstrictmap_node_debug_check_linked_to_parent(node->right);
	}
#endif
}
*/

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

/* Given a node, rotate it so that the child node becomes the parent of that node.
 * teds_sortedstrictmap_tree_rotate_dir_root should be used if node might be the root node. */
static zend_always_inline void teds_sortedstrictmap_node_rotate_dir(teds_sortedstrictmap_node *const node, int dir) {
	teds_sortedstrictmap_node *const c = node->children[dir];
	teds_sortedstrictmap_node *const parent = node->parent;
	teds_sortedstrictmap_node *const transfer = c->children[1 - dir];

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

static zend_always_inline void teds_sortedstrictmap_tree_rotate_dir_root(teds_sortedstrictmap_tree *const tree, teds_sortedstrictmap_node *const node, int dir) {
	teds_sortedstrictmap_node *const c = node->children[dir];
	ZEND_ASSERT(c != NULL);
	teds_sortedstrictmap_node *const parent = node->parent;
	teds_sortedstrictmap_node *const transfer = c->children[1 - dir];
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

		teds_sortedstrictmap_tree_rotate_dir_root(tree, grandparent, dir);
		TEDS_SORTEDSTRICTMAP_NODE_COLOR(parent) = TEDS_NODE_BLACK;
		TEDS_SORTEDSTRICTMAP_NODE_COLOR(grandparent) = TEDS_NODE_RED;
		return;
	}
}

static zend_always_inline void teds_sortedstrictmap_node_release(teds_sortedstrictmap_node *node) {
	ZEND_ASSERT(node != NULL);
	ZEND_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(node) > 0);
	if (EXPECTED(TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(node) == 1)) {
		ZEND_ASSERT(Z_ISUNDEF(node->key));
		efree_size(node, sizeof(teds_sortedstrictmap_node));
	} else {
		--TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(node);
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
				if (UNEXPECTED(tree->nNumOfElements >= TEDS_MAX_ZVAL_PAIR_COUNT)) {
					/* Mainly, the reason to do that is that get_properties returns the array of properties for get_gc, which expects a uint32_t in php-src/Zend/zend_gc.c
					 * A less severe reason is that this is converted to an array in var_dump/var_export for debugging, but the latter can be avoided */
					zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\SortedStrictMap capacity");
					ZEND_UNREACHABLE();
					return false;
				}

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

static zend_always_inline bool teds_sortedstrictmap_tree_uninitialized(teds_sortedstrictmap_tree *array)
{
	if (array->initialized) {
		return false;
	}
	ZEND_ASSERT(array->root == NULL);
	ZEND_ASSERT(array->nNumOfElements == 0);
	return true;
}

static zend_always_inline void teds_sortedstrictmap_tree_set_empty_tree(teds_sortedstrictmap_tree *array)
{
	array->root = NULL;
	array->nNumOfElements = 0;
	array->initialized = true;
}

static teds_sortedstrictmap_node *teds_sortedstrictmap_node_build_tree_from_sorted_nodes_helper(teds_sortedstrictmap_node **nodes, const uint32_t n, teds_sortedstrictmap_node *left_parent, teds_sortedstrictmap_node *right_parent, int leaf_depth)
{
	ZEND_ASSERT(n > 0);
	const uint32_t mid = n/2;
	teds_sortedstrictmap_node *const root = nodes[mid];
	ZEND_ASSERT(leaf_depth >= 0);
	TEDS_SORTEDSTRICTMAP_NODE_COLOR(root) = (leaf_depth == 0 ? TEDS_NODE_RED : TEDS_NODE_BLACK);
	leaf_depth--;
	{
		if (mid > 0) {
			teds_sortedstrictmap_node *const left = teds_sortedstrictmap_node_build_tree_from_sorted_nodes_helper(nodes, mid, left_parent, root, leaf_depth);
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
			teds_sortedstrictmap_node *const right = teds_sortedstrictmap_node_build_tree_from_sorted_nodes_helper(nodes + mid + 1, right_count, root, right_parent, leaf_depth);
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

static teds_sortedstrictmap_node *teds_sortedstrictmap_node_build_tree_from_sorted_nodes(teds_sortedstrictmap_node **nodes, const uint32_t n)
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
	return teds_sortedstrictmap_node_build_tree_from_sorted_nodes_helper(nodes, n, NULL, NULL, leaf_depth);
}

void teds_sortedstrictmap_tree_init_from_array(teds_sortedstrictmap_tree *array, zend_array *values)
{
	teds_sortedstrictmap_tree_set_empty_tree(array);
	if (values->nNumOfElements == 0) {
		return;
	}
	teds_sortedstrictmap_node **nodes = emalloc(values->nNumOfElements * sizeof(teds_sortedstrictmap_node*));
	teds_sortedstrictmap_node *prev = NULL;
	uint32_t i = 0;

	zend_long nkey;
	zend_string *skey;
	zval *val;

	/* Note: The comparison is redundant for a packed array and can be sped up more. */
	ZEND_HASH_FOREACH_KEY_VAL(values, nkey, skey, val)  {
		zval key;
		if (skey) {
			ZVAL_STR(&key, skey);
		} else {
			ZVAL_LONG(&key, nkey);
		}
		ZVAL_DEREF(val);
		if (nodes != NULL) {
			if (i == 0 || teds_stable_compare(&key, &prev->key) > 0) {
				prev = teds_sortedstrictmap_node_alloc(&key, val, NULL);
				nodes[i] = prev;
				i++;
				continue;
			}
			array->root = teds_sortedstrictmap_node_build_tree_from_sorted_nodes(nodes, i);
			array->nNumOfElements = i;
			efree(nodes);
			nodes = NULL;
		}

		bool created = teds_sortedstrictmap_tree_insert(array, &key, val, true);
		ZEND_ASSERT(created);
	} ZEND_HASH_FOREACH_END();

	if (nodes != NULL) {
		array->root = teds_sortedstrictmap_node_build_tree_from_sorted_nodes(nodes, i);
		array->nNumOfElements = i;
		efree(nodes);
	}
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
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (intern->array.nNumOfElements > 0) {
		zval *key, *val;

		TEDS_SORTEDSTRICTMAP_FOREACH_KEY_VAL(&intern->array, key, val) {
			zend_get_gc_buffer_add_zval(gc_buffer, key);
			zend_get_gc_buffer_add_zval(gc_buffer, val);
		} TEDS_SORTEDSTRICTMAP_FOREACH_END();

	}
	/* Overwrites table and table_count. The caller of get_gc does not initialize these. */
	zend_get_gc_buffer_use(gc_buffer, table, table_count);

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

	for (uint32_t i = len; i < old_length; i++) {
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

	teds_assert_object_has_empty_member_list(new_object);

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

	const uint32_t raw_size = zend_hash_num_elements(raw_data);
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
	teds_sortedstrictmap_node **nodes = emalloc(raw_size / 2 * sizeof(teds_sortedstrictmap_node*));
	teds_sortedstrictmap_node *prev = NULL;
	uint32_t sorted_nodes_count = 0;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_sortedstrictmap_tree_clear(array);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\SortedStrictMap::__unserialize saw unexpected string key, expected sequence of keys and values", 0);
			if (nodes != NULL) {
				while (sorted_nodes_count > 0) {
					teds_sortedstrictmap_node *n = nodes[--sorted_nodes_count];
					zval_ptr_dtor(&n->key);
					zval_ptr_dtor(&n->value);
					ZVAL_UNDEF(&n->key);
					teds_sortedstrictmap_node_release(n);
				}
				efree(nodes);
			}
			RETURN_THROWS();
		}

		ZVAL_DEREF(val);
		if (!is_key) {
			is_key = true;
			if (nodes != NULL) {
				if (sorted_nodes_count == 0 || teds_stable_compare(&key, &prev->key) > 0) {
					prev = teds_sortedstrictmap_node_alloc(&key, val, NULL);
					nodes[sorted_nodes_count] = prev;
					sorted_nodes_count++;
					continue;
				}
				array->root = teds_sortedstrictmap_node_build_tree_from_sorted_nodes(nodes, sorted_nodes_count);
				array->nNumOfElements = sorted_nodes_count;
				efree(nodes);
				nodes = NULL;
			}
			teds_sortedstrictmap_tree_insert(array, &key, val, false);
		} else {
			ZVAL_COPY_VALUE(&key, val);
			is_key = false;
		}
	} ZEND_HASH_FOREACH_END();

	if (nodes != NULL) {
		array->root = teds_sortedstrictmap_node_build_tree_from_sorted_nodes(nodes, sorted_nodes_count);
		array->nNumOfElements = sorted_nodes_count;
		efree(nodes);
	}
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
	const uint32_t num_entries = zend_hash_num_elements(raw_data);
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
	const uint32_t len = intern->array.nNumOfElements;
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
			RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_COLOR(right) == TEDS_NODE_RED);
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

static void teds_sortedstrictmap_tree_rebalance_after_removal(teds_sortedstrictmap_tree *tree, teds_sortedstrictmap_node *p) {
	/* Based on https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Removal:_simple_cases */
	/* p is the parent of the removed node n. Before being removed, n was a black node with 2 children. */
	ZEND_ASSERT(p != NULL);
	/* Side of the parent on which the value with no children was removed */
	teds_sortedstrictmap_node *n = NULL;
	teds_sortedstrictmap_node *root = tree->root;
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
		teds_sortedstrictmap_node *s = p->children[1 - dir];
		RBTREE_INVARIANT_ASSERT(s != NULL);
		if (UNEXPECTED(s == NULL)) {
			return;
		}
		ZEND_ASSERT(s->parent == p);
		/* close sibling on same side of sibling as node n is of parent p */
		teds_sortedstrictmap_node *c = s->children[    dir];
		ZEND_ASSERT(!c || c->parent == s);
		/* distant sibling on opposite side of sibling as node n is of parent p */
		teds_sortedstrictmap_node *d = s->children[1 - dir];
		ZEND_ASSERT(!d || d->parent == s);

		const bool s_black = TEDS_SORTEDSTRICTMAP_NODE_COLOR(s) != TEDS_NODE_RED;
		const bool c_black = TEDS_SORTEDSTRICTMAP_NODE_COLOR_NULLABLE(c) != TEDS_NODE_RED;
		const bool d_black = TEDS_SORTEDSTRICTMAP_NODE_COLOR_NULLABLE(d) != TEDS_NODE_RED;
		const bool p_black = TEDS_SORTEDSTRICTMAP_NODE_COLOR(p) != TEDS_NODE_RED;
		if (s_black) {
			if (!d_black) {
				/* Delete case 6 - s is black, d is red */
				DEBUG_PRINTF("Delete case 6\n");
delete_case_6:
				DEBUG_PRINTF("Delete case 6 inner\n");
				teds_sortedstrictmap_tree_rotate_dir_root(tree, p, 1 - dir);
				TEDS_SORTEDSTRICTMAP_NODE_COLOR(s) = TEDS_SORTEDSTRICTMAP_NODE_COLOR(p);
				TEDS_SORTEDSTRICTMAP_NODE_COLOR(p) = TEDS_NODE_BLACK;
				TEDS_SORTEDSTRICTMAP_NODE_COLOR(d) = TEDS_NODE_BLACK;
				return;
			}
			if (c_black) {
				if (p_black) {
					/* Delete case 1 - change sibling to red node and recurse on black node parent*/
					n = p;
					p = p->parent;
					TEDS_SORTEDSTRICTMAP_NODE_COLOR(s) = TEDS_NODE_RED;
					DEBUG_PRINTF("Delete case 1\n");
					continue;
				} else {
					/* Delete case 4 - change parent to black node and sibling to red node, making path counts balanced. */
					DEBUG_PRINTF("Delete case 4\n");
					TEDS_SORTEDSTRICTMAP_NODE_COLOR(p) = TEDS_NODE_BLACK;
					TEDS_SORTEDSTRICTMAP_NODE_COLOR(s) = TEDS_NODE_RED;
					return;
				}
			} else {
				DEBUG_PRINTF("Delete case 5\n");
				/* Delete case 5: c is red, s is black, d is black */
				ZEND_ASSERT(tree->root != s);
				teds_sortedstrictmap_node_rotate_dir(s, dir);
				TEDS_SORTEDSTRICTMAP_NODE_COLOR(s) = TEDS_NODE_RED;
				TEDS_SORTEDSTRICTMAP_NODE_COLOR(c) = TEDS_NODE_BLACK;
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
			teds_sortedstrictmap_tree_rotate_dir_root(tree, p, 1 - dir);
			TEDS_SORTEDSTRICTMAP_NODE_COLOR(p) = TEDS_NODE_RED;
			TEDS_SORTEDSTRICTMAP_NODE_COLOR(s) = TEDS_NODE_BLACK;
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

static zend_always_inline void teds_sortedstrictmap_tree_replace_node_with_descendant_and_rebalance(teds_sortedstrictmap_tree *tree, teds_sortedstrictmap_node *node) {
	teds_sortedstrictmap_node *const replacement = teds_sortedstrictmap_node_remove_leftmost(node->right);
	teds_sortedstrictmap_node *const parent = node->parent;
	ZEND_ASSERT(replacement->parent != NULL);
	/* parent_of_removed_node is non-null if replacement is a black node with no children. */
	teds_sortedstrictmap_node *parent_of_removed_node;
	if (replacement->right == NULL && TEDS_SORTEDSTRICTMAP_NODE_COLOR(replacement) == TEDS_NODE_BLACK) {
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

	TEDS_SORTEDSTRICTMAP_NODE_COLOR(replacement) = TEDS_SORTEDSTRICTMAP_NODE_COLOR(node);
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
	/* teds_sortedstrictmap_node_debug_check_linked_to_parent(tree->root); */

	if (parent_of_removed_node) {
		/* Paths going through the replaced replacement node (black with both nil children)
		 * now have one less black node than all other paths and this needs to be rebalanced. */
		teds_sortedstrictmap_tree_rebalance_after_removal(tree, parent_of_removed_node);
		/* replacement was a black node with no children */
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
		/* If a valid red-black tree has only 1 child,
		 * then that child is guaranteed to be red, so that the number of black nodes on paths on both sides are equal. */
		teds_sortedstrictmap_node *const right = node->right;
		ZEND_ASSERT(right == NULL || right->parent == node);
		teds_sortedstrictmap_tree_replace_node_with_child(tree, node, right);

		if (right == NULL) {
			if (TEDS_SORTEDSTRICTMAP_NODE_COLOR(node) == TEDS_NODE_BLACK) {
				if (node->parent != NULL) {
					/* This was a black node with no children. The tree needs to be rebalanced if it was not empty. */
					teds_sortedstrictmap_tree_rebalance_after_removal(tree, node->parent);
				} else {
					ZEND_ASSERT(tree->root == NULL);
				}
			}
		} else {
			/* Change right node's color to black to maintain the invariant */
			RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_COLOR(node) == TEDS_NODE_BLACK);
			RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_COLOR(right) == TEDS_NODE_RED);
			TEDS_SORTEDSTRICTMAP_NODE_COLOR(right) = TEDS_NODE_BLACK;
		}
	} else if (!node->right) {
		ZEND_ASSERT(node->left->parent == node);
		teds_sortedstrictmap_tree_replace_node_with_child(tree, node, node->left);
		/* Change left node's color to black to maintain the invariant */
		RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_COLOR(node) == TEDS_NODE_BLACK);
		RBTREE_INVARIANT_ASSERT(TEDS_SORTEDSTRICTMAP_NODE_COLOR(node->left) == TEDS_NODE_RED);
		TEDS_SORTEDSTRICTMAP_NODE_COLOR(node->left) = TEDS_NODE_BLACK;
	} else {
		ZEND_ASSERT(node->left->parent == node);
		ZEND_ASSERT(node->right->parent == node);

		teds_sortedstrictmap_tree_replace_node_with_descendant_and_rebalance(tree, node);
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

PHP_METHOD(Teds_SortedStrictMap, contains)
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

PHP_METHOD(Teds_SortedStrictMap, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap_tree *const array = &Z_SORTEDSTRICTMAP_P(ZEND_THIS)->array;
	uint32_t len = array->nNumOfElements;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	zend_array *values = zend_new_array(len);

	zval *key, *val;
	TEDS_SORTEDSTRICTMAP_FOREACH_KEY_VAL(array, key, val) {
		Z_TRY_ADDREF_P(val);
		array_set_zval_key(values, key, val);
		zval_ptr_dtor_nogc(val);
		if (UNEXPECTED(EG(exception))) {
			zend_array_destroy(values);
			RETURN_THROWS();
		}
	} TEDS_SORTEDSTRICTMAP_FOREACH_END();
	RETURN_ARR(values);
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

/* Returns -1 for unbalanced tree */
static ZEND_COLD int teds_sortedstrictmap_node_is_balanced(const teds_sortedstrictmap_node *node) {
	if (!node) {
		return 0;
	}

	ZEND_ASSERT(!node->left || node->left->parent == node);
	int left_result = teds_sortedstrictmap_node_is_balanced(node->left);
	if (left_result < 0) {
		return left_result;
	}
	ZEND_ASSERT(!node->right || node->right->parent == node);
	int right_result = teds_sortedstrictmap_node_is_balanced(node->right);
	if (right_result < 0) {
		return right_result;
	}
	if (left_result != right_result) {
		return -1;
	}
	return left_result + ((TEDS_SORTEDSTRICTMAP_NODE_COLOR(node) == TEDS_NODE_BLACK) ? 1 : 0);
}

ZEND_COLD PHP_METHOD(Teds_SortedStrictMap, debugIsBalanced)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_sortedstrictmap *intern = Z_SORTEDSTRICTMAP_P(ZEND_THIS);
	int result = teds_sortedstrictmap_node_is_balanced(intern->array.root);
	RETURN_BOOL(result >= 0);
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
	teds_ce_SortedStrictMap = register_class_Teds_SortedStrictMap(zend_ce_aggregate, teds_ce_Collection, php_json_serializable_ce);
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
