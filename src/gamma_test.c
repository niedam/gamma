/** @file
 * Przykładowe użycie silnika gry gamma
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 18.03.2020
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* Ten plik włączamy na początku i dwa razy, aby sprawdzić, czy zawiera
 * wszystko, co jest potrzebne. */
#include "gamma.h"


/* CMake w wersji release wyłącza asercje. */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/** FUNKCJE POMOCNE PRZY DEBUGOWANIU TESTÓW **/

#if 0

#include <stdio.h>
#include <inttypes.h>

static inline void print_board(gamma_t *g) {
  char *board = gamma_board(g);
  assert(board);
  printf(board);
  free(board);
}

static inline void print_players(gamma_t *g,
                                 uint32_t from_player, uint32_t to_player) {
  assert(from_player >= 1);
  for (uint32_t player = from_player - 1; player++ < to_player;)
    printf("player = %" PRIu32 ", busy = %" PRIu64 ", free = %" PRIu64
           ", golden = %d\n",
           player,
           gamma_busy_fields(g, player),
           gamma_free_fields(g, player),
           gamma_golden_possible(g, player));
}

#endif

/** KONFIGUARACJA TESTÓW **/

/* Nie było wymagania, aby ustawiać errno, więc w wersji docelowej nie
 * będziemy tego sprawdzać. */
static const bool test_errno = false;

/* Możliwe wyniki testu */
#define PASS 0
#define FAIL 1
#define WRONG_TEST 2
#define MEM_PASS 13

/* Liczba elementów tablicy x */
#define SIZE(x) (sizeof(x) / sizeof(x)[0])

/* Różne rozmiary planszy */
#define SMALL_BOARD_SIZE    10
#define MIDDLE_BOARD_SIZE  100
#define BIG_BOARD_SIZE    1000

#define MANY_GAMES 42

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t players;
    uint32_t areas;
} gamma_param_t;

/** WŁAŚCIWE TESTY **/

/* Testuje opublikowany przykład użycia. */
static void example(void **state) {
    (void) state;
    static const char board[] =
            "1.........\n"
            "..........\n"
            "..........\n"
            "......2...\n"
            ".....2....\n"
            "..........\n"
            "..........\n"
            "1.........\n"
            "1221......\n"
            "1.........\n";

    gamma_t *g;

    g = gamma_new(0, 0, 0, 0);
    assert(g == NULL);
    assert_null(g);

    g = gamma_new(10, 10, 2, 3);
    assert(g != NULL);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 0, 0));
    assert_true(gamma_busy_fields(g, 1) == 1);
    assert_true(gamma_busy_fields(g, 2) == 0);
    assert_true(gamma_free_fields(g, 1) == 99);
    assert_true(gamma_free_fields(g, 2) == 99);
    assert_true(!gamma_golden_possible(g, 1));
    assert_true(gamma_move(g, 2, 3, 1));
    assert_true(gamma_busy_fields(g, 1) == 1);
    assert_true(gamma_busy_fields(g, 2) == 1);
    assert_true(gamma_free_fields(g, 1) == 98);
    assert_true(gamma_free_fields(g, 2) == 98);
    assert_true(gamma_move(g, 1, 0, 2));
    assert_true(gamma_move(g, 1, 0, 9));
    assert_true(!gamma_move(g, 1, 5, 5));
    assert_true(gamma_free_fields(g, 1) == 6);
    assert_true(gamma_move(g, 1, 0, 1));
    assert_true(gamma_free_fields(g, 1) == 95);
    assert_true(gamma_move(g, 1, 5, 5));
    assert_true(!gamma_move(g, 1, 6, 6));
    assert_true(gamma_busy_fields(g, 1) == 5);
    assert_true(gamma_free_fields(g, 1) == 10);
    assert_true(gamma_move(g, 2, 2, 1));
    assert_true(gamma_move(g, 2, 1, 1));
    assert_true(gamma_free_fields(g, 1) == 9);
    assert_true(gamma_free_fields(g, 2) == 92);
    assert_true(!gamma_move(g, 2, 0, 1));
    assert_true(gamma_golden_possible(g, 2));
    assert_true(!gamma_golden_move(g, 2, 0, 1));
    assert_true(gamma_golden_move(g, 2, 5, 5));
    assert_true(!gamma_golden_possible(g, 2));
    assert_true(gamma_move(g, 2, 6, 6));
    assert_true(gamma_busy_fields(g, 1) == 4);
    assert_true(gamma_free_fields(g, 1) == 91);
    assert_true(gamma_busy_fields(g, 2) == 5);
    assert_true(gamma_free_fields(g, 2) == 13);
    assert_true(gamma_golden_move(g, 1, 3, 1));
    assert_true(gamma_busy_fields(g, 1) == 5);
    assert_true(gamma_free_fields(g, 1) == 8);
    assert_true(gamma_busy_fields(g, 2) == 4);
    assert_true(gamma_free_fields(g, 2) == 10);

    char *p = gamma_board(g);
    assert_non_null(p);
    assert_true(strcmp(p, board) == 0);
    free(p);

    gamma_delete(g);
}

