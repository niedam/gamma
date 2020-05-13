#define __STDC_FORMAT_MACROS
#include <stdlib.h>
#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <inttypes.h>
#include "interactivemode.h"
#include "stringology.h"
#include "gamma.h"

#define ISNULL(ptr) (ptr == NULL)


#define CLEAR_CONSOLE "\033c"
#define SHOW_CURSOR "\033[?25h"
#define HIDE_CURSOR "\033[?25l"

#define DISABLED_WRAPLINE "\033[?7l"
#define ENABLED_WRAPLINE "\033[?7h"
#define ENABLED_HIGHLINE "\033[7m"
#define DISABLED_HIGHLINE "\033[0m"
#define BOARD_SIGNATURE DISABLED_WRAPLINE "%.*s" \
                        ENABLED_HIGHLINE "%.*s" DISABLED_HIGHLINE \
                        "%s" ENABLED_WRAPLINE
#define PLAYER_SIGNATURE "<PLAYER %*.d | Busy fields: %*.1"PRIu64" | " \
                         "Free fields: %*.1"PRIu64" | Golden move: %3.3s>\n"




#define BOARD_DESCRIPTION(board, first, id_len)  \
            /*  PARAMETRY ZWIĄZANE Z WYPISYWANĄ PLANSZĄ. */ \
            /* Liczba znaków przed zaznaczeniem: */ first, \
            /* Plansza: */ board, \
            /* Długość pola na planszy: */ id_len, \
            /* Wskaźnik na wyróżnione pole: */ board + first, \
            /* Reszta planszy do wypisania: */ board + first + id_len


#define PLAYER_DESCRIPTION(player_id, length_id, length_fields, \
                           busy_fields, free_fields, golden_move) \
            /* Długość identyfikatorów graczy: */ length_id, \
            /* Identyfikator aktualnego gracza: */ player_id, \
            /* Liczba cyfr w max. liczbie pól: */ length_fields, \
            /* Liczba zajętych pól: */ busy_fields, \
            /* Liczba cyfr w max. liczbie pól: */ length_fields, \
            /* Liczba możliwych do zajęcia pól: */ free_fields, \
            /* Czy dostępny złoty ruch: */ golden_move


static struct interactive_model {
    gamma_t *game;
    struct termios old_attr;
    int player_len;
    int fields_len;
    uint32_t current_player;
    uint32_t current_column;
    uint32_t current_row;
} model;


static void interactive_clear() {
    printf(CLEAR_CONSOLE HIDE_CURSOR);
}


static void finish_program() {
    printf(SHOW_CURSOR);
    tcsetattr(STDIN_FILENO, TCSANOW, &model.old_attr);
}


static void interactive_init(gamma_t *g, struct interactive_model *m) {
    if (ISNULL(g) || ISNULL(m)) {
        return;
    }
    m->game = g;
    m->current_player = 1;
    m->current_column = (gamma_width(g) - 1) / 2;
    m->current_row = (gamma_height(g) - 1) / 2;
    m->player_len = uint32_length(gamma_players(g));
    m->fields_len = uint64_length(gamma_width(g) * gamma_height(g));
    struct termios new_attr;
    if (tcgetattr(STDIN_FILENO, &m->old_attr) < 0) {
        exit(EXIT_FAILURE);
    }
    new_attr = m->old_attr;
    new_attr.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_attr) < 0) {
        exit(EXIT_FAILURE);
    }
}


/**
 *
 * @param m
 * @param fun_move
 */
static void interactive_move(struct interactive_model *m,
                             bool (*fun_move)(gamma_t *, uint32_t, uint32_t, uint32_t)) {
    if (ISNULL(fun_move) || ISNULL(m)) {
        return;
    }
    if (fun_move(m->game, m->current_player,
                 m->current_column, gamma_height(m->game) - 1 - m->current_row)) {
        m->current_player = m->current_player == gamma_players(m->game) ?
                            1 : m->current_player + 1;
    }
}

static void interactive_view(struct interactive_model *m) {
    if (ISNULL(m)) {
        return;
    }
    char *board = gamma_board(m->game);
    uint64_t busy_fields = gamma_busy_fields(m->game, m->current_player);
    uint64_t free_fields = gamma_free_fields(m->game, m->current_player);
    const char *golden_move = gamma_golden_possible(m->game, m->current_player) ? "YES" : "NO";
    uint32_t i = (gamma_width(m->game)) * m->current_row + m->current_column;
    i *= uint32_length(gamma_players(m->game));
    i += m->current_row;
    interactive_clear();
    printf(BOARD_SIGNATURE PLAYER_SIGNATURE,
           BOARD_DESCRIPTION(board, i, m->player_len),
           PLAYER_DESCRIPTION(m->current_player, m->player_len,
                              m->fields_len, busy_fields,
                              free_fields, golden_move));
    free(board);
}


static void interactive_finish(struct interactive_model *m) {
    if (ISNULL(m)) {
        return;
    }
    char *board = gamma_board(m->game);
    interactive_clear();
    printf(BOARD_SIGNATURE, BOARD_DESCRIPTION(board, 0, 0));
    free(board);
    uint32_t players = gamma_players(m->game);
    for (uint32_t p = 1; p <= players; ++p) {
        printf("PLAYER %*.1d %"PRIu64"\n", m->player_len, p,
               gamma_busy_fields(m->game, p));
    }
    exit(EXIT_SUCCESS);
}


static void interactive_control(struct interactive_model *m) {
    if (ISNULL(m)) {
        return;
    }
    uint32_t board_width = gamma_width(m->game);
    uint32_t board_height = gamma_height(m->game);
    int c = getchar();
    switch (c) {
        case 67:
            if (m->current_column < board_width - 1) {
                m->current_column++;
            }
            break;
        case 68:
            if (m->current_column > 0) {
                m->current_column--;
            }
            break;
        case 65:
            if (m->current_row > 0) {
                m->current_row--;
            }
            break;
        case 66:
            if (m->current_row < board_height - 1) {
                m->current_row++;
            }
            break;
        case 4:
            interactive_finish(m);
            break;
        case 'G':
        case 'g':
            interactive_move(m, gamma_golden_move);
            break;
        case 'M':
        case 'm':
            interactive_move(m, gamma_move);
            break;
        default:
            break;
    }
}

void interactive_run(gamma_t *g) {
    if (ISNULL(g)) {
        exit(EXIT_FAILURE);
    }
    struct interactive_model *m = &model;
    interactive_init(g, m);
    atexit(finish_program);
    interactive_view(m);
    while (true) {
        interactive_control(m);
        interactive_view(m);
    }
}