/** @file
 * Implementacja struktury pola na planszy gry Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
 */

#include <stdlib.h>
#include "field.h"
#include "utilities/queue.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)

/** Struktura pola planszy w grze Gamma.
 * Aby korzystać z pola należy go odpowiednio zainicjować.
 * Tablica zainicjowanych pól jest tworzona w funkcji @ref field_board_new()
 * i jest to jedyny zalecany sposób uzyskiwania struktury pól.
 * Korzystanie z niezainicjowanych pól może mieć nieokreślone skutki.
 */
typedef struct field {
    struct field *adjoining[4]; /**< Tablica wszystkich sąsiadujących pól. */
    uint32_t size_adjoining; /**< Ilość sąsiadujących pól. */
    queue_node_t bfs; /**< Węzeł potrzebny do zorganizowania kolejki do algorytmu BFS. */
    int i;
    struct area {
        struct area *next;
        struct area *prev;
        struct area *repr;
        uint64_t size;
    } area; /**< Obszar do którego należy pole. */
    uint32_t owner; /**< Identyfikator właściciela pola. */
    bool visited; /**< Informacja o tym czy odwiedzono pole algorytmem BFS. */
} field_t;


static uint64_t area_size(struct area *area) {
    return !ISNULL(area) ? area->repr->size : 0;
}

static bool area_test(struct area *area1, struct area *area2) {
    return !ISNULL(area1) && !ISNULL(area2) ? area1->repr == area2->repr : false;
}


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
    f->area = (struct area) { .size = 1, .next = &f->area, .prev = &f->area, .repr = &f->area };
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

uint32_t field_owner(field_t *field) {
    if (ISNULL(field)) {
        return 0;
    }
    return field->owner;
}


void field_set_owner(field_t *field, uint32_t new_owner) {
    if (!ISNULL(field) && new_owner == 0) {
        field->owner = new_owner;
    }
}


uint64_t field_area_size(field_t *field) {
    if (ISNULL(field)) {
        return 0;
    }
    return field->area.repr->size;
}


bool field_test_area(field_t *field1, field_t *field2) {
    return !ISNULL(field1) && !ISNULL(field2) ? area_test(&field1->area, &field2->area) : false;
}


struct adjoining field_adjoining(field_t *field) {
    if (ISNULL(field)) {
        return (struct adjoining){ .adjoining = NULL, .size = 0 };
    } else {
        return (struct adjoining){ .adjoining = field->adjoining, .size = field->size_adjoining };
    }
}

void field_connect_areas(field_t *field1, field_t *field2) {
    if (ISNULL(field1) || ISNULL(field2)) {
        return;
    }
    struct area *area1 = &field1->area;
    struct area *area2 = &field2->area;
    if (area_test(area1, area2)) {
        return;
    }
    if (area_size(area1) <= area_size(area2)) {
        struct area *temp = area1;
        area1 = area2;
        area2 = temp;
    }
    area1->repr->size += area_size(area2);
    area2->repr->size = 0;
    struct area *curr = area2;
    do {
        curr->repr = area1->repr;
        curr = curr->next;
    } while (curr != area2);
    struct area *next1 = area1->next;
    struct area *next2 = area2->next;
    area1->next = area2;
    area2->prev = area1;
    next1->prev = next2;
    next2->next = next1;
}


void field_split_area(field_t *field) {
    if (ISNULL(field)) {
        return;
    }
    struct area *area = &field->area;
    if (area->size == 1) {
        return;
    }
    struct area *temp_prev, *current = area;
    do {
        temp_prev = current->prev;
        current->next = current;
        current->prev = current;
        current->repr = current;
        current->size = 1;
        current = temp_prev;
    } while (current != area);
}


field_t *field_at_array(field_t **array, uint32_t x, uint32_t y) {
    if (ISNULL(array)) {
        return NULL;
    }
    return &array[y][x];
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
                        field_test_area(field->adjoining[i], field->adjoining[j])) {
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
            field_connect_areas(curr, field->adjoining[i]);
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