/* Testuje najmniejszą możliwą grę. */
static void minimal(void **state) {
    (void) state;
    gamma_t *g = gamma_new(1, 1, 1, 1);
    assert_non_null(g);
    assert_true(gamma_move(g, 1, 0, 0));
    assert_true(gamma_busy_fields(g, 1) == 1);
    assert_true(gamma_free_fields(g, 1) == 0);
    gamma_delete(g);
}

/* Testuje sprawdzanie poprawności parametrów poszczególnych funkcji. */
static void params(void **state) {
    (void) state;
    assert_null(gamma_new(0, 10, 2, 2));
    assert_null(gamma_new(10, 0, 2, 2));
    assert_null(gamma_new(10, 10, 0, 2));
    assert_null(gamma_new(10, 10, 2, 0));

    gamma_t *g = gamma_new(10, 10, 2, 2);

    assert_false(gamma_move(NULL, 1, 5, 5));
    assert_false(gamma_move(g, 0, 5, 5));
    assert_false(gamma_move(g, 3, 5, 5));
    assert_false(gamma_move(g, UINT32_MAX, 5, 5));
    assert_false(gamma_move(g, 1, 10, 5));
    assert_false(gamma_move(g, 1, UINT32_MAX, 5));
    assert_false(gamma_move(g, 1, 5, 10));
    assert_false(gamma_move(g, 1, 5, UINT32_MAX));

    assert_false(gamma_golden_move(NULL, 1, 5, 5));
    assert_false(gamma_golden_move(g, 0, 5, 5));
    assert_false(gamma_golden_move(g, 3, 5, 5));
    assert_false(gamma_golden_move(g, UINT32_MAX, 5, 5));
    assert_false(gamma_golden_move(g, 1, 10, 5));
    assert_false(gamma_golden_move(g, 1, UINT32_MAX, 5));
    assert_false(gamma_golden_move(g, 1, 5, 10));
    assert_false(gamma_golden_move(g, 1, 5, UINT32_MAX));

    assert_true(gamma_busy_fields(NULL, 1) == 0);
    assert_true(gamma_busy_fields(g, 0) == 0);
    assert_true(gamma_busy_fields(g, 3) == 0);
    assert_true(gamma_busy_fields(g, UINT32_MAX) == 0);

    assert_true(gamma_free_fields(NULL, 1) == 0);
    assert_true(gamma_free_fields(g, 0) == 0);
    assert_true(gamma_free_fields(g, 3) == 0);
    assert_true(gamma_free_fields(g, UINT32_MAX) == 0);

    assert_true(gamma_golden_possible(NULL, 1) == 0);
    assert_true(gamma_golden_possible(g, 0) == 0);
    assert_true(gamma_golden_possible(g, 3) == 0);
    assert_true(gamma_golden_possible(g, UINT32_MAX) == 0);

    assert_true(gamma_board(NULL) == NULL);

    gamma_delete(g);
}

/* Testuje, czy nie ma problemów przy wypisywaniu planszy w grze
 * z dużą liczbą graczy. */
static void many_players(void **state) {
    (void) state;
    gamma_t *g = gamma_new(MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE,
                           MIDDLE_BOARD_SIZE * MIDDLE_BOARD_SIZE, 1);
    assert_non_null(g);

    for (uint32_t x = 0; x < MIDDLE_BOARD_SIZE; ++x)
        for (uint32_t y = 0; y < MIDDLE_BOARD_SIZE; ++y)
            assert_true(gamma_move(g, x * MIDDLE_BOARD_SIZE + y + 1, x, y));

    char *board = gamma_board(g);
    assert_non_null(board);
    free(board);

    gamma_delete(g);
}

