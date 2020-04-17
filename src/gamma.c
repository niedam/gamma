/** @file
 * Implementacja struktury i metod klasy przechowującej stan gry Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
 */

#include <stdlib.h>
#include "gamma.h"
#include "field.h"
#include "stringology.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


/** Struktura reprezentująca informacje na temat gracza gry Gamma.
 */
typedef struct player {
    uint32_t id; /**< Identyfikator gracza. */
    bool golden_move_done; /**< Informacja o tym czy wykonano już złoty ruch. */
    uint64_t occupied_fields; /**< Liczba pól zajętych przez gracza. */
    uint64_t free_adjoining; /**< Liczba wolnych pól przylegających do pól gracza. */
    uint32_t areas; /**< Liczba obszarów gracza na planszy. */
} player_t ;


/** Struktura reprezentująca instancję gry Gamma, przechowująca związane z nią
 * informacje.
 */
struct gamma {
    uint32_t height; /**< Wysokość planszy. */
    uint32_t width; /**< Szerokość planszy. */
    field_t *fields; /**< Dwuwymiarowa tablica pól. */
    uint32_t no_players; /**< Liczba graczy w rozgrywce. */
    uint32_t areas_limit; /**< Limit obszarów. */
    player_t *players; /**< Tablica graczy. */
    uint64_t ocupied_fields; /**< Liczba zajętych pól na planszy. */
};


/** @brief Sprawdzenie poprawności identyfikatora gracza.
 * @param[in] g             – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player        – identyfikator gracza.
 * @return Wartość @p true, jeżeli @param player jest liczbą dodatnią niewiększą
 * od liczby graczy w grze reprezentowanej przez @p g, w przeciwnym wypadku
 * @p false lub gdy @p g jest `NULL`-em.
 */
static bool test_player(const gamma_t *g, uint32_t player) {
    return !ISNULL(g) && player > 0 && player <= g->no_players;
}


/** @brief Sprawdzenie poprawności współrzędnych pola planszy.
 * @param[in] g             – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x             – numer kolumny,
 * @param[in] y             – numer wiersza.
 * @return Wartość @p true jeżeli współrzędne odpowiadają poprawnemu polu planszy,
 * @p false - w przeciwnym wypadku, lub gdy @p g jest `NULL`-em.
 */
static bool test_field(const gamma_t *g, uint32_t x, uint32_t y) {
    return !ISNULL(g) && x < g->width && y < g->height;
}


/** @brief Dostęp do pola planszy.
 * @param[in] g             – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x             – numer kolumny,
 * @param[in] y             – numer wiersza.
 * @return Wskaźnik do struktury przechowującej informacje dotyczące pola
 * o współrzędnych (@p x, @p y). Jeżeli któryś z argumentów jest
 * niepoprawny wynikiem funkcji jest `NULL`.
 */
static field_t *gamma_get_field(gamma_t *g, uint32_t x, uint32_t y) {
    return !test_field(g, x, y) ? NULL :
                    field_at_board(g->fields, (uint64_t) (g->width) * y + x);
}


/** @brief Wskaźnik do informacji o graczu.
 * @param[in] g             – wskaźnik na strukturę przechowującą stan gry,
 * @param player            – identyfikator gracza.
 * @return Wskaźnik do struktury opisującej gracza o podanym
 * identyfikatorze lub `NULL` jeżeli któryś z parametrów jest nieprawidłowy.
 */
static player_t *gamma_get_player(const gamma_t *g, uint32_t player) {
    return test_player(g, player) ? &g->players[player - 1] : NULL;
}


/** @brief Zajęcie pola przez gracza.
 * W wyniku działania funkcji wskazane pole zostaje zajęte przez gracza o
 * podanym identyfikatorze.
 * @param[in, out] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in, out] player   – wskaźnik do informacji związanych z graczem
 *                            zajmującym pole,
 * @param[in, out] field    – wskaźnik do informacji związanych z zajmowanym
 *                            polem.
 */
