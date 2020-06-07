/** @file
 * Implementacja struktury pola na planszy gry Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 12.06.2020
 */

#include <stdlib.h>
#include "field.h"
#include "isnull.h"


/** Struktura przechowująca informacje o polu planszy.
 */
typedef struct field {
    struct field *adjoining[ADJOINING_FIELDS]; /**< Tablica wszystkich
                                                * sąsiadujących pól. */
    uint32_t size_adjoining; /**< Ilość sąsiadujących pól. */
    field_t *next_node; /**< Pomocnicze pole do zorganizowania pól w kolejkę. */
    /** Struktura reprezentująca obszar pól.
     * Obszar zaimplementowany jest jako struktura zbiorów rozłącznych w postaci
     * kolejki cyklicznej.
     */
    struct area {
        struct area *next; /**< Następne pole należące do obszaru. */
        struct area *prev; /**< Poprzednie pole należące do obszaru. */
        struct area *repr; /**< Wskaźnik na reprezentanta obszaru. */
        uint64_t size; /**< Rozmiar obszaru (jeżeli pole jest reprezentantem).*/
    } area; /**< Obszar do którego należy pole. */
    uint32_t owner; /**< Identyfikator właściciela pola. */
    bool visited; /**< Informacja o tym czy odwiedzono pole algorytmem BFS. */
} field_t;


field_t *field_at_board(field_t *b, uint64_t field_id) {
    return !ISNULL(b) ? &b[field_id] : NULL;
}


/** @brief Inicjacja pola.
 * @param[out] fields       – wskaźnik na tablicę pól do zainicjowania,
 *                            która ma reprezentować planszę,
 * @param[in] x             – numer kolumny pola do zainicjowania,
 *                            liczba nieujemna mniejsza od @p max_x,
 * @param[in] y             – numer wiersza pola do zainicjowania,
 *                            liczba nieujemna mniejsza od @p max_y,
 * @param[in] max_x         – liczba kolumn na planszy,
 * @param[in] max_y         – liczba wierszy na planszy
 */
static void field_init(field_t *fields, uint32_t x, uint32_t y,
                       uint32_t max_x, uint32_t max_y) {
    if (ISNULL(fields) || x >= max_x || y >= max_y) {
        return;
    }
    field_t *f = &fields[(uint64_t) max_x * y + x];
    f->visited = false;
    f->owner = 0;
    f->area = (struct area){ .prev = &f->area, .next = &f->area,
                              .repr = &f->area, .size = 1};
    for (uint32_t i = 0; i < ADJOINING_FIELDS; ++i) {
        f->adjoining[i] = NULL;
    }
    uint32_t last = 0;
    int v_x[ADJOINING_FIELDS] = {0, -1, 0, 1};
    int v_y[ADJOINING_FIELDS] = {1, 0, -1, 0};
    for (uint32_t i = 0; i < ADJOINING_FIELDS; ++i) {
        if (x + v_x[i] + 1 > 0 && x + v_x[i] + 1 <= max_x
                && y + v_y[i] + 1 > 0 && y + v_y[i] + 1 <= max_y) {
            f->adjoining[last] =
                    &fields[(y + v_y[i]) * (uint64_t) max_x + x + v_x[i]];
            ++last;
        }
    }
    f->size_adjoining = last;
}


uint32_t field_owner(const field_t *field) {
    if (ISNULL(field)) {
        return 0;
    } else {
        return field->owner;
    }
}


void field_set_owner(field_t *field, uint32_t new_owner) {
    if (ISNULL(field)) {
        return;
    }
    field->owner = new_owner;
}


void field_adjoining(field_t *field, field_t *adjoining[ADJOINING_FIELDS]) {
    if (ISNULL(field) || ISNULL(adjoining)) {
        return;
    }
    for (uint32_t i = 0; i < field->size_adjoining; ++i) {
        adjoining[i] = field->adjoining[i];
    }
    for (uint32_t i = field->size_adjoining; i < ADJOINING_FIELDS; ++i) {
        adjoining[i] = NULL;
    }
}


uint32_t field_adjoining_size(const field_t *field) {
    if (ISNULL(field)) {
        return 0;
    }
    return field->size_adjoining;
}


field_t *field_board_new(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) {
        return NULL;
    }
    size_t n_fields = (size_t) width * height;
    field_t *result = calloc(n_fields, sizeof(field_t));
    if (ISNULL(result)) {
        return NULL;
    }
    for (uint32_t i = 0; i < height; ++i) {
        for (uint32_t j = 0; j < width; ++j) {
            field_init(result, j, i, width, height);
        }
    }
    return result;
}


