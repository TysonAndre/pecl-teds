/*
  +----------------------------------------------------------------------+
  | teds extension for PHP                                               |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef TEDS_STRICTTREESET_H
#define TEDS_STRICTTREESET_H

PHP_MINIT_FUNCTION(teds_stricttreeset);

typedef struct _teds_stricttreeset_node {
	zval key;
	union {
		struct {
			struct _teds_stricttreeset_node *left;
			struct _teds_stricttreeset_node *right;
		};
		struct _teds_stricttreeset_node *children[2];
	};
	struct _teds_stricttreeset_node *parent;
	struct _teds_stricttreeset_node *prev;
	struct _teds_stricttreeset_node *next;
	uint8_t color;
} teds_stricttreeset_node;

#define TEDS_STRICTTREESET_NODE_REFCOUNT(node) Z_EXTRA((node)->key)
#define TEDS_STRICTTREESET_NODE_COLOR(node) ((node)->color)
#define TEDS_STRICTTREESET_NODE_COLOR_NULLABLE(node) ((node) != NULL ? TEDS_STRICTTREESET_NODE_COLOR(node) : TEDS_NODE_BLACK)

typedef struct _teds_stricttreeset_tree {
	struct _teds_stricttreeset_node *root;
	uint32_t nNumOfElements;
	bool initialized;
} teds_stricttreeset_tree;

typedef struct _teds_stricttreeset {
	teds_stricttreeset_tree	array;
	zend_object					std;
} teds_stricttreeset;

void teds_stricttreeset_tree_init_from_array(teds_stricttreeset_tree *array, zend_array *values);

void teds_stricttreeset_tree_init_from_traversable(teds_stricttreeset_tree *array, zend_object *obj);

void teds_stricttreeset_tree_dtor(teds_stricttreeset_tree *array);

void teds_stricttreeset_tree_dtor_range(teds_stricttreeset_node *start, size_t from, size_t to);

static zend_always_inline teds_stricttreeset_node *teds_stricttreeset_tree_get_first(const teds_stricttreeset_tree *tree)
{
	teds_stricttreeset_node *it = tree->root;
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

static zend_always_inline teds_stricttreeset_node *teds_stricttreeset_tree_get_last(const teds_stricttreeset_tree *tree)
{
	teds_stricttreeset_node *it = tree->root;
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

#define TEDS_STRICTTREESET_FOREACH(_stricttreeset) do { \
	const teds_stricttreeset_tree *const __stricttreeset = (_stricttreeset); \
	teds_stricttreeset_node *_p = teds_stricttreeset_tree_get_first(__stricttreeset); \
	if (_p) { ZEND_ASSERT(__stricttreeset->nNumOfElements > 0); } else { ZEND_ASSERT(__stricttreeset->nNumOfElements == 0); } \
	while (_p != NULL) {

#define TEDS_STRICTTREESET_FOREACH_END() \
		_p = _p->next; \
	} \
} while (0)

#define TEDS_STRICTTREESET_FOREACH_KEY(stricttreeset, k) TEDS_STRICTTREESET_FOREACH(stricttreeset) \
	k = &_p->key;

/* Helps enforce the invariants in debug mode:
 *   - if capacity == 0, then entries == NULL
 *   - if capacity > 0, then entries != NULL
 */
static zend_always_inline bool teds_stricttreeset_tree_empty_size(const teds_stricttreeset_tree *array)
{
	if (array->nNumOfElements > 0) {
		ZEND_ASSERT(array->root != NULL);
		ZEND_ASSERT(array->initialized);
		return false;
	}
	ZEND_ASSERT(array->root == NULL);
	return true;
}

#endif	/* TEDS_STRICTTREESET_H */
