
#include "queue.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr       – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


void queue_init(queue_t *queue) {
    if (ISNULL(queue)) {
        return;
    }
    queue->begin = NULL;
    queue->end = NULL;
}


void queue_init_node(queue_node_t *node, void *link) {
    if (ISNULL(node)) {
        return;
    }
    node->next = NULL;
    node->previous = NULL;
    node->queue = NULL;
    node->link = link;
}


void queue_put_front(queue_t *q, queue_node_t *node) {
    if (ISNULL(q) || ISNULL(node)) {
        return;
    }
    if (!ISNULL(q->begin)) {
        q->begin->previous = node;
    }
    node->queue = q;
    node->next = q->begin;
    node->previous = NULL;
    if (ISNULL(q->begin) && ISNULL(q->end)) {
        q->end = node;
    }
    q->begin = node;
}


void queue_put_back(queue_t *q, queue_node_t *node) {
    if (ISNULL(q) || ISNULL(node)) {
        return;
    }
    if (!ISNULL(q->end)) {
        q->end->next = node;
    }
    node->queue = q;
    node->previous = q->end;
    node->next = NULL;
    if (ISNULL(q->begin) && ISNULL(q->end)) {
        q->begin = node;
    }
    q->end = node;
}


void *queue_delete_node(queue_node_t *node) {
    if (ISNULL(node) || ISNULL(node->queue)) {
        return NULL;
    }
    if (!ISNULL(node->previous)) {
        node->previous->next = node->next;
    } else {
        node->queue->begin = node->next;
    }
    if (!ISNULL(node->next)) {
        node->next->previous = node->previous;
    } else {
        node->queue->end = node->previous;
    }
    node->queue = NULL;
    node->next = NULL;
    node->previous = NULL;
    return node->link;
}