uint32_t field_count_adjoining_areas(const field_t *field, uint32_t player_id) {
    uint32_t result = 0;
    for (uint32_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner == player_id) {
            int add = 1;
            for (uint32_t j = i + 1; j < field->size_adjoining; ++j) {
                if (field->adjoining[j]->owner == player_id &&
                        field->adjoining[i]->area.repr == field->adjoining[j]->area.repr) {
                    add = 0;
                    break;
                }
            }
            result += add;
        }
    }
    return result;
}


void field_connect_area(field_t *field1, field_t *field2) {
    if (ISNULL(field1) || ISNULL(field2)) {
        return;
    }
    struct area *area1 = &field1->area, *area2 = &field2->area;
    if (area1->repr == area2->repr) {
        return;
    }
    if (area1->repr->size < area2->repr->size) {
        struct area *tmp = area1;
        area1 = area2;
        area2 = tmp;
    }
    area1->repr->size += area2->repr->size;
    area2->repr->size = 0;
    struct area *curr = area2;
    do {
        curr->repr = area1->repr;
        curr = curr->next;
    } while (curr != area2);
    struct area *next1 = area1->next;
    struct area *prev2 = area2->prev;
    area1->next = area2;
    area2->prev = area1;
    next1->prev = prev2;
    prev2->next = next1;
}


void field_split_area(field_t *field) {
    if (ISNULL(field)) {
        return;
    }
    struct area *area = &field->area;
    if (area->repr == area && area->size == 1) {
        return;
    }
    struct area *temp_next, *current = area;
    do {
        temp_next = current->next;
        current->next = current;
        current->prev = current;
        current->repr = current;
        current->size = 1;
        current = temp_next;
    } while (current != area);
}


uint32_t field_count_adjoining_areas_after_breaking(field_t *field) {
    uint32_t player = field->owner;
    uint32_t result = 0;
    field->visited = true;
    field_t *queue = NULL;
    field_t *reset = NULL;
    for (uint32_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner != player
                || field->adjoining[i]->visited) {
            continue;
        }
        field->adjoining[i]->visited = true;
        field->adjoining[i]->next_node = queue;
        queue = field->adjoining[i];
        while (!ISNULL(queue)) {
            field_t *curr = queue;
            queue = queue->next_node;
            curr->next_node = reset;
            reset = curr;
            for (uint32_t j = 0; j < curr->size_adjoining; ++j) {
                if (curr->adjoining[j]->visited ||
                    curr->adjoining[j]->owner != player) {
                    continue;
                }
                curr->adjoining[j]->visited = true;
                curr->adjoining[j]->next_node = queue;
                queue = curr->adjoining[j];
            }
        }
        result++;
    }
    while (!ISNULL(reset)) {
        field_t *curr = reset;
        reset = reset->next_node;
        curr->visited = false;
        curr->next_node = NULL;
    }
    field->visited = false;
    return result;
}


uint32_t field_count_adjoining_fields(const field_t *field, uint32_t player_id) {
    if (ISNULL(field) || player_id == 0) {
        return 0;
    }
    uint32_t result = 0;
    for (uint32_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner == player_id) {
            result++;
        }
    }
    return result;
}


void field_rebuild_areas_around(field_t *field, uint32_t player_id) {
    field_t *queue = NULL;
    field_t *reset = NULL;
    for (uint32_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner != player_id ||
            field->adjoining[i]->visited) {
            continue;
        }
        field->adjoining[i]->visited = true;
        field->adjoining[i]->next_node = queue;
        queue = field->adjoining[i];
        while (!ISNULL(queue)) {
            field_t *curr = queue;
            queue = queue->next_node;
            curr->next_node = reset;
            reset = curr;
            field_connect_area(curr, field->adjoining[i]);
            for (uint32_t j = 0; j < curr->size_adjoining; ++j) {
                if (curr->adjoining[j]->visited ||
                    curr->adjoining[j]->owner != player_id) {
                    continue;
                }
                curr->adjoining[j]->visited = true;
                curr->adjoining[j]->next_node = queue;
                queue = curr->adjoining[j];
            }
        }
    }
    while (!ISNULL(reset)) {
        field_t *curr = reset;
        reset = reset->next_node;
        curr->next_node = NULL;
        curr->visited = false;
    }
}
