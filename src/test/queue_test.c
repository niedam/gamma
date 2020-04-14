#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include "cmocka.h"
#include "../utilities/queue.h"


static void node_init(void **state) {
    (void) state; /* unused */
    queue_node_t node1;
    queue_init_node(&node1, NULL);
    assert_ptr_equal(node1.queue, NULL);
    assert_ptr_equal(node1.next, NULL);
    assert_ptr_equal(node1.previous, NULL);
    assert_ptr_equal(node1.link, NULL);
    queue_node_t node2;
    int number = 42;
    queue_init_node(&node2, &number);
    assert_ptr_equal(node2.queue, NULL);
    assert_ptr_equal(node2.next, NULL);
    assert_ptr_equal(node2.previous, NULL);
    assert_ptr_equal(node2.link, &number);
    assert_int_equal(number, *((int*) node2.link));
}


void queue_one_element(void **state) {
    (void) state;
    queue_t queue;
    queue_init(&queue);
    queue_node_t node;
    queue_init_node(&node, NULL);
    queue_put_front(&queue, &node);
    assert_ptr_equal(node.queue, &queue);
    assert_null(node.next);
    assert_null(node.previous);
    queue_t queue2;
    queue_init(&queue2);
    queue_node_t node2;
    queue_init_node(&node2, NULL);
    queue_put_back(&queue2, &node2);
    assert_ptr_equal(node2.queue, &queue2);
    assert_null(node2.next);
    assert_null(node2.previous);
}


void del_middle_node(void **state) {
    (void) state;
    queue_t queue;
    queue_init(&queue);
    queue_node_t qNode[3];
    for (int i = 0; i < 3; ++i) {
        queue_init_node(&qNode[i], NULL);
    }
    queue_put_front(&queue, &qNode[2]);
    queue_put_front(&queue, &qNode[1]);
    queue_put_front(&queue, &qNode[0]);

    queue_delete_node(&qNode[1]);
    assert_null(qNode[1].queue);
    assert_null(qNode[1].next);
    assert_null(qNode[1].previous);
    assert_ptr_equal(qNode[0].queue, &queue);
    assert_ptr_equal(qNode[2].queue, &queue);
    assert_ptr_equal(qNode[0].next, &qNode[2]);
    assert_ptr_equal(qNode[2].previous, &qNode[0]);
    assert_null(qNode[0].previous);
    assert_null(qNode[2].next);
}


void delete_one(void **state) {
    (void) state;
    queue_t queue;
    queue_init(&queue);
    queue_node_t node;
    queue_init_node(&node, NULL);
    queue_put_front(&queue, &node);
    queue_delete_node(&node);
    assert_null(queue.begin);
    assert_null(queue.end);
    assert_null(node.queue);
    assert_null(node.next);
    assert_null(node.previous);
}



void put_front_1000(void **state) {
    (void) state;
    const int N = 1000;
    queue_node_t node[N];
    int numbers[N];
    for (int i = 0; i < N; ++i) {
        numbers[i] = i;
        queue_init_node(&node[i], &numbers[i]);
    }
    queue_t queue;
    queue_init(&queue);
    for (int i = 0; i < N; ++i) {
        queue_put_front(&queue, &node[i]);
    }
    assert_ptr_equal(queue.begin, &node[N - 1]);
    assert_ptr_equal(queue.end, &node[0]);
    assert_null(queue.begin->previous);
    assert_null(queue.end->next);
    for (int i = 0; i < N - 1; ++i) {
        assert_ptr_equal(node[i + 1].next, &node[i]);
        assert_ptr_equal(node[i].previous, &node[i + 1]);
        assert_ptr_equal(node[i].link, &numbers[i]);
        assert_int_equal(*(int *)node[i].link + 1, *(int *)node[i].previous->link);
    }
}


void put_back_1000(void **state) {
    (void) state;
    const int N = 1000;
    queue_node_t node[N];
    int numbers[N];
    for (int i = 0; i < N; ++i) {
        numbers[i] = i;
        queue_init_node(&node[i], &numbers[i]);
    }
    queue_t queue;
    queue_init(&queue);
    for (int i = 0; i < N; ++i) {
        queue_put_back(&queue, &node[i]);
    }
    assert_ptr_equal(queue.begin, &node[0]);
    assert_ptr_equal(queue.end, &node[N - 1]);
    assert_null(node[0].previous);
    assert_null(node[N - 1].next);
    for (int i = 0; i < N - 1; ++i) {
        assert_ptr_equal(node[i].next, &node[i + 1]);
        assert_ptr_equal(node[i + 1].previous, &node[i]);
        assert_ptr_equal(node[i].link, &numbers[i]);
        assert_int_equal(*(int *)node[i].link + 1, *(int *)node[i].next->link);
    }
}


void pop_front_1000(void **state) {
    (void) state;
    const int N = 1000;
    queue_node_t node[N];
    int numbers[N];
    for (int i = 0; i < N; ++i) {
        numbers[i] = i;
        queue_init_node(&node[i], &numbers[i]);
    }
    queue_t queue;
    queue_init(&queue);
    for (int i = 0; i < N; ++i) {
        queue_put_back(&queue, &node[i]);
    }
    int i = 0;
    while (queue.begin != NULL) {
        queue_node_t *former_begin = queue.begin;
        if ((queue.begin->next) != NULL) {
            assert_ptr_equal(former_begin, queue.begin->next->previous);
        }
        int *res = queue_pop_front(&queue);
        assert_ptr_not_equal(former_begin, queue.begin);
        assert_int_equal(*res, i);
        if (queue.begin != NULL && queue.begin->next != NULL) {
            assert_ptr_not_equal(former_begin, queue.begin->next->previous);
        }
        i++;
    }
    assert_ptr_equal(queue.begin, NULL);
    assert_ptr_equal(queue.end, NULL);
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(node_init),
        cmocka_unit_test(queue_one_element),
        cmocka_unit_test(put_front_1000),
        cmocka_unit_test(put_back_1000),
        cmocka_unit_test(del_middle_node),
        cmocka_unit_test(delete_one),
        cmocka_unit_test(pop_front_1000),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
