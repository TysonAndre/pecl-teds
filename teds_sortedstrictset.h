/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_SORTEDSTRICTSET_H
#define TEDS_SORTEDSTRICTSET_H

PHP_MINIT_FUNCTION(teds_sortedstrictset);

typedef struct _teds_sortedstrictset_node {
	zval key;
	union {
		struct {
			struct _teds_sortedstrictset_node *left;
			struct _teds_sortedstrictset_node *right;
		};
		struct _teds_sortedstrictset_node *children[2];
	};
	struct _teds_sortedstrictset_node *parent;
	struct _teds_sortedstrictset_node *prev;
	struct _teds_sortedstrictset_node *next;
	uint8_t color;
} teds_sortedstrictset_node;

#define TEDS_SORTEDSTRICTSET_NODE_REFCOUNT(node) Z_EXTRA((node)->key)
#define TEDS_SORTEDSTRICTSET_NODE_COLOR(node) ((node)->color)

typedef struct _teds_sortedstrictset_tree {
	struct _teds_sortedstrictset_node *root;
	size_t nNumOfElements;
	bool initialized;
} teds_sortedstrictset_tree;

typedef struct _teds_sortedstrictset {
	teds_sortedstrictset_tree	array;
	zend_object					std;
} teds_sortedstrictset;

void teds_sortedstrictset_tree_init_from_array(teds_sortedstrictset_tree *array, zend_array *values);

void teds_sortedstrictset_tree_init_from_traversable(teds_sortedstrictset_tree *array, zend_object *obj);

void teds_sortedstrictset_tree_dtor(teds_sortedstrictset_tree *array);

void teds_sortedstrictset_tree_dtor_range(teds_sortedstrictset_node *start, size_t from, size_t to);

static zend_always_inline teds_sortedstrictset_node *teds_sortedstrictset_tree_get_first(const teds_sortedstrictset_tree *tree)
{
	teds_sortedstrictset_node *it = tree->root;
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

static zend_always_inline teds_sortedstrictset_node *teds_sortedstrictset_tree_get_last(const teds_sortedstrictset_tree *tree)
{
	teds_sortedstrictset_node *it = tree->root;
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

#define TEDS_SORTEDSTRICTSET_FOREACH(_sortedstrictset) do { \
	const teds_sortedstrictset_tree *const __sortedstrictset = (_sortedstrictset); \
	teds_sortedstrictset_node *_p = teds_sortedstrictset_tree_get_first(__sortedstrictset); \
	if (_p) { ZEND_ASSERT(__sortedstrictset->nNumOfElements > 0); } else { ZEND_ASSERT(__sortedstrictset->nNumOfElements == 0); } \
	while (_p != NULL) {

#define TEDS_SORTEDSTRICTSET_FOREACH_END() \
		_p = _p->next; \
	} \
} while (0)

#define TEDS_SORTEDSTRICTSET_FOREACH_KEY(sortedstrictset, k) TEDS_SORTEDSTRICTSET_FOREACH(sortedstrictset) \
	k = &_p->key;

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_sortedstrictset_tree_empty_size(const teds_sortedstrictset_tree *array)
{
	if (array->nNumOfElements > 0) {
		ZEND_ASSERT(array->root != NULL);
		ZEND_ASSERT(array->initialized);
		return false;
	}
	ZEND_ASSERT(array->root == NULL);
	return true;
}

#endif	/* TEDS_SORTEDSTRICTSET_H */