/* Testuje, czy można rozgrywać równocześnie więcej niż jedną grę. */
static void many_games(void **state) {
    (void) state;
    static const gamma_param_t game[] = {
            {7, 9, 2, 4},
            {11, 21, 2, 4},
            {47, 3, 2, 4},
            {2, 99, 2, 2},
    };
    static const uint64_t free1[SIZE(game)] = {8, 8, 6, 4};

    static gamma_t *g[MANY_GAMES][SIZE(game)];

    for (size_t i = 0; i < MANY_GAMES; ++i) {
        for (size_t j = 0; j < SIZE(game); ++j) {
            g[i][j] = gamma_new(game[j].width, game[j].height,
                                game[j].players, game[j].areas);
            assert_non_null(g[i][j]);
        }
    }

    for (size_t i = 0; i < MANY_GAMES; ++i) {
        for (size_t j = 0; j < SIZE(game); ++j) {
            assert_true(gamma_move(g[i][j], 1, 0, 0));
            assert_true(gamma_move(g[i][j], 1, 0, game[j].height - 1));
            assert_true(gamma_move(g[i][j], 1, game[j].width - 1, 0));
            assert_true(gamma_move(g[i][j], 1, game[j].width - 1, game[j].height - 1));
            assert_false(gamma_move(g[i][j], 1, game[j].width, game[j].height - 1));
            assert_false(gamma_move(g[i][j], 1, game[j].width - 1, game[j].height));
        }
    }

    for (size_t i = 0; i < MANY_GAMES; ++i) {
        for (size_t j = 0; j < SIZE(game); ++j) {
            assert_true(gamma_busy_fields(g[i][j], 1) == 4);
            assert_true(gamma_free_fields(g[i][j], 1) == free1[j]);
            assert_true(gamma_busy_fields(g[i][j], 2) == 0);
            uint64_t size = (uint64_t)game[j].width * (uint64_t)game[j].height;
            assert_true(gamma_free_fields(g[i][j], 2) == size - 4);
        }
    }

    for (size_t i = 0; i < MANY_GAMES; ++i)
        for (size_t j = 0; j < SIZE(game); ++j)
    gamma_delete(g[i][j]);
}

/* Testuje, czy gamma_delete wywołane z parametrem NULL nic nie robi. */
static void delete_null(void **state) {
    (void) state;
    gamma_delete(NULL);
}

/* Uruchamia kilka krótkich testów poprawności wykonywania zwykłych ruchów oraz
 * obliczania liczby zajętych i wolnych pól po wykonaniu zwykłego ruchu. */
static void normal_move(void **state) {
    (void) state;
    gamma_t *g = gamma_new(2, 2, 2, 2);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 0, 0));
    assert_true(gamma_move(g, 2, 1, 0));
    assert_true(gamma_move(g, 1, 1, 1));
    assert_true(gamma_move(g, 2, 0, 1));

    assert_true(gamma_busy_fields(g, 1) == 2);
    assert_true(gamma_free_fields(g, 1) == 0);
    assert_true(gamma_busy_fields(g, 2) == 2);
    assert_true(gamma_free_fields(g, 2) == 0);

    gamma_delete(g);
    g = gamma_new(5, 5, 2, 4);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 2, 1));
    assert_true(gamma_move(g, 1, 2, 3));
    assert_true(gamma_move(g, 1, 1, 2));
    assert_true(gamma_move(g, 1, 3, 2));
    assert_true(gamma_move(g, 2, 2, 2));

    assert_true(gamma_busy_fields(g, 1) == 4);
    assert_true(gamma_free_fields(g, 1) == 8);
    assert_true(gamma_busy_fields(g, 2) == 1);
    assert_true(gamma_free_fields(g, 2) == 20);

    assert_true(gamma_move(g, 2, 1, 1));
    assert_true(gamma_move(g, 2, 3, 3));
    assert_true(gamma_move(g, 2, 1, 3));

    assert_true(gamma_busy_fields(g, 1) == 4);
    assert_true(gamma_free_fields(g, 1) == 5);
    assert_true(gamma_busy_fields(g, 2) == 4);
    assert_true(gamma_free_fields(g, 2) == 6);

    gamma_delete(g);
    g = gamma_new(5, 5, 5, 1);
    assert_non_null(g);

    assert_true(gamma_move(g, 2, 1, 2));
    assert_true(gamma_move(g, 4, 2, 1));
    assert_true(gamma_move(g, 5, 3, 2));
    assert_true(gamma_move(g, 3, 2, 2));
    assert_true(gamma_move(g, 1, 2, 3));

    assert_true(gamma_busy_fields(g, 1) == 1);
    assert_true(gamma_free_fields(g, 1) == 3);
    assert_true(gamma_busy_fields(g, 2) == 1);
    assert_true(gamma_free_fields(g, 2) == 3);
    assert_true(gamma_busy_fields(g, 3) == 1);
    assert_true(gamma_free_fields(g, 3) == 0);
    assert_true(gamma_busy_fields(g, 4) == 1);
    assert_true(gamma_free_fields(g, 4) == 3);
    assert_true(gamma_busy_fields(g, 5) == 1);
    assert_true(gamma_free_fields(g, 5) == 3);

    gamma_delete(g);
}