static void gamma_take_field(gamma_t *g, player_t *player, field_t *field) {
    if (ISNULL(g) || ISNULL(player) || ISNULL(field) || field_owner(field) != 0) {
        return;
    }
    field_set_owner(field, player->id);
    g->ocupied_fields++;
    player->occupied_fields++;
    player->areas += 1 - field_count_adjoining_areas(field, player->id);
    field_t *adjoining[4];
    field_adjoining(field, adjoining);
    for (uint32_t i = 0; i < field_adjoining_size(field); ++i) {
        if (field_owner(adjoining[i]) != 0) {
            size_t diff = 1;
            for (size_t j = i + 1; j < field_adjoining_size(field); ++j) {
                if (field_owner(adjoining[i]) == field_owner(adjoining[j])) {
                    diff = 0;
                    break;
                }
            }
            player_t *current = gamma_get_player(g, field_owner(adjoining[i]));
            if (ISNULL(current)) {
                return;
            }
            current->free_adjoining -= diff;
        }
    }
    uint32_t count_adj;
    for (size_t i = 0; i < field_adjoining_size(field); ++i) {
        count_adj = field_count_adjoining_fields(adjoining[i], player->id);
        if (field_owner(adjoining[i]) == 0 && count_adj == 1) {
            player->free_adjoining++;
        } else if (field_owner(adjoining[i]) == player->id) {
            field_connect_area(adjoining[i], field);
        }
    }

}


/** @brief Zwolnienie pola zajętego przez gracza.
 * W wyniku funkcji zajęte przez pewnego gracza pole staje się wolne.
 * @param[in, out] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in, out] field    – wskaźnik do informacji związanych ze zwalnianym
 *                            polem.
 */
static void gamma_release_field(gamma_t *g, field_t *field) {
    if (ISNULL(g) || ISNULL(field)) {
        return;
    }
    player_t *owner = gamma_get_player(g, field_owner(field));
    if (ISNULL(owner)) {
        return;
    }
    field_set_owner(field, 0);
    field_split_area(field);
    field_rebuild_areas_around(field, owner->id);
    size_t diff;
    field_t *adjoining[4];
    field_adjoining(field, adjoining);
    for (size_t i = 0; i < field_adjoining_size(field); ++i) {
        if (field_owner(adjoining[i]) != 0) {
            diff = 1;
            for (size_t j = i + 1; j < field_adjoining_size(field); ++j) {
                if (field_owner(adjoining[i]) == field_owner(adjoining[j])) {
                    diff = 0;
                    break;
                }
            }
            player_t *current = gamma_get_player(g, field_owner(adjoining[i]));
            if (ISNULL(current)) {
                return;
            }
            current->free_adjoining += diff;
        }
    }
    uint32_t count_adj;
    for (size_t i = 0; i < field_adjoining_size(field); ++i) {
        count_adj = field_count_adjoining_fields(adjoining[i], owner->id);
        if (field_owner(adjoining[i]) == 0 && count_adj == 0) {
            owner->free_adjoining--;
        }
    }
    owner->areas -= 1 - field_count_adjoining_areas(field, owner->id);
    owner->occupied_fields--;
    g->ocupied_fields--;
}


gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (width == 0 || height == 0 || players == 0 || areas == 0) {
        return NULL;
    }
    /** W ramach alokacji struktury wykonywane są następujące czynności:
     */
    gamma_t *g = malloc(sizeof(struct gamma));
    if (ISNULL(g)) {
        return NULL;
    }
    /** 1. Alokacja i inicjacja informacji o graczach.
     */
    g->players = calloc(sizeof(player_t), players);
    if (ISNULL(g->players)) {
        free(g);
        return NULL;
    }
    for (size_t i = 0; i < players; ++i) {
        g->players[i].id = i + 1;
        g->players[i].areas = 0;
        g->players[i].occupied_fields = 0;
        g->players[i].free_adjoining = 0;
        g->players[i].golden_move_done = false;
    }
    g->no_players = players;
    /** 2. Alokacja i inicjacja pól na planszy.
     */
    g->width = width;
    g->height = height;
    g->fields = field_board_new(width, height);
    if (ISNULL(g->fields)) {
        free(g->players);
        free(g);
        return NULL;
    }
    /** 3. Zainicjowanie przechowywanych wartości pomocniczych.
     */
    g->areas_limit = areas;
    g->ocupied_fields = 0;
    return g;
}


