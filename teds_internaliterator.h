#ifndef TEDS_INTERNALITERATOR_H
#define TEDS_INTERNALITERATOR_H

typedef struct _teds_intrusive_dllist_node {
	struct _teds_intrusive_dllist_node *prev;
	struct _teds_intrusive_dllist_node *next;
} teds_intrusive_dllist_node;

typedef struct _teds_intrusive_dllist {
	struct _teds_intrusive_dllist_node *first;
} teds_intrusive_dllist;

static zend_always_inline void teds_intrusive_dllist_prepend(teds_intrusive_dllist *list, teds_intrusive_dllist_node *node) {
	teds_intrusive_dllist_node *first = list->first;
	ZEND_ASSERT(node != first);
	node->next = first;
	node->prev = NULL;
	list->first = node;

	if (first) {
		ZEND_ASSERT(first->prev == NULL);
		first->prev = node;
	}
}

static zend_always_inline void teds_intrusive_dllist_remove(teds_intrusive_dllist *list, const teds_intrusive_dllist_node *node) {
	teds_intrusive_dllist_node *next = node->next;
	teds_intrusive_dllist_node *prev = node->prev;
	ZEND_ASSERT(node != next);
	ZEND_ASSERT(node != prev);
	ZEND_ASSERT(next != prev || next == NULL);
	if (next) {
		next->prev = prev;
	}
	if (list->first == node) {
		list->first = next;
		ZEND_ASSERT(prev == NULL);
	} else if (prev) {
		prev->next = next;
	}
}
#endif
