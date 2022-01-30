/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_SORTEDSTRICTMAP_H
#define TEDS_SORTEDSTRICTMAP_H

PHP_MINIT_FUNCTION(teds_sortedstrictmap);

typedef struct _teds_sortedstrictmap_node {
	zval key;
	zval value;

	union {
		struct {
			struct _teds_sortedstrictmap_node *left;
			struct _teds_sortedstrictmap_node *right;
		};
		struct _teds_sortedstrictmap_node *children[2];
	};
	struct _teds_sortedstrictmap_node *parent;
	struct _teds_sortedstrictmap_node *prev;
	struct _teds_sortedstrictmap_node *next;
} teds_sortedstrictmap_node;

#define TEDS_SORTEDSTRICTMAP_NODE_REFCOUNT(node) Z_EXTRA((node)->key)
#define TEDS_SORTEDSTRICTMAP_NODE_COLOR(node) Z_EXTRA((node)->value)

typedef struct _teds_sortedstrictmap_tree {
	struct _teds_sortedstrictmap_node *root;
	size_t nNumOfElements;
	bool initialized;
} teds_sortedstrictmap_tree;

typedef struct _teds_sortedstrictmap {
	teds_sortedstrictmap_tree	array;
	zend_object					std;
} teds_sortedstrictmap;

void teds_sortedstrictmap_tree_init_from_array(teds_sortedstrictmap_tree *array, zend_array *values);

void teds_sortedstrictmap_tree_init_from_traversable(teds_sortedstrictmap_tree *array, zend_object *obj);

void teds_sortedstrictmap_tree_dtor(teds_sortedstrictmap_tree *array);

void teds_sortedstrictmap_tree_dtor_range(teds_sortedstrictmap_node *start, size_t from, size_t to);

static zend_always_inline teds_sortedstrictmap_node *teds_sortedstrictmap_tree_get_first(const teds_sortedstrictmap_tree *tree)
{
	teds_sortedstrictmap_node *it = tree->root;
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

static zend_always_inline teds_sortedstrictmap_node *teds_sortedstrictmap_tree_get_last(const teds_sortedstrictmap_tree *tree)
{
	teds_sortedstrictmap_node *it = tree->root;
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

#define TEDS_SORTEDSTRICTMAP_FOREACH(_sortedstrictmap) do { \
	const teds_sortedstrictmap_tree *const __sortedstrictmap = (_sortedstrictmap); \
	teds_sortedstrictmap_node *_p = teds_sortedstrictmap_tree_get_first(__sortedstrictmap); \
	if (_p) { ZEND_ASSERT(__sortedstrictmap->nNumOfElements > 0); } else { ZEND_ASSERT(__sortedstrictmap->nNumOfElements == 0); } \
	while (_p != NULL) {

#define TEDS_SORTEDSTRICTMAP_FOREACH_END() \
		_p = _p->next; \
	} \
} while (0)

#define TEDS_SORTEDSTRICTMAP_FOREACH_KEY_VAL(sortedstrictmap, k, v) TEDS_SORTEDSTRICTMAP_FOREACH(sortedstrictmap) \
	k = &_p->key; \
	v = &_p->value;

#define TEDS_SORTEDSTRICTMAP_FOREACH_KEY(sortedstrictmap, k) TEDS_SORTEDSTRICTMAP_FOREACH(sortedstrictmap) \
	k = &_p->key;

#define TEDS_SORTEDSTRICTMAP_FOREACH_VAL(sortedstrictmap, v) TEDS_SORTEDSTRICTMAP_FOREACH(sortedstrictmap) \
	v = &_p->value;

#define TEDS_SORTEDSTRICTMAP_FOREACH_NODE(sortedstrictmap, node) TEDS_SORTEDSTRICTMAP_FOREACH(sortedstrictmap) \
	node = _p;


/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_sortedstrictmap_tree_empty_size(const teds_sortedstrictmap_tree *array)
{
	if (array->nNumOfElements > 0) {
		ZEND_ASSERT(array->root != NULL);
		ZEND_ASSERT(array->initialized);
		return false;
	}
	ZEND_ASSERT(array->root == NULL);
	return true;
}

#endif	/* TEDS_SORTEDSTRICTMAP_H */
