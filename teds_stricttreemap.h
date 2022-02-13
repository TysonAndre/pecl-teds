/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STRICTTREEMAP_H
#define TEDS_STRICTTREEMAP_H

PHP_MINIT_FUNCTION(teds_stricttreemap);

typedef struct _teds_stricttreemap_node {
	zval key;
	zval value;

	union {
		struct {
			struct _teds_stricttreemap_node *left;
			struct _teds_stricttreemap_node *right;
		};
		struct _teds_stricttreemap_node *children[2];
	};
	struct _teds_stricttreemap_node *parent;
	struct _teds_stricttreemap_node *prev;
	struct _teds_stricttreemap_node *next;
} teds_stricttreemap_node;

#define TEDS_STRICTTREEMAP_NODE_REFCOUNT(node) Z_EXTRA((node)->key)
#define TEDS_STRICTTREEMAP_NODE_COLOR(node) Z_EXTRA((node)->value)
#define TEDS_STRICTTREEMAP_NODE_COLOR_NULLABLE(node) ((node) != NULL ? TEDS_STRICTTREEMAP_NODE_COLOR(node) : TEDS_NODE_BLACK)

typedef struct _teds_stricttreemap_tree {
	struct _teds_stricttreemap_node *root;
	uint32_t nNumOfElements;
	bool initialized;
} teds_stricttreemap_tree;

typedef struct _teds_stricttreemap {
	teds_stricttreemap_tree	array;
	zend_object					std;
} teds_stricttreemap;

void teds_stricttreemap_tree_init_from_array(teds_stricttreemap_tree *array, zend_array *values);

void teds_stricttreemap_tree_init_from_traversable(teds_stricttreemap_tree *array, zend_object *obj);

void teds_stricttreemap_tree_dtor(teds_stricttreemap_tree *array);

void teds_stricttreemap_tree_dtor_range(teds_stricttreemap_node *start, size_t from, size_t to);

static zend_always_inline teds_stricttreemap_node *teds_stricttreemap_tree_get_first(const teds_stricttreemap_tree *tree)
{
	teds_stricttreemap_node *it = tree->root;
	if (it == NULL) {
		return NULL;
	}
	while (true) {
		if (!it->left) {
			return it;
		}
		it = it->left;
	}
}

static zend_always_inline teds_stricttreemap_node *teds_stricttreemap_tree_get_last(const teds_stricttreemap_tree *tree)
{
	teds_stricttreemap_node *it = tree->root;
	if (it == NULL) {
		return NULL;
	}
	while (true) {
		if (!it->right) {
			return it;
		}
		it = it->right;
	}
}

#define TEDS_STRICTTREEMAP_FOREACH(_stricttreemap) do { \
	const teds_stricttreemap_tree *const __stricttreemap = (_stricttreemap); \
	teds_stricttreemap_node *_p = teds_stricttreemap_tree_get_first(__stricttreemap); \
	if (_p) { ZEND_ASSERT(__stricttreemap->nNumOfElements > 0); } else { ZEND_ASSERT(__stricttreemap->nNumOfElements == 0); } \
	while (_p != NULL) {

#define TEDS_STRICTTREEMAP_FOREACH_END() \
		_p = _p->next; \
	} \
} while (0)

#define TEDS_STRICTTREEMAP_FOREACH_KEY_VAL(stricttreemap, k, v) TEDS_STRICTTREEMAP_FOREACH(stricttreemap) \
	k = &_p->key; \
	v = &_p->value;

#define TEDS_STRICTTREEMAP_FOREACH_KEY(stricttreemap, k) TEDS_STRICTTREEMAP_FOREACH(stricttreemap) \
	k = &_p->key;

#define TEDS_STRICTTREEMAP_FOREACH_VAL(stricttreemap, v) TEDS_STRICTTREEMAP_FOREACH(stricttreemap) \
	v = &_p->value;

#define TEDS_STRICTTREEMAP_FOREACH_NODE(stricttreemap, node) TEDS_STRICTTREEMAP_FOREACH(stricttreemap) \
	node = _p;


/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_stricttreemap_tree_empty_size(const teds_stricttreemap_tree *array)
{
	if (array->nNumOfElements > 0) {
		ZEND_ASSERT(array->root != NULL);
		ZEND_ASSERT(array->initialized);
		return false;
	}
	ZEND_ASSERT(array->root == NULL);
	return true;
}

#endif	/* TEDS_STRICTTREEMAP_H */
