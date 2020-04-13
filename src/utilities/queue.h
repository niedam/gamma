#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct queue_node queue_node_t;

typedef struct queue {
    queue_node_t *begin;
    queue_node_t *end;
} queue_t;


typedef struct queue_node {
    queue_t *queue;
    queue_node_t *next;
    queue_node_t *previous;
    void *link;
} queue_node_t;

void queue_init(queue_t *queue);

void queue_init_node(queue_node_t *node, void *link);

void queue_put_front(queue_t *q, queue_node_t *node);

void queue_put_back(queue_t *q, queue_node_t *node);

void *queue_delete_node(queue_node_t *node);

static inline void *queue_pop_front(queue_t *queue) {
    return queue_delete_node(queue->begin);
}

static inline bool queue_empty(queue_t *queue) {
    return queue->begin == 0 && queue->end == 0;
}


#endif //GAMMA_QUEUE_H