/* Uruchamia kilka krótkich testów poprawności wykonywania zwykłych ruchów
 * i złotych ruchów oraz obliczania liczby zajętych i wolnych pól po wykonaniu
 * tych ruchów. */
static void golden_move(void **state) {
    (void) state;
    gamma_t *g = gamma_new(10, 10, 3, 2);
    assert_non_null(g != NULL);

    assert_true(gamma_move(g, 1, 4, 5));
    assert_true(gamma_move(g, 2, 5, 5));
    assert_true(gamma_move(g, 3, 4, 6));
    assert_true(gamma_move(g, 1, 5, 6));
    assert_true(gamma_move(g, 2, 3, 5));
    assert_true(gamma_move(g, 3, 4, 4));
    assert_true(gamma_golden_move(g, 1, 3, 5));

    assert_true(gamma_busy_fields(g, 1) == 3);
    assert_true(gamma_free_fields(g, 1) == 5);
    assert_true(gamma_busy_fields(g, 2) == 1);
    assert_true(gamma_free_fields(g, 2) == 94);
    assert_true(gamma_busy_fields(g, 3) == 2);
    assert_true(gamma_free_fields(g, 3) == 5);

    gamma_delete(g);
    g = gamma_new(7, 5, 3, 2);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 2, 2));
    assert_true(gamma_move(g, 2, 4, 2));
    assert_true(gamma_move(g, 3, 3, 2));
    assert_true(gamma_move(g, 1, 6, 4));
    assert_true(gamma_move(g, 2, 0, 0));
    assert_true(gamma_move(g, 1, 1, 2));
    assert_true(gamma_golden_move(g, 1, 3, 2));

    assert_true(gamma_busy_fields(g, 1) == 4);
    assert_true(gamma_free_fields(g, 1) == 9);
    assert_true(gamma_busy_fields(g, 2) == 2);
    assert_true(gamma_free_fields(g, 2) == 5);
    assert_true(gamma_busy_fields(g, 3) == 0);
    assert_true(gamma_free_fields(g, 3) == 29);

    gamma_delete(g);
    g = gamma_new(6, 3, 2, 2);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 0, 1));
    assert_true(gamma_move(g, 1, 2, 1));
    assert_true(gamma_move(g, 2, 3, 0));
    assert_true(gamma_move(g, 2, 3, 1));
    assert_true(gamma_move(g, 2, 3, 2));
    assert_true(gamma_golden_move(g, 1, 3, 1));

    assert_true(gamma_busy_fields(g, 1) == 3);
    assert_true(gamma_free_fields(g, 1) == 6);
    assert_true(gamma_busy_fields(g, 2) == 2);
    assert_true(gamma_free_fields(g, 2) == 4);

    gamma_delete(g);
    g = gamma_new(6, 3, 2, 2);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 0, 1));
    assert_true(gamma_move(g, 1, 2, 1));
    assert_true(gamma_move(g, 2, 3, 0));
    assert_true(gamma_move(g, 2, 3, 1));
    assert_true(gamma_move(g, 2, 3, 2));
    assert_true(gamma_move(g, 2, 5, 0));
    assert_false(gamma_golden_move(g, 1, 3, 1));

    assert_true(gamma_busy_fields(g, 1) == 2);
    assert_true(gamma_free_fields(g, 1) == 5);
    assert_true(gamma_busy_fields(g, 2) == 4);
    assert_true(gamma_free_fields(g, 2) == 6);

    gamma_delete(g);
    g = gamma_new(6, 3, 2, 2);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 0, 0));
    assert_true(gamma_move(g, 1, 0, 2));
    assert_true(gamma_move(g, 2, 3, 0));
    assert_true(gamma_move(g, 2, 3, 1));
    assert_true(gamma_move(g, 2, 3, 2));
    assert_false(gamma_golden_move(g, 1, 3, 1));

    assert_true(gamma_busy_fields(g, 1) == 2);
    assert_true(gamma_free_fields(g, 1) == 3);
    assert_true(gamma_busy_fields(g, 2) == 3);
    assert_true(gamma_free_fields(g, 2) == 13);

    gamma_delete(g);
}