void gamma_delete(gamma_t *g) {
    if (ISNULL(g)) {
        return;
    }
    free(g->players);
    free(g->fields);
    free(g);
}


bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    field_t *field = gamma_get_field(g, x, y);
    player_t *player_info = gamma_get_player(g, player);
    if (ISNULL(g) || ISNULL(field) || ISNULL(player_info) || field_owner(field) != 0) {
        return false;
    }
    uint32_t my_adjoining_areas = field_count_adjoining_areas(field, player);
    if (player_info->areas == g->areas_limit && my_adjoining_areas == 0) {
        return false;
    }
    gamma_take_field(g, player_info, field);
    return true;
}


bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (ISNULL(g) || !test_player(g, player) || !test_field(g, x, y)) {
        return false;
    }
    player_t *player_link = gamma_get_player(g, player);
    field_t *field = gamma_get_field(g, x, y);
    if (player_link->golden_move_done) {
        /* Złoty ruch został już wykonany przez tego gracza.
         */
        return false;
    }
    if (field_owner(field) == player || field_owner(field) == 0) {
        return false;
    }
    if (player_link->areas == g->areas_limit
        && field_count_adjoining_areas(field, player) == 0) {
        /* Gracz osiągnął limit obszarów i nie powiększy żadnego istniejącego.
        */
        return false;

    }
    player_t *owner = gamma_get_player(g, field_owner(field));
    size_t areas_after_breaking = field_count_adjoining_areas_after_breaking(field);
    if (g->areas_limit - owner->areas < 4 && g->areas_limit <
                                    areas_after_breaking - 1 + owner->areas) {
        /* Zdjęcie pionka innemu graczu stworzyłoby mu obszary ponad limit.
         */
        return false;
    }
    gamma_release_field(g, field);
    gamma_take_field(g, player_link, field);
    player_link->golden_move_done = true;
    return true;
}


uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (ISNULL(g) || !test_player(g, player)) {
        return 0;
    }
    return gamma_get_player(g, player)->occupied_fields;
}


uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    player_t *player_info = gamma_get_player(g, player);
    if (player_info->areas == g->areas_limit) {
        return player_info->free_adjoining;
    } else {
        return (uint64_t) g->width * g->height - g->ocupied_fields;
    }
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    player_t *p_info = gamma_get_player(g, player);
    if (ISNULL(g) || ISNULL(p_info)) {
        return false;
    }
    return !p_info->golden_move_done
            && g->ocupied_fields - p_info->occupied_fields > 0;
}


char* gamma_board(gamma_t *g) {
    if (ISNULL(g)) {
        return NULL;
    }
    size_t size = 0;
    for (uint32_t i = 0; i < g->height; ++i) {
        for (uint32_t j = 0; j < g->width; ++j) {
            field_t *f = gamma_get_field(g, j, i);
            size_t len = uint32_length(field_owner(f));
            size += len == 1 ? 1 : len + 2;
        }
        size++;
    }
    size++;
    char *result = calloc(sizeof(char), size);
    if (ISNULL(result)) {
        return NULL;
    }
    char *buff = result;
    for (uint32_t i = g->height; i > 0; --i) {
        for (uint32_t j = 0; j < g->width; ++j) {
            field_t *f = gamma_get_field(g, j, i - 1);
            int k = player_print(buff, size, field_owner(f));
            buff += k;
        }
        buff[0] = '\n';
        buff++;
    }
    result[size - 1] = '\0';
    return result;
}
