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
#include "teds_stricttreeset_arginfo.h"
#include "teds_stricttreeset.h"
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

zend_object_handlers teds_handler_StrictTreeSet;
zend_class_entry *teds_ce_StrictTreeSet;

static zend_always_inline teds_stricttreeset_node *teds_stricttreeset_tree_find_key(const teds_stricttreeset_tree *tree, zval *key)
{
	teds_stricttreeset_node *it = tree->root;
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

static zend_always_inline void teds_stricttreeset_tree_remove_node(teds_stricttreeset_tree *tree, teds_stricttreeset_node *const node, bool free_zvals);

static zend_always_inline teds_stricttreeset_node *teds_stricttreeset_node_alloc(const zval *key, teds_stricttreeset_node *parent) {
	ZEND_ASSERT(!Z_ISUNDEF_P(key));
	teds_stricttreeset_node *n = emalloc(sizeof(teds_stricttreeset_node));
	n->parent = parent;
	ZVAL_COPY(&n->key, key);
	TEDS_STRICTTREESET_NODE_COLOR(n) = TEDS_NODE_RED;
	return n;
}

/* Given a node, rotate it so that the child node becomes the parent of that node.
 * teds_stricttreeset_tree_rotate_dir_root should be used if node might be the root node. */
static zend_always_inline void teds_stricttreeset_node_rotate_dir(teds_stricttreeset_node *const node, int dir) {
	teds_stricttreeset_node *const c = node->children[dir];
	teds_stricttreeset_node *const parent = node->parent;
	teds_stricttreeset_node *const transfer = c->children[1 - dir];

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

static zend_always_inline void teds_stricttreeset_tree_rotate_dir_root(teds_stricttreeset_tree *const tree, teds_stricttreeset_node *const node, int dir) {
	teds_stricttreeset_node *const c = node->children[dir];
	ZEND_ASSERT(c != NULL);
	teds_stricttreeset_node *const parent = node->parent;
	teds_stricttreeset_node *const transfer = c->children[1 - dir];
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

static zend_always_inline void teds_stricttreeset_tree_rebalance_after_insert(teds_stricttreeset_tree *tree, teds_stricttreeset_node *node) {
	while (true) {
		teds_stricttreeset_node *parent = node->parent;
		// ZEND_ASSERT(TEDS_STRICTTREESET_NODE_COLOR(node) == TEDS_NODE_RED);

		/* Based on https://en.wikipedia.org/wiki/Red%E2%80%93black_tree */
		if (parent == NULL || TEDS_STRICTTREESET_NODE_COLOR(parent) == TEDS_NODE_BLACK) {
			return;
		}

		teds_stricttreeset_node *const grandparent = parent->parent;
		if (grandparent == NULL) {
			TEDS_STRICTTREESET_NODE_COLOR(parent) = TEDS_NODE_BLACK;
			return;
		}
		/* In a valid tree, the grandparent node would be black if parent was red */

		const int dir = grandparent->right == parent ? 1 : 0;
		ZEND_ASSERT(grandparent->children[dir] == parent);
		teds_stricttreeset_node *uncle;
		if (dir) {
			uncle = grandparent->left;
		} else {
			uncle = grandparent->right;
		}

		if (uncle && TEDS_STRICTTREESET_NODE_COLOR(uncle) == TEDS_NODE_RED) {
			TEDS_STRICTTREESET_NODE_COLOR(uncle) = TEDS_NODE_BLACK;
			TEDS_STRICTTREESET_NODE_COLOR(parent) = TEDS_NODE_BLACK;
			TEDS_STRICTTREESET_NODE_COLOR(grandparent) = TEDS_NODE_RED;
			node = grandparent;
			continue;
		}
		/* Parent node is red but the uncle node is black. */
		/* In a valid tree, the grandparent node would be black */
		if (node == parent->children[1 - dir]) {
			teds_stricttreeset_node_rotate_dir(parent, 1 - dir);
			teds_stricttreeset_node *orig_node = node;
			node = parent;
			parent = orig_node;
		}
		ZEND_ASSERT(grandparent->children[dir] == parent);
		ZEND_ASSERT(parent->children[dir] == node);
		ZEND_ASSERT(parent->parent == grandparent);
		ZEND_ASSERT(node->parent == parent);

		teds_stricttreeset_tree_rotate_dir_root(tree, grandparent, dir);
		TEDS_STRICTTREESET_NODE_COLOR(parent) = TEDS_NODE_BLACK;
		TEDS_STRICTTREESET_NODE_COLOR(grandparent) = TEDS_NODE_RED;
		return;
	}
}

static zend_always_inline void teds_stricttreeset_node_release(teds_stricttreeset_node *node) {
	ZEND_ASSERT(node != NULL);
	ZEND_ASSERT(!Z_ISUNDEF(node->key));
	efree_size(node, sizeof(teds_stricttreeset_node));
}

/* Returns true if a new entry was added to the set, false if updated. Based on _zend_hash_add_or_update_i. */
static zend_always_inline bool teds_stricttreeset_tree_insert(teds_stricttreeset_tree *tree, zval *key, bool add_new)
{
	ZEND_ASSERT(Z_TYPE_P(key) != IS_UNDEF);

	/* TODO optimize */
	teds_stricttreeset_node *it = tree->root;
	if (it == NULL) {
		/* Initialize this tree as a new binary search tree of size 1 */
		it = teds_stricttreeset_node_alloc(key, NULL);
		tree->root = it;
		it->left = NULL;
		it->right = NULL;
		tree->nNumOfElements++;
		tree->should_rebuild_properties = true;
		return true;
	}
	/* c must be declared in outer scope for goto to work. */
	teds_stricttreeset_node *c;
	while (true) {
		const int comparison = teds_stable_compare(key, &it->key);
		if (comparison > 0) {
			if (it->right == NULL) {
				c = teds_stricttreeset_node_alloc(key, it);

				c->left = NULL;
				c->right = NULL;
				it->right = c;
finish_insert:
				tree->nNumOfElements++;
				tree->should_rebuild_properties = true;
				if (UNEXPECTED(tree->nNumOfElements >= TEDS_MAX_ZVAL_PAIR_COUNT)) {
					/* Mainly, the reason to do that is that get_properties returns the tree of properties for get_gc, which expects a uint32_t in php-src/Zend/zend_gc.c
					 * A less severe reason is that this is converted to an tree in var_dump/var_export for debugging, but the latter can be avoided */
					zend_error_noreturn(E_ERROR, "exceeded max valid Teds\\StrictTreeSet capacity");
					ZEND_UNREACHABLE();
					return false;
				}
				teds_stricttreeset_tree_rebalance_after_insert(tree, c);

				return true;
			}
			it = it->right;
		} else if ((add_new && !ZEND_DEBUG) || comparison < 0) {
			if (it->left == NULL) {
				c = teds_stricttreeset_node_alloc(key, it);
				c->left = NULL;
				c->right = NULL;
				it->left = c;
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

static void teds_stricttreeset_tree_clear(teds_stricttreeset_tree *tree);

/* Used by InternalIterator returned by StrictTreeSet->getIterator() */
typedef struct _teds_stricttreeset_it {
	zend_object_iterator       intern;
	teds_stricttreeset_node   *node;
	/* Connects to other active iterators */
	teds_intrusive_dllist_node dllist_node;
	bool is_before_first;
} teds_stricttreeset_it;

static zend_always_inline teds_stricttreeset *teds_stricttreeset_from_object(zend_object *obj)
{
	return (teds_stricttreeset*)((char*)(obj) - XtOffsetOf(teds_stricttreeset, std));
}

static zend_always_inline teds_stricttreeset_it *teds_stricttreeset_it_from_dllist_node(teds_intrusive_dllist_node *node)
{
	return (teds_stricttreeset_it*)((char*)(node) - XtOffsetOf(teds_stricttreeset_it, dllist_node));
}

#define teds_stricttreeset_tree_from_object(obj) (&teds_stricttreeset_from_object((obj))->tree)
#define Z_STRICTTREESET_P(zv)  teds_stricttreeset_from_object(Z_OBJ_P((zv)))
#define Z_STRICTTREESET_TREE_P(zv) (&Z_STRICTTREESET_P((zv))->tree)

static zend_always_inline bool teds_stricttreeset_tree_uninitialized(teds_stricttreeset_tree *tree)
{
	if (tree->initialized) {
		return false;
	}
	ZEND_ASSERT(tree->root == NULL);
	ZEND_ASSERT(tree->nNumOfElements == 0);
	return true;
}

static zend_always_inline void teds_stricttreeset_tree_set_empty_tree(teds_stricttreeset_tree *tree)
{
	tree->root = NULL;
	tree->nNumOfElements = 0;
	tree->initialized = true;
}

static teds_stricttreeset_node *teds_stricttreeset_node_build_tree_from_sorted_nodes_helper(teds_stricttreeset_node **nodes, const uint32_t n, teds_stricttreeset_node *left_parent, teds_stricttreeset_node *right_parent, int leaf_depth)
{
	ZEND_ASSERT(n > 0);
	const uint32_t mid = n/2;
	teds_stricttreeset_node *const root = nodes[mid];
	ZEND_ASSERT(leaf_depth >= 0);
	TEDS_STRICTTREESET_NODE_COLOR(root) = (leaf_depth == 0 ? TEDS_NODE_RED : TEDS_NODE_BLACK);
	leaf_depth--;
	{
		if (mid > 0) {
			teds_stricttreeset_node *const left = teds_stricttreeset_node_build_tree_from_sorted_nodes_helper(nodes, mid, left_parent, root, leaf_depth);
			root->left = left;
			left->parent = root;
			ZEND_ASSERT(root != left);
		} else {
			root->left = NULL;
		}
	}

	{
		const uint32_t right_count = n - mid - 1;
		if (right_count > 0) {
			teds_stricttreeset_node *const right = teds_stricttreeset_node_build_tree_from_sorted_nodes_helper(nodes + mid + 1, right_count, root, right_parent, leaf_depth);
			root->right = right;
			ZEND_ASSERT(root != right);
			right->parent = root;
		} else {
			root->right = NULL;
		}
	}
	return root;
}

static teds_stricttreeset_node *teds_stricttreeset_node_build_tree_from_sorted_nodes(teds_stricttreeset_node **nodes, const uint32_t n)
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
	return teds_stricttreeset_node_build_tree_from_sorted_nodes_helper(nodes, n, NULL, NULL, leaf_depth);
}


void teds_stricttreeset_tree_init_from_array(teds_stricttreeset_tree *tree, zend_array *values)
{
	zval *val;

	teds_stricttreeset_tree_set_empty_tree(tree);
	if (values->nNumOfElements == 0) {
		return;
	}
	tree->should_rebuild_properties = true;

	/* Optimization for constructing this from a sorted tree */
	teds_stricttreeset_node **nodes = emalloc(values->nNumOfElements * sizeof(teds_stricttreeset_node*));
	teds_stricttreeset_node *prev = NULL;
	uint32_t i = 0;

	ZEND_HASH_FOREACH_VAL(values, val)  {
		ZVAL_DEREF(val);

		if (nodes != NULL) {
			if (i == 0 || teds_stable_compare(val, &prev->key) > 0) {
				prev = teds_stricttreeset_node_alloc(val, NULL);
				nodes[i] = prev;
				i++;
				continue;
			}
			tree->root = teds_stricttreeset_node_build_tree_from_sorted_nodes(nodes, i);
			tree->nNumOfElements = i;
			efree(nodes);
			nodes = NULL;
		}
		teds_stricttreeset_tree_insert(tree, val, false);
	} ZEND_HASH_FOREACH_END();

	if (nodes != NULL) {
		tree->root = teds_stricttreeset_node_build_tree_from_sorted_nodes(nodes, i);
		tree->nNumOfElements = i;
		efree(nodes);
	}
}

void teds_stricttreeset_tree_init_from_traversable(teds_stricttreeset_tree *tree, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	teds_stricttreeset_tree_set_empty_tree(tree);
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
			goto cleanup_iter;
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
		const bool created_new_entry = teds_stricttreeset_tree_insert(tree, value, false);
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

cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
	if (UNEXPECTED(EG(exception))) {
		teds_stricttreeset_tree_clear(tree);
	}
}

static teds_stricttreeset_node *teds_stricttreeset_node_copy_ctor_recursive(const teds_stricttreeset_node *from, teds_stricttreeset_node *parent, teds_stricttreeset_node *left_parent_node, teds_stricttreeset_node *right_parent_node) {
	ZEND_ASSERT(from != NULL);
	teds_stricttreeset_node *copy = teds_stricttreeset_node_alloc(&from->key, parent);
	if (from->left) {
		copy->left = teds_stricttreeset_node_copy_ctor_recursive(from->left, copy, left_parent_node, copy);
	} else {
		copy->left = NULL;
	}
	if (from->right) {
		copy->right = teds_stricttreeset_node_copy_ctor_recursive(from->right, copy, copy, right_parent_node);
	} else {
		copy->right = NULL;
	}
	return copy;
}

static void teds_stricttreeset_tree_copy_ctor(teds_stricttreeset_tree *to, teds_stricttreeset_tree *from)
{
	teds_stricttreeset_tree_set_empty_tree(to);
	/* Copy the original tree structure. It will be balanced if the original tree is balanced. */
	to->nNumOfElements = from->nNumOfElements;
	to->should_rebuild_properties = true;
	to->initialized = true;
	if (!teds_stricttreeset_tree_empty_size(from)) {
		to->root = teds_stricttreeset_node_copy_ctor_recursive(from->root, NULL, NULL, NULL);
	} else {
		to->root = NULL;
	}
}

static void teds_stricttreeset_node_dtor(teds_stricttreeset_node *node)
{
	/* Free keys in sorted order */
	while (node != NULL) {
		/* Free the left side */
		teds_stricttreeset_node_dtor(node->left);
		/* Free the current node */
		teds_stricttreeset_node *right = node->right;
		zval_ptr_dtor(&node->key);
		teds_stricttreeset_node_release(node);
		/* Free the right side in the next iteration */
		node = right;
	}
}

/* Destructs and frees contents and the tree itself.
 * If you want to re-use the tree then you need to re-initialize it.
 */
void teds_stricttreeset_tree_dtor(teds_stricttreeset_tree *tree)
{
	if (teds_stricttreeset_tree_empty_size(tree)) {
		return;
	}
	teds_stricttreeset_node *root = tree->root;
	teds_stricttreeset_tree_set_empty_tree(tree);
	teds_stricttreeset_node_dtor(root);
}

static HashTable* teds_stricttreeset_get_gc(zend_object *obj, zval **table, int *table_count)
{
	teds_stricttreeset *intern = teds_stricttreeset_from_object(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (intern->tree.nNumOfElements > 0) {
		zval *key;

		TEDS_STRICTTREESET_FOREACH_KEY(&intern->tree, key) {
			zend_get_gc_buffer_add_zval(gc_buffer, key);
		} TEDS_STRICTTREESET_FOREACH_END();
	}
	/* Overwrites table and table_count. The caller does not initialize these. */
	zend_get_gc_buffer_use(gc_buffer, table, table_count);

	return obj->properties;
}

static HashTable* teds_stricttreeset_get_and_populate_properties(zend_object *obj)
{
	teds_stricttreeset_tree *const tree = teds_stricttreeset_tree_from_object(obj);
	HashTable *ht = zend_std_get_properties(obj);

	/* Re-initialize properties array */
	/*
	 * Usually, the reference count of the hash table is 1,
	 * except during cyclic reference cycles.
	 *
	 * Maintain the DEBUG invariant that a hash table isn't modified during iteration,
	 * and avoid unnecessary work rebuilding a hash table for unmodified properties.
	 *
	 * See https://github.com/php/php-src/issues/8079 and tests/Deque/var_export_recursion.phpt
	 * Also see https://github.com/php/php-src/issues/8044 for alternate considered approaches.
	 */
	if (!tree->should_rebuild_properties) {
		return ht;
	}
	tree->should_rebuild_properties = false;
	if (!tree->nNumOfElements && !zend_hash_num_elements(ht)) {
		/* Nothing to add, update, or remove. */
		return ht;
	}
	if (UNEXPECTED(GC_REFCOUNT(ht) > 1)) {
		obj->properties = zend_array_dup(ht);
		GC_DELREF(ht);
	}

	// Note that destructors may mutate the original array.
	// FIXME copy to a temporary buffer
	uint32_t i = 0;
	zval *elem;
	TEDS_STRICTTREESET_FOREACH_KEY(tree, elem) {
		Z_TRY_ADDREF_P(elem);
		zend_hash_index_update(ht, i, elem);
		i++;
	} TEDS_STRICTTREESET_FOREACH_END();

	const uint32_t properties_size = zend_hash_num_elements(ht);
	if (UNEXPECTED(properties_size > i)) {
		for (; i < properties_size; i++) {
			zend_hash_index_del(ht, i);
		}
	}
#if PHP_VERSION_ID >= 80200
	if (HT_IS_PACKED(ht)) {
		/* Engine doesn't expect packed array */
		zend_hash_packed_to_hash(ht);
	}
#endif

	return ht;
}

static zend_array *teds_stricttreeset_tree_to_refcounted_array(const teds_stricttreeset_tree *tree);

static HashTable* teds_stricttreeset_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	teds_stricttreeset_tree *tree = teds_stricttreeset_tree_from_object(obj);
	if (!tree->nNumOfElements && !obj->properties) {
		/* Similar to ext/ffi/ffi.c zend_fake_get_properties */
		/* debug_zval_dump DEBUG purpose requires null or a refcounted array. */
		return NULL;
	}
	switch (purpose) {
		case ZEND_PROP_PURPOSE_JSON: /* jsonSerialize and get_properties() is used instead. */
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_DEBUG: {
			HashTable *ht = teds_stricttreeset_get_and_populate_properties(obj);
			GC_TRY_ADDREF(ht);
			return ht;
		}
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
		case ZEND_PROP_PURPOSE_SERIALIZE:
			return teds_stricttreeset_tree_to_refcounted_array(tree);
		default:
			ZEND_UNREACHABLE();
			return NULL;
	}
}

static void teds_stricttreeset_free_storage(zend_object *object)
{
	teds_stricttreeset *intern = teds_stricttreeset_from_object(object);
	teds_stricttreeset_tree_dtor(&intern->tree);
	zend_object_std_dtor(&intern->std);
}

static zend_object *teds_stricttreeset_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	teds_stricttreeset *intern;

	intern = zend_object_alloc(sizeof(teds_stricttreeset), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == teds_ce_StrictTreeSet);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &teds_handler_StrictTreeSet;

	if (orig && clone_orig) {
		teds_stricttreeset *other = teds_stricttreeset_from_object(orig);
		teds_stricttreeset_tree_copy_ctor(&intern->tree, &other->tree);
	} else {
		intern->tree.root = NULL;
	}

	return &intern->std;
}

static zend_object *teds_stricttreeset_new(zend_class_entry *class_type)
{
	return teds_stricttreeset_new_ex(class_type, NULL, 0);
}


static zend_object *teds_stricttreeset_clone(zend_object *old_object)
{
	zend_object *new_object = teds_stricttreeset_new_ex(old_object->ce, old_object, 1);

	teds_assert_object_has_empty_member_list(new_object);

	return new_object;
}

static int teds_stricttreeset_count_elements(zend_object *object, zend_long *count)
{
	teds_stricttreeset *intern;

	intern = teds_stricttreeset_from_object(object);
	*count = intern->tree.nNumOfElements;
	return SUCCESS;
}

/* Get number of entries in this StrictTreeSet */
PHP_METHOD(Teds_StrictTreeSet, count)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_stricttreeset *intern = Z_STRICTTREESET_P(object);
	RETURN_LONG(intern->tree.nNumOfElements);
}

/* Get whether this StrictTreeSet is empty */
PHP_METHOD(Teds_StrictTreeSet, isEmpty)
{
	zval *object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_NONE();

	teds_stricttreeset *intern = Z_STRICTTREESET_P(object);
	RETURN_BOOL(intern->tree.nNumOfElements == 0);
}

/* Create this from an iterable */
PHP_METHOD(Teds_StrictTreeSet, __construct)
{
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS);

	if (UNEXPECTED(!teds_stricttreeset_tree_uninitialized(&intern->tree))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Teds\\StrictTreeSet::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		teds_stricttreeset_tree_set_empty_tree(&intern->tree);
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			teds_stricttreeset_tree_init_from_array(&intern->tree, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			teds_stricttreeset_tree_init_from_traversable(&intern->tree, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Teds_StrictTreeSet, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void teds_stricttreeset_it_dtor(zend_object_iterator *iter)
{
	teds_intrusive_dllist_remove(&Z_STRICTTREESET_TREE_P(&iter->data)->active_iterators, &((teds_stricttreeset_it*)iter)->dllist_node);
	zval_ptr_dtor(&iter->data);
}

static void teds_stricttreeset_it_rewind(zend_object_iterator *iter)
{
	teds_stricttreeset *object   = Z_STRICTTREESET_P(&iter->data);
	teds_stricttreeset_node *const new_node = teds_stricttreeset_tree_get_first(&object->tree);
	((teds_stricttreeset_it*)iter)->node = new_node;
	((teds_stricttreeset_it*)iter)->is_before_first = new_node == NULL;
}

static zend_always_inline bool teds_stricttreeset_node_valid(const teds_stricttreeset_node *node) {
	return node != NULL;
}

static int teds_stricttreeset_it_valid(zend_object_iterator *iter)
{
	teds_stricttreeset_it     *iterator = (teds_stricttreeset_it*)iter;
	return teds_stricttreeset_node_valid(iterator->node) ? SUCCESS : FAILURE;
}

static zval *teds_stricttreeset_it_get_current_data(zend_object_iterator *iter)
{
	teds_stricttreeset_node *node = ((teds_stricttreeset_it*)iter)->node;
	if (teds_stricttreeset_node_valid(node)) {
		return &node->key;
	} else {
		return &EG(uninitialized_zval);
	}
}

static void teds_stricttreeset_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	teds_stricttreeset_node *node = ((teds_stricttreeset_it*)iter)->node;
	if (teds_stricttreeset_node_valid(node)) {
		ZVAL_COPY(key, &node->key);
	} else {
		ZVAL_NULL(key);
	}
}

static void teds_stricttreeset_it_move_forward(zend_object_iterator *iter)
{
	teds_stricttreeset_it *tree_iter = (teds_stricttreeset_it *)iter;
	const teds_stricttreeset_node *const node = tree_iter->node;
	if (!teds_stricttreeset_node_valid(node)) {
		if (tree_iter->is_before_first) {
			teds_stricttreeset_tree *tree = Z_STRICTTREESET_TREE_P(&iter->data);
			tree_iter->node = teds_stricttreeset_tree_get_first(tree);
			tree_iter->is_before_first = false;
		}
		return;
	}
	teds_stricttreeset_node *const next = teds_stricttreeset_node_get_next(node);
	tree_iter->node = next;
	ZEND_ASSERT(!tree_iter->is_before_first);
}

/* iterator handler table */
static const zend_object_iterator_funcs teds_stricttreeset_it_funcs = {
	teds_stricttreeset_it_dtor,
	teds_stricttreeset_it_valid,
	teds_stricttreeset_it_get_current_data,
	teds_stricttreeset_it_get_current_key,
	teds_stricttreeset_it_move_forward,
	teds_stricttreeset_it_rewind,
	NULL,
	teds_internaliterator_get_gc,
};


zend_object_iterator *teds_stricttreeset_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	teds_stricttreeset_it *iterator;

	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(teds_stricttreeset_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	zend_object *obj = Z_OBJ_P(object);
	ZVAL_OBJ_COPY(&iterator->intern.data, obj);
	iterator->intern.funcs = &teds_stricttreeset_it_funcs;
	teds_stricttreeset_node *node = teds_stricttreeset_tree_get_first(&Z_STRICTTREESET_P(object)->tree);
	iterator->node = node;
	(void) ce;

	teds_intrusive_dllist_prepend(&teds_stricttreeset_tree_from_object(obj)->active_iterators, &iterator->dllist_node);

	return &iterator->intern;
}

PHP_METHOD(Teds_StrictTreeSet, __unserialize)
{
	HashTable *raw_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	teds_stricttreeset *const intern = Z_STRICTTREESET_P(ZEND_THIS);
	teds_stricttreeset_tree *const tree = &intern->tree;
	if (UNEXPECTED(!teds_stricttreeset_tree_uninitialized(tree))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}

	teds_stricttreeset_tree_set_empty_tree(tree);
	if (raw_data->nNumOfElements == 0) {
		return;
	}

	zend_string *str;
	zval *val;

	teds_stricttreeset_node **nodes = emalloc(raw_data->nNumOfElements * sizeof(teds_stricttreeset_node*));
	teds_stricttreeset_node *prev = NULL;
	uint32_t sorted_nodes_count = 0;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			teds_stricttreeset_tree_clear(tree);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Teds\\StrictTreeSet::__unserialize saw unexpected string key, expected sequence of values", 0);
			if (nodes != NULL) {
				while (sorted_nodes_count > 0) {
					teds_stricttreeset_node *n = nodes[--sorted_nodes_count];
					zval_ptr_dtor(&n->key);
					teds_stricttreeset_node_release(n);
				}
				efree(nodes);
			}
			RETURN_THROWS();
		}

		if (nodes != NULL) {
			if (EXPECTED(sorted_nodes_count == 0 || teds_stable_compare(val, &prev->key) > 0)) {
				prev = teds_stricttreeset_node_alloc(val, NULL);
				nodes[sorted_nodes_count] = prev;
				sorted_nodes_count++;
				continue;
			}
			tree->root = teds_stricttreeset_node_build_tree_from_sorted_nodes(nodes, sorted_nodes_count);
			tree->nNumOfElements = sorted_nodes_count;
			efree(nodes);
			/* With nodes being NULL, sorted_nodes_count is no longer read from */
			nodes = NULL;
		}
		teds_stricttreeset_tree_insert(tree, val, false);
	} ZEND_HASH_FOREACH_END();

	if (nodes != NULL) {
		tree->root = teds_stricttreeset_node_build_tree_from_sorted_nodes(nodes, sorted_nodes_count);
		tree->nNumOfElements = sorted_nodes_count;
		tree->should_rebuild_properties = true;
		efree(nodes);
	}
}

PHP_METHOD(Teds_StrictTreeSet, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = teds_stricttreeset_new(teds_ce_StrictTreeSet);
	teds_stricttreeset *intern = teds_stricttreeset_from_object(object);
	teds_stricttreeset_tree_init_from_array(&intern->tree, array_ht);

	RETURN_OBJ(object);
}

#define IMPLEMENT_READ_POSITION_PHP_METHOD(pos) \
PHP_METHOD(Teds_StrictTreeSet, pos) \
{ \
       ZEND_PARSE_PARAMETERS_NONE(); \
       const teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS); \
       if (teds_stricttreeset_tree_empty_size(&intern->tree)) { \
               zend_throw_exception(spl_ce_UnderflowException, "Cannot read " # pos " value of empty StrictTreeSet", 0); \
               RETURN_THROWS(); \
       } \
       teds_stricttreeset_node *node = teds_stricttreeset_tree_get_ ## pos(&intern->tree); \
       RETVAL_COPY(&node->key); \
}

IMPLEMENT_READ_POSITION_PHP_METHOD(first)
IMPLEMENT_READ_POSITION_PHP_METHOD(last)

#define IMPLEMENT_REMOVE_POSITION_PHP_METHOD(methodName, pos) \
PHP_METHOD(Teds_StrictTreeSet, methodName) \
{ \
	ZEND_PARSE_PARAMETERS_NONE(); \
	teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS); \
	if (teds_stricttreeset_tree_empty_size(&intern->tree)) { \
		zend_throw_exception(spl_ce_UnderflowException, "Cannot " # methodName " from empty StrictTreeSet", 0); \
		RETURN_THROWS(); \
	} \
	teds_stricttreeset_node *node = teds_stricttreeset_tree_get_ ## pos(&intern->tree); \
	RETVAL_COPY_VALUE(&node->key); \
	teds_stricttreeset_tree_remove_node(&intern->tree, node, false); \
}

IMPLEMENT_REMOVE_POSITION_PHP_METHOD(pop, last)
IMPLEMENT_REMOVE_POSITION_PHP_METHOD(shift, first)

static zend_array *teds_stricttreeset_tree_to_refcounted_array(const teds_stricttreeset_tree *tree)
{
	const uint32_t len = tree->nNumOfElements;
	zend_array *values = zend_new_array(len);
	/* Initialize return tree */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return tree */
	ZEND_HASH_FILL_PACKED(values) {
		zval *value;
		TEDS_STRICTTREESET_FOREACH_KEY(tree, value) {
			Z_TRY_ADDREF_P(value);
			ZEND_HASH_FILL_ADD(value);
		} TEDS_STRICTTREESET_FOREACH_END();
	} ZEND_HASH_FILL_END();
	return values;
}

PHP_METHOD(Teds_StrictTreeSet, values)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricttreeset_tree *tree = Z_STRICTTREESET_TREE_P(ZEND_THIS);
	if (!tree->nNumOfElements) {
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(teds_stricttreeset_tree_to_refcounted_array(tree));
}

PHP_METHOD(Teds_StrictTreeSet, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricttreeset_tree *const tree = Z_STRICTTREESET_TREE_P(ZEND_THIS);
	if (!tree->nNumOfElements) {
		RETURN_EMPTY_ARRAY();
	}

	zend_array *values = zend_new_array(tree->nNumOfElements);
	zval *val;
	/* TODO: Check for side effects of notices */
	TEDS_STRICTTREESET_FOREACH_KEY(tree, val) {
		Z_TRY_ADDREF_P(val);
		array_set_zval_key(values, val, val);
		zval_ptr_dtor_nogc(val);
		if (UNEXPECTED(EG(exception))) {
			zend_array_destroy(values);
			RETURN_THROWS();
		}
	} TEDS_STRICTTREESET_FOREACH_END();
	RETURN_ARR(values);
}

static void teds_stricttreeset_tree_replace_node_with_child(teds_stricttreeset_tree *tree, teds_stricttreeset_node *removed_node, teds_stricttreeset_node *child_node) {
	teds_stricttreeset_node *const parent = removed_node->parent;
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

static zend_always_inline teds_stricttreeset_node *teds_stricttreeset_node_remove_leftmost(teds_stricttreeset_node *node) {
	while (true) {
		ZEND_ASSERT(node != NULL);
		ZEND_ASSERT(node->parent != NULL);
		ZEND_ASSERT(node->parent != node);
		if (node->left) {
			ZEND_ASSERT(node->left->parent == node);
			node = node->left;
			continue;
		}
		teds_stricttreeset_node *const parent = node->parent;
		teds_stricttreeset_node *const right = node->right;
		if (right) {
			ZEND_ASSERT(right->parent == node);
			RBTREE_INVARIANT_ASSERT(TEDS_STRICTTREESET_NODE_COLOR(right) == TEDS_NODE_RED);
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

static void teds_stricttreeset_tree_rebalance_after_removal(teds_stricttreeset_tree *tree, teds_stricttreeset_node *p) {
	/* Based on https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Removal:_simple_cases */
	/* p is the parent of the removed node n. Before being removed, n was a black node with 2 children. */
	ZEND_ASSERT(p != NULL);
	/* Side of the parent on which the value with no children was removed */
	teds_stricttreeset_node *n = NULL;
	teds_stricttreeset_node *root = tree->root;
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
		teds_stricttreeset_node *s = p->children[1 - dir];
		RBTREE_INVARIANT_ASSERT(s != NULL);
		if (UNEXPECTED(s == NULL)) {
			return;
		}
		ZEND_ASSERT(s->parent == p);
		/* close sibling on same side of sibling as node n is of parent p */
		teds_stricttreeset_node *c = s->children[    dir];
		ZEND_ASSERT(!c || c->parent == s);
		/* distant sibling on opposite side of sibling as node n is of parent p */
		teds_stricttreeset_node *d = s->children[1 - dir];
		ZEND_ASSERT(!d || d->parent == s);

		const bool s_black = TEDS_STRICTTREESET_NODE_COLOR(s) != TEDS_NODE_RED;
		const bool c_black = TEDS_STRICTTREESET_NODE_COLOR_NULLABLE(c) != TEDS_NODE_RED;
		const bool d_black = TEDS_STRICTTREESET_NODE_COLOR_NULLABLE(d) != TEDS_NODE_RED;
		const bool p_black = TEDS_STRICTTREESET_NODE_COLOR(p) != TEDS_NODE_RED;
		if (s_black) {
			if (!d_black) {
				/* Delete case 6 - s is black, d is red */
				DEBUG_PRINTF("Delete case 6\n");
delete_case_6:
				DEBUG_PRINTF("Delete case 6 inner\n");
				teds_stricttreeset_tree_rotate_dir_root(tree, p, 1 - dir);
				TEDS_STRICTTREESET_NODE_COLOR(s) = TEDS_STRICTTREESET_NODE_COLOR(p);
				TEDS_STRICTTREESET_NODE_COLOR(p) = TEDS_NODE_BLACK;
				TEDS_STRICTTREESET_NODE_COLOR(d) = TEDS_NODE_BLACK;
				return;
			}
			if (c_black) {
				if (p_black) {
					/* Delete case 1 - change sibling to red node and recurse on black node parent*/
					n = p;
					p = p->parent;
					TEDS_STRICTTREESET_NODE_COLOR(s) = TEDS_NODE_RED;
					DEBUG_PRINTF("Delete case 1\n");
					continue;
				} else {
					/* Delete case 4 - change parent to black node and sibling to red node, making path counts balanced. */
					DEBUG_PRINTF("Delete case 4\n");
					TEDS_STRICTTREESET_NODE_COLOR(p) = TEDS_NODE_BLACK;
					TEDS_STRICTTREESET_NODE_COLOR(s) = TEDS_NODE_RED;
					return;
				}
			} else {
				DEBUG_PRINTF("Delete case 5\n");
				/* Delete case 5: c is red, s is black, d is black */
				ZEND_ASSERT(tree->root != s);
				teds_stricttreeset_node_rotate_dir(s, dir);
				TEDS_STRICTTREESET_NODE_COLOR(s) = TEDS_NODE_RED;
				TEDS_STRICTTREESET_NODE_COLOR(c) = TEDS_NODE_BLACK;
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
			teds_stricttreeset_tree_rotate_dir_root(tree, p, 1 - dir);
			TEDS_STRICTTREESET_NODE_COLOR(p) = TEDS_NODE_RED;
			TEDS_STRICTTREESET_NODE_COLOR(s) = TEDS_NODE_BLACK;
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

static zend_always_inline void teds_stricttreeset_tree_replace_node_with_descendant_and_rebalance(teds_stricttreeset_tree *tree, teds_stricttreeset_node *node) {
	teds_stricttreeset_node *const replacement = teds_stricttreeset_node_remove_leftmost(node->right);
	teds_stricttreeset_node *const parent = node->parent;
	ZEND_ASSERT(replacement->parent != NULL);
	/* parent_of_removed_node is non-null if replacement is a black node with no children. */
	teds_stricttreeset_node *parent_of_removed_node;
	if (replacement->right == NULL && TEDS_STRICTTREESET_NODE_COLOR(replacement) == TEDS_NODE_BLACK) {
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

	TEDS_STRICTTREESET_NODE_COLOR(replacement) = TEDS_STRICTTREESET_NODE_COLOR(node);
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

	if (parent_of_removed_node) {
		/* Paths going through the replaced replacement node (black with both nil children)
		 * now have one less black node than all other paths and this needs to be rebalanced. */
		teds_stricttreeset_tree_rebalance_after_removal(tree, parent_of_removed_node);
		/* replacement was a black node with no children */
	}
}

static void teds_stricttreeset_tree_adjust_iterators_before_remove(teds_intrusive_dllist_node *dllist_node, const teds_stricttreeset_node *const tree_node)
{
	ZEND_ASSERT(tree_node != NULL);
	do {
		teds_stricttreeset_it *it = teds_stricttreeset_it_from_dllist_node(dllist_node);
		if (it->node == tree_node) {
			teds_stricttreeset_node *prev = teds_stricttreeset_node_get_prev(tree_node);
			it->node = prev;
			it->is_before_first = prev == NULL;
		}

		dllist_node = dllist_node->next;
	} while (dllist_node != NULL);
}

static zend_always_inline void teds_stricttreeset_tree_maybe_adjust_iterators_before_remove(teds_stricttreeset_tree *tree, teds_stricttreeset_node *const node) {
	if (UNEXPECTED(tree->active_iterators.first)) {
		teds_stricttreeset_tree_adjust_iterators_before_remove(tree->active_iterators.first, node);
	}
}

static zend_always_inline void teds_stricttreeset_tree_remove_node(teds_stricttreeset_tree *tree, teds_stricttreeset_node *const node, bool free_zvals) {
	teds_stricttreeset_tree_maybe_adjust_iterators_before_remove(tree, node);

	if (!node->left) {
		/* If a valid red-black tree has only 1 child,
		 * then that child is guaranteed to be red, so that the number of black nodes on paths on both sides are equal. */
		teds_stricttreeset_node *const right = node->right;
		ZEND_ASSERT(right == NULL || right->parent == node);
		teds_stricttreeset_tree_replace_node_with_child(tree, node, right);

		if (right == NULL) {
			if (TEDS_STRICTTREESET_NODE_COLOR(node) == TEDS_NODE_BLACK) {
				if (node->parent != NULL) {
					/* This was a black node with no children. The tree needs to be rebalanced if it was not empty. */
					teds_stricttreeset_tree_rebalance_after_removal(tree, node->parent);
				} else {
					ZEND_ASSERT(tree->root == NULL);
				}
			}
		} else {
			/* Change right node's color to black to maintain the invariant */
			RBTREE_INVARIANT_ASSERT(TEDS_STRICTTREESET_NODE_COLOR(node) == TEDS_NODE_BLACK);
			RBTREE_INVARIANT_ASSERT(TEDS_STRICTTREESET_NODE_COLOR(right) == TEDS_NODE_RED);
			TEDS_STRICTTREESET_NODE_COLOR(right) = TEDS_NODE_BLACK;
		}
	} else if (!node->right) {
		ZEND_ASSERT(node->left->parent == node);
		teds_stricttreeset_tree_replace_node_with_child(tree, node, node->left);
		/* Change left node's color to black to maintain the invariant */
		RBTREE_INVARIANT_ASSERT(TEDS_STRICTTREESET_NODE_COLOR(node) == TEDS_NODE_BLACK);
		RBTREE_INVARIANT_ASSERT(TEDS_STRICTTREESET_NODE_COLOR(node->left) == TEDS_NODE_RED);
		TEDS_STRICTTREESET_NODE_COLOR(node->left) = TEDS_NODE_BLACK;
	} else {
		ZEND_ASSERT(node->left->parent == node);
		ZEND_ASSERT(node->right->parent == node);

		teds_stricttreeset_tree_replace_node_with_descendant_and_rebalance(tree, node);
	}

	ZEND_ASSERT(tree->nNumOfElements > 0);
	tree->nNumOfElements--;
	tree->should_rebuild_properties = true;
	if (free_zvals) {
		zval_ptr_dtor(&node->key);
	}
	teds_stricttreeset_node_release(node);
}

static bool teds_stricttreeset_tree_remove_key(teds_stricttreeset_tree *tree, zval *key)
{
	/* FIXME implement binary tree removal */
	teds_stricttreeset_node *const node = teds_stricttreeset_tree_find_key(tree, key);
	if (node == NULL) {
		/* Nothing to remove */
		return false;
	}
	teds_stricttreeset_tree_remove_node(tree, node, true);
	return true;
}

PHP_METHOD(Teds_StrictTreeSet, add)
{
	zval *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS);
	RETURN_BOOL(teds_stricttreeset_tree_insert(&intern->tree, value, false));
}

PHP_METHOD(Teds_StrictTreeSet, remove)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS);
	RETURN_BOOL(teds_stricttreeset_tree_remove_key(&intern->tree, key));
}

PHP_METHOD(Teds_StrictTreeSet, contains)
{
	zval *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	const teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS);
	if (intern->tree.nNumOfElements > 0) {
		teds_stricttreeset_node *entry = teds_stricttreeset_tree_find_key(&intern->tree, key);
		RETURN_BOOL(entry != NULL);
	}
	RETURN_FALSE;
}

static ZEND_COLD void teds_stricttreeset_node_debug_representation(zval *return_value, teds_stricttreeset_node *node, zend_string *strings[4]) {
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
	ZVAL_BOOL(&tmp, TEDS_STRICTTREESET_NODE_COLOR(node) == TEDS_NODE_RED);
	zend_hash_add_new(ht, strings[1], &tmp);

	teds_stricttreeset_node_debug_representation(&tmp, node->left, strings);
	zend_hash_add_new(ht, strings[2], &tmp);

	teds_stricttreeset_node_debug_representation(&tmp, node->right, strings);
	zend_hash_add_new(ht, strings[3], &tmp);
	RETURN_ARR(ht);
}

ZEND_COLD PHP_METHOD(Teds_StrictTreeSet, debugGetTreeRepresentation)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS);
	if (intern->tree.nNumOfElements == 0) {
		RETURN_EMPTY_ARRAY();
	}
	zend_string *strings[4] = {
		zend_string_init_interned("key", sizeof("key") - 1, 0),
		zend_string_init_interned("red", sizeof("red") - 1, 0),
		zend_string_init_interned("left", sizeof("left") - 1, 0),
		zend_string_init_interned("right", sizeof("right") - 1, 0),
	};
	teds_stricttreeset_node_debug_representation(return_value, intern->tree.root, strings);
}

/* Returns -1 for unbalanced tree */
static ZEND_COLD int teds_stricttreeset_node_is_balanced(const teds_stricttreeset_node *node) {
	if (!node) {
		return 0;
	}

	ZEND_ASSERT(!node->left || node->left->parent == node);
	int left_result = teds_stricttreeset_node_is_balanced(node->left);
	if (left_result < 0) {
		return left_result;
	}
	ZEND_ASSERT(!node->right || node->right->parent == node);
	int right_result = teds_stricttreeset_node_is_balanced(node->right);
	if (right_result < 0) {
		return right_result;
	}
	if (left_result != right_result) {
		return -1;
	}
	return left_result + ((TEDS_STRICTTREESET_NODE_COLOR(node) == TEDS_NODE_BLACK) ? 1 : 0);
}

ZEND_COLD PHP_METHOD(Teds_StrictTreeSet, debugIsBalanced)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS);
	int result = teds_stricttreeset_node_is_balanced(intern->tree.root);
	RETURN_BOOL(result >= 0);
}

static void teds_stricttreeset_tree_clear(teds_stricttreeset_tree *tree) {
	if (teds_stricttreeset_tree_empty_size(tree)) {
		return;
	}
	teds_stricttreeset_tree array_copy = *tree;

	teds_stricttreeset_tree_set_empty_tree(tree);

	teds_stricttreeset_tree_dtor(&array_copy);
	/* Could call teds_stricttreeset_get_properties but properties tree is typically not initialized unless var_dump or other inefficient functionality is used */
}

PHP_METHOD(Teds_StrictTreeSet, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	teds_stricttreeset *intern = Z_STRICTTREESET_P(ZEND_THIS);
	teds_stricttreeset_tree_clear(&intern->tree);
	if (intern->std.properties) {
		zend_hash_clean(intern->std.properties);
	}
	TEDS_RETURN_VOID();
}

PHP_MINIT_FUNCTION(teds_stricttreeset)
{
	TEDS_MINIT_IGNORE_UNUSED();
	teds_ce_StrictTreeSet = register_class_Teds_StrictTreeSet(zend_ce_aggregate, teds_ce_Set, php_json_serializable_ce);
	teds_ce_StrictTreeSet->create_object = teds_stricttreeset_new;

	memcpy(&teds_handler_StrictTreeSet, &std_object_handlers, sizeof(zend_object_handlers));

	teds_handler_StrictTreeSet.offset          = XtOffsetOf(teds_stricttreeset, std);
	teds_handler_StrictTreeSet.clone_obj       = teds_stricttreeset_clone;
	teds_handler_StrictTreeSet.count_elements  = teds_stricttreeset_count_elements;
	teds_handler_StrictTreeSet.get_properties_for  = teds_stricttreeset_get_properties_for;
	teds_handler_StrictTreeSet.get_gc          = teds_stricttreeset_get_gc;
	teds_handler_StrictTreeSet.dtor_obj        = zend_objects_destroy_object;
	teds_handler_StrictTreeSet.free_obj        = teds_stricttreeset_free_storage;

	teds_ce_StrictTreeSet->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	teds_ce_StrictTreeSet->get_iterator = teds_stricttreeset_get_iterator;

	return SUCCESS;
}