/* Testuje zgodność implementacji funkcji golden_possible z jej opisem. */
static void golden_possible(void **state) {
    (void) state;
    gamma_t *g = gamma_new(10, 10, 3, 1);
    assert_non_null(g);

    assert_true(gamma_move(g, 2, 1, 1));
    assert_true(gamma_move(g, 2, 1, 2));
    assert_true(gamma_move(g, 2, 1, 3));

    assert_true(gamma_golden_possible(g, 1));
    assert_false(gamma_golden_move(g, 1, 1, 2));
    assert_false(gamma_golden_move(g, 1, 2, 2));
    assert_true(gamma_golden_move(g, 1, 1, 1));

    gamma_delete(g);
    g = gamma_new(9, 9, 2, 2);
    assert_non_null(g);

    assert_true(gamma_move(g, 2, 1, 1));
    assert_true(gamma_move(g, 2, 1, 2));
    assert_true(gamma_move(g, 2, 1, 3));
    assert_true(gamma_move(g, 1, 5, 4));
    assert_true(gamma_move(g, 1, 4, 5));
    assert_true(gamma_golden_possible(g, 1));
    assert_false(gamma_golden_move(g, 1, 1, 1));
    assert_false(gamma_golden_move(g, 1, 1, 2));
    assert_false(gamma_golden_move(g, 1, 1, 3));

    gamma_delete(g);
}

/* Testuje liczenie obszarów jednego gracza. */
static void areas(void **state) {
    (void) state;
    gamma_t *g = gamma_new(31, 37, 1, 42);
    assert(g != NULL);

    for (uint32_t i = 0; i < 21; ++i) {
        assert_true(gamma_move(g, 1, i, i));
        assert_true(gamma_move(g, 1, i + 2, i));
    }

    assert_false(gamma_move(g, 1, 0, 2));
    assert_false(gamma_move(g, 1, 0, 4));

    for (uint32_t i = 0; i < 9; ++i)
        assert_true(gamma_move(g, 1, i + 1, i));

    for (uint32_t i = 2; i <= 36; i += 2)
        assert_true(gamma_move(g, 1, 0, i));

    assert_false(gamma_move(g, 1, 4, 0));
    assert_false(gamma_move(g, 1, 6, 0));

    gamma_delete(g);
}

