/** @file
 * Implementacja struktury pola na planszy gry Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
 */

#include <stdlib.h>
#include "field.h"
#include "utilities/uset.h"
#include "utilities/queue.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


/** @brief Inicjacja pola.
 * @param[out] fields       – wskaźnik na pole do zainicjowania,
 * @param[in] x             – numer kolumny pola,
 * @param[in] y             – numer wiersza pola,
 * @param[in] max_x         – liczba kolumn na planszy,
 * @param[in] max_y         – liczba wierszy na planszy
 */
static void field_init(field_t **fields, uint32_t x, uint32_t y,
                       uint32_t max_x, uint32_t max_y) {
    if (ISNULL(fields) || x == 0 || y == 0 || x > max_x || y > max_y) {
        return;
    }
    field_t *f = &fields[y - 1][x - 1];
    f->visited = false;
    f->owner = 0;
    uset_init(&f->area);
    queue_init_node(&f->bfs, f);
    for (size_t i = 0; i < 4; ++i) {
        f->adjoining[i] = NULL;
    }
    size_t last = 0;
    int v_x[4] = {0, -1, 0, 1};
    int v_y[4] = {1, 0, -1, 0};
    for (size_t i = 0; i < 4; ++i) {
        if (x + v_x[i] > 0 && x + v_x[i] <= max_x
            && y + v_y[i] > 0 && y + v_y[i] <= max_y) {
            f->adjoining[last] = &fields[y + v_y[i] - 1][x + v_x[i] - 1];
            ++last;
        }
    }
    f->size_adjoining = last;
}


field_t **field_board_new(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) {
        return NULL;
    }
    field_t **result = calloc(sizeof(struct field *), height);
    if (ISNULL(result)) {
        return NULL;
    }
    for (uint32_t i = 0; i < height; ++i) {
        result[i] = calloc(sizeof(field_t), width);
        if (ISNULL(result[i])) {
            for (uint32_t j = 0; j < i; ++j) {
                free(result[j]);
            }
            free(result);
            return false;
        }
    }
    for (uint32_t i = 1; i <= height; ++i) {
        for (uint32_t j = 1; j <= width; ++j) {
            field_init(result, j, i, width, height);
        }
    }
    return result;
}


uint32_t field_count_adjoining_areas(const field_t *field, uint32_t player_id) {
    uint32_t result = 0;
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner == player_id) {
            int add = 1;
            for (size_t j = i + 1; j < field->size_adjoining; ++j) {
                if (field->adjoining[j]->owner == player_id &&
                        uset_test(&field->adjoining[i]->area, &field->adjoining[j]->area)) {
                    add = 0;
                    break;
                }
            }
            result += add;
        }
    }
    return result;
}


uint32_t field_count_adjoining_areas_after_breaking(field_t *field) {
    uint32_t player = field->owner;
    uint32_t result = 0;
    field->visited = true;
    queue_t queue;
    queue_t reset;
    queue_init(&queue);
    queue_init(&reset);
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner != player
                || field->adjoining[i]->visited) {
            continue;
        }
        field->adjoining[i]->visited = true;
        queue_put_back(&queue, &field->adjoining[i]->bfs);
        while (!queue_empty(&queue)) {
            field_t *curr = queue_pop_front(&queue);
            queue_put_front(&reset, &curr->bfs);
            for (size_t j = 0; j < curr->size_adjoining; ++j) {
                if (curr->adjoining[j]->visited ||
                    curr->adjoining[j]->owner != player) {
                    continue;
                }
                curr->adjoining[j]->visited = true;
                queue_put_back(&queue, &curr->adjoining[j]->bfs);
            }
        }
        result++;
    }
    while (!queue_empty(&reset)) {
        field_t *curr = queue_pop_front(&reset);
        curr->visited = false;
    }
    return result;
}


uint32_t field_count_adjoining_fields(const field_t *field, uint32_t player_id) {
    if (ISNULL(field) || player_id == 0) {
        return 0;
    }
    uint32_t result = 0;
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner == player_id) {
            result++;
        }
    }
    return result;
}


void field_rebuild_areas_around(field_t *field, uint32_t player_id) {
    queue_t queue;
    queue_t reset;
    queue_init(&queue);
    queue_init(&reset);
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner != player_id ||
            field->adjoining[i]->visited) {
            continue;
        }
        field->adjoining[i]->visited = true;
        queue_put_back(&queue, &field->adjoining[i]->bfs);
        while (!queue_empty(&queue)) {
            field_t *curr = queue_pop_front(&queue);
            queue_put_front(&reset, &curr->bfs);
            uset_union(&curr->area, &field->adjoining[i]->area);
            for (size_t j = 0; j < curr->size_adjoining; ++j) {
                if (curr->adjoining[j]->visited ||
                    curr->adjoining[j]->owner != player_id) {
                    continue;
                }
                curr->adjoining[j]->visited = true;
                queue_put_back(&queue, &curr->adjoining[j]->bfs);
            }
        }
    }
    while (!queue_empty(&reset)) {
        field_t *curr = queue_pop_front(&reset);
        curr->visited = false;
    }
}