/* Testuje rozgałęzione obszary. */
static void tree(void **state) {
    (void) state;
    gamma_t *g = gamma_new(16, 15, 3, 2);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 1, 1));
    assert_true(gamma_move(g, 1, 12, 3));
    assert_true(gamma_move(g, 1, 2, 1));
    assert_true(gamma_move(g, 1, 12, 4));
    assert_true(gamma_move(g, 1, 1, 2));
    assert_true(gamma_move(g, 1, 12, 5));
    assert_true(gamma_move(g, 1, 3, 1));
    assert_true(gamma_move(g, 1, 12, 2));
    assert_true(gamma_move(g, 1, 1, 3));
    assert_true(gamma_move(g, 1, 12, 1));
    assert_true(gamma_move(g, 1, 4, 1));
    assert_true(gamma_move(g, 1, 11, 3));
    assert_true(gamma_move(g, 1, 2, 3));
    assert_true(gamma_move(g, 1, 10, 3));
    assert_true(gamma_move(g, 1, 5, 1));
    assert_true(gamma_move(g, 1, 13, 3));
    assert_true(gamma_move(g, 1, 3, 3));
    assert_true(gamma_move(g, 1, 14, 3));
    assert_true(gamma_move(g, 1, 6, 1));
    assert_true(gamma_move(g, 1, 10, 2));
    assert_true(gamma_move(g, 1, 4, 3));
    assert_true(gamma_move(g, 1, 10, 1));
    assert_true(gamma_move(g, 1, 7, 1));
    assert_true(gamma_move(g, 1, 11, 1));
    assert_true(gamma_move(g, 1, 5, 3));
    assert_true(gamma_move(g, 1, 13, 1));
    assert_true(gamma_move(g, 1, 8, 1));
    assert_true(gamma_move(g, 1, 14, 1));
    assert_true(gamma_move(g, 1, 6, 3));
    assert_true(gamma_move(g, 1, 14, 2));
    assert_true(gamma_move(g, 1, 6, 4));
    assert_true(gamma_move(g, 1, 14, 4));
    assert_true(gamma_move(g, 1, 7, 4));
    assert_true(gamma_move(g, 1, 14, 5));
    assert_true(gamma_move(g, 1, 7, 5));
    assert_true(gamma_move(g, 1, 13, 5));
    assert_true(gamma_move(g, 1, 11, 4));
    assert_true(gamma_move(g, 1, 11, 5));
    assert_true(gamma_move(g, 1, 10, 5));
    assert_true(gamma_move(g, 1, 13, 2));
    assert_true(gamma_move(g, 1, 13, 4));
    assert_true(gamma_move(g, 1, 9, 1));
    assert_true(gamma_move(g, 1, 4, 7));
    assert_true(gamma_move(g, 1, 4, 8));
    assert_true(gamma_move(g, 1, 4, 9));
    assert_true(gamma_move(g, 1, 4, 10));
    assert_true(gamma_move(g, 1, 4, 11));
    assert_true(gamma_move(g, 1, 4, 12));
    assert_true(gamma_move(g, 1, 4, 13));
    assert_true(gamma_move(g, 1, 3, 10));
    assert_true(gamma_move(g, 1, 2, 10));
    assert_true(gamma_move(g, 1, 1, 10));
    assert_true(gamma_move(g, 1, 5, 10));
    assert_true(gamma_move(g, 1, 6, 10));
    assert_true(gamma_move(g, 1, 7, 10));
    assert_true(gamma_move(g, 1, 8, 10));
    assert_true(gamma_move(g, 1, 3, 8));
    assert_true(gamma_move(g, 1, 5, 8));
    assert_true(gamma_move(g, 1, 2, 9));
    assert_true(gamma_move(g, 1, 2, 11));
    assert_true(gamma_move(g, 1, 3, 12));
    assert_true(gamma_move(g, 1, 5, 12));
    assert_true(gamma_move(g, 1, 7, 9));
    assert_true(gamma_move(g, 1, 7, 8));
    assert_true(gamma_move(g, 1, 7, 7));
    assert_true(gamma_move(g, 1, 7, 11));
    assert_true(gamma_move(g, 1, 7, 12));
    assert_true(gamma_move(g, 1, 8, 12));
    assert_true(gamma_move(g, 1, 8, 13));
    assert_true(gamma_move(g, 1, 9, 10));
    assert_true(gamma_move(g, 1, 10, 10));
    assert_true(gamma_move(g, 1, 11, 10));
    assert_true(gamma_move(g, 1, 11, 9));
    assert_true(gamma_move(g, 1, 11, 8));
    assert_true(gamma_move(g, 1, 12, 8));
    assert_true(gamma_move(g, 1, 12, 7));
    assert_true(gamma_move(g, 1, 11, 11));
    assert_true(gamma_move(g, 1, 11, 12));
    assert_true(gamma_move(g, 1, 10, 12));
    assert_true(gamma_move(g, 1, 12, 10));
    assert_true(gamma_move(g, 1, 13, 10));
    assert_true(gamma_move(g, 1, 13, 11));
    assert_true(gamma_move(g, 1, 14, 10));
    assert_true(gamma_move(g, 1, 13, 9));

    assert_false(gamma_move(g, 1, 2, 5));
    assert_false(gamma_golden_move(g, 3, 9, 10));
    assert_true(gamma_move(g, 1, 9, 12));
    assert_true(gamma_golden_move(g, 3, 9, 10));
    assert_true(gamma_move(g, 2, 7, 6));
    assert_true(gamma_golden_move(g, 1, 7, 6));
    assert_true(gamma_golden_move(g, 2, 9, 12));
    assert_false(gamma_move(g, 1, 9, 7));

    gamma_delete(g);
}

/* Testuje ruchy wykonywane wyłącznie na brzegu planszy. */
static void border(void **state) {
    (void) state;
    static const char board[] =
            "321442\n"
            "4....1\n"
            "4....4\n"
            "1....3\n"
            "123412\n";

    gamma_t *g = gamma_new(6, 5, 4, 4);
    assert_non_null(g);

    assert_true(gamma_move(g, 1, 0, 0));
    assert_true(gamma_move(g, 2, 1, 0));
    assert_true(gamma_move(g, 3, 2, 0));
    assert_true(gamma_move(g, 4, 3, 0));
    assert_true(gamma_move(g, 1, 4, 0));
    assert_true(gamma_move(g, 2, 5, 0));
    assert_true(gamma_move(g, 3, 5, 1));
    assert_true(gamma_move(g, 4, 5, 2));
    assert_true(gamma_move(g, 1, 5, 3));
    assert_true(gamma_move(g, 2, 5, 4));
    assert_true(gamma_move(g, 3, 4, 4));
    assert_true(gamma_move(g, 4, 3, 4));
    assert_true(gamma_move(g, 1, 2, 4));
    assert_true(gamma_move(g, 2, 1, 4));
    assert_true(gamma_move(g, 3, 0, 4));
    assert_true(gamma_move(g, 4, 0, 3));

    assert_false(gamma_move(g, 1, 0, 2));
    assert_false(gamma_move(g, 2, 0, 1));
    assert_false(gamma_move(g, 3, 0, 2));
    assert_false(gamma_move(g, 4, 0, 1));
    assert_true(gamma_move(g, 4, 0, 2));
    assert_true(gamma_move(g, 1, 0, 1));
    assert_false(gamma_golden_move(g, 3, 2, 4));
    assert_true(gamma_golden_move(g, 4, 4, 4));

    char *p = gamma_board(g);
    assert_non_null(p);
    assert_true(strcmp(p, board) == 0);
    free(p);

    assert_true(gamma_busy_fields(g, 1) == 5);
    assert_true(gamma_free_fields(g, 1) == 4);
    assert_true(gamma_busy_fields(g, 2) == 4);
    assert_true(gamma_free_fields(g, 2) == 2);
    assert_true(gamma_busy_fields(g, 3) == 3);
    assert_true(gamma_free_fields(g, 3) == 12);
    assert_true(gamma_busy_fields(g, 4) == 6);
    assert_true(gamma_free_fields(g, 4) == 6);

    gamma_delete(g);
}

/* Wykonanie niektórych funkcji w tym teście może się nie udać z powodu braku
 * pamięci, ale nie powinno to skutkować załamaniem wykonywania programu. */
static int memory_alloc(void) {
    gamma_t *g = gamma_new(SMALL_BOARD_SIZE, SMALL_BOARD_SIZE, 2, 2);
    assert(g != NULL);

    /* Alokujemy całą dostępną pamięć. */
    for (size_t s = 1024 * 1024 * 1024; s >= 1; s /= 2) {
        void *p;
        do {
            p = malloc(s);
        } while (p);
    }

    while (gamma_new(MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, 10, 3) != NULL);

    for (uint32_t x = 0; x < SMALL_BOARD_SIZE; ++x)
        for (uint32_t y = 0; y < SMALL_BOARD_SIZE; ++y)
            gamma_move(g, 1, x, y);

    /* To jest test alokacji pamięci – nie zwalniamy jej. */
    return MEM_PASS;
}

/* Testuje odporność implementacji na duże wartości parametrów w gamma_new. */
static void big_board(void **state) {
    (void) state;
    static const gamma_param_t game[] = {
            {       UINT32_MAX,        UINT32_MAX,        UINT32_MAX,        UINT32_MAX},
            {                2,        UINT32_MAX,        UINT32_MAX,        UINT32_MAX},
            {       UINT32_MAX,                 2,        UINT32_MAX,        UINT32_MAX},
            {       UINT32_MAX,        UINT32_MAX,                 2,        UINT32_MAX},
            {       UINT32_MAX,        UINT32_MAX,        UINT32_MAX,                 5},
            {       UINT32_MAX,                 2,                 2,                 5},
            {                2,        UINT32_MAX,                 2,                 5},
            {                2,                 2,        UINT32_MAX,                 5},
            {                2,                 2,                 2,        UINT32_MAX},
            {          1 << 16,           1 << 16,                 2,                 5},
            {    (1 << 24) + 1,           1 <<  8,                 2,                 5},
            {    (1 << 15) + 1,     (1 << 14) + 1,                 2,                 5},
            {    (1 << 14) + 1,     (1 << 13) + 1,                 2,                 5},
            {    (1 << 13) + 1,     (1 << 12) + 1,                 2,                 5},
            {    (1 << 12) + 1,     (1 << 11) + 1,                 2,                 5},
            {    (1 << 11) + 1,     (1 << 10) + 1,                 2,                 5},
            {   BIG_BOARD_SIZE,    BIG_BOARD_SIZE,    BIG_BOARD_SIZE,    BIG_BOARD_SIZE},
            {MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE},
            { SMALL_BOARD_SIZE,  SMALL_BOARD_SIZE,  SMALL_BOARD_SIZE,  SMALL_BOARD_SIZE},
    };

    unsigned success = 0;
    for (size_t i = 0; i < SIZE(game); ++i) {
        errno = 0;
        gamma_t *g = gamma_new(game[i].width, game[i].height,
                               game[i].players, game[i].areas);
        if (g != NULL) {
            uint64_t size = (uint64_t)game[i].width * (uint64_t)game[i].height;
            assert_true(gamma_free_fields(g, game[i].players) == size);
            // Przynajmniej cztery ruchy powinny się udać.
            assert_true(gamma_move(g, game[i].players, 0, 0));
            assert_true(gamma_move(g, game[i].players, 0, game[i].height - 1));
            assert_true(gamma_move(g, game[i].players, game[i].width - 1, 0));
            assert_true(gamma_move(g, game[i].players, game[i].width - 1, game[i].height - 1));
            assert_true(gamma_free_fields(g, game[i].players) == size - 4);
            assert_true(gamma_busy_fields(g, game[i].players) == 4);
            gamma_delete(g);
            ++success;
        }
        else {
            assert_true(!test_errno || errno == ENOMEM);
        }
    }
    // Przynajmniej jedną z tych gier powinno się udać utworzyć.
    assert_true(success > 0);
}

/* Testuje ogranicznenia na rozmiar planszy w gamma_new. */
static void middle_board(void **state) {
    (void) state;
    unsigned success = 0;
    for (uint32_t size = 8000; size >= 125; size /= 2) {
        gamma_t *g1 = gamma_new(size, size, 2, 2);
        if (g1 != NULL) {
            gamma_delete(g1);
            // Jeśli udało się zaalokować planszę o rozmiarze size * size, to powinno
            // się też udać zaalokować inną planszę o nieco mniejszej powierzchni.
            gamma_t *g2;
            g2 = gamma_new((size - 1) * (size - 1), 1, 2, 2);
            assert_non_null(g2);
            gamma_delete(g2);
            g2 = gamma_new(1, (size - 1) * (size - 1), 2, 2);
            assert_non_null(g2);
            gamma_delete(g2);
            ++success;
        }
        else {
            assert_true(!test_errno || errno == ENOMEM);
        }
    }
    // Przynajmniej jedną z tych gier powinno się udać utworzyć.
    assert_true(success > 0);
}

/** URUCHAMIANIE TESTÓW **/

typedef struct {
    char const *name;
    int (*function)(void);
} test_list_t;

#define TEST(t) {#t, t}

/*static const test_list_t test_list[] = {
        TEST(memory_alloc),
};*/

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(example),
        cmocka_unit_test(minimal),
        cmocka_unit_test(big_board),
        cmocka_unit_test(middle_board),
        cmocka_unit_test(params),
        cmocka_unit_test(delete_null),
        cmocka_unit_test(many_players),
        cmocka_unit_test(many_games),
        cmocka_unit_test(normal_move),
        cmocka_unit_test(golden_move),
        cmocka_unit_test(golden_possible),
        cmocka_unit_test(areas),
        cmocka_unit_test(tree),
        cmocka_unit_test(border),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
