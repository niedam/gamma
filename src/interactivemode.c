/** @file
 * Implementacja trybu interaktywnego umożliwiającego rozgrywkę przy użyciu
 * silnika gry Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <inttypes.h>
#include "interactivemode.h"
#include "stringology.h"
#include "gamma.h"

/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


/** Stały ciąg znaków reprezentujący pozytywną odpowiedź
 * na @ref gamma_golden_possible.
 */
#define ANSWER_YES "YES"


/** Stały ciąg znaków reprezentujący negatywną odpowiedź
 * na @ref gamma_golden_possible.
 */
#define ANSWER_NO "NO"


/** Kod ANSI escape resetujący terminal.
 */
#define CLEAR_CONSOLE "\033c"


/** Kod ANSI escape pokazujący domyślny kursor terminala.
 */
#define SHOW_CURSOR "\033[?25h"


/** Kod ANSI escape ukrywający domyślny kursor terminala.
 */
#define HIDE_CURSOR "\033[?25l"


/** Kod ANSI escape zapobiegający zawijaniu wierszy w terminalu.
 */
#define DISABLED_WRAPLINE "\033[?7l"


/** Kod ANSI escape przywracający zawijanie wierszy w terminalu.
 */
#define ENABLED_WRAPLINE "\033[?7h"


/** Kod ANSI escape włączający podświetlanie wypisywanego tekstu w terminalu.
 */
#define ENABLED_HIGHLINE "\033[7m"


/** Kod ANSI escape wyłączający podświetlanie wypisywanego tekstu w terminalu.
 */
#define DISABLED_HIGHLINE "\033[0m"


/** @brief Sygnatura służąca do wypisywania planszy przez funkcje z
 * rodziny printf. Należy używać razem z @ref BOARD_DESCRIPTION.
 */
#define BOARD_SIGNATURE DISABLED_WRAPLINE "%.*s" \
                        ENABLED_HIGHLINE "%.*s" DISABLED_HIGHLINE \
                        "%s" ENABLED_WRAPLINE


/** @brief Sygnatura służąca do wypisywania przez funkcje z rodziny prinrf
 * statystyk dotyczących gracza, którego tura trwa.
 */
#define PLAYER_SIGNATURE "<PLAYER %*.d | Busy: %*.1"PRIu64" | " \
                         "Free: %*.1"PRIu64" | Golden move: %3.3s>\n"


/** Kod klawisza: strzałka w górę.
 */
#define UP_KEY 65


/** Kod klawisza: strzałka w dół.
 */
#define DOWN_KEY 66


/** Kod klawisza: strzałka w prawo.
 */
#define RIGHT_KEY 67


/** Kod klawisza: strzałka w lewo.
 */
#define LEFT_KEY 68


/** @brief Makro ustawia parametry w taki sposób, żeby były zgodne z
 * @ref BOARD_SIGNATURE
 * @param[in] board             – ciąg znaków reprezentujący planszę,
 * @param[in] first             – liczba znaków przed wyróżnionym polem,
 * @param[in] id_len            – długość wyróżnionego pola.
 */
#define BOARD_DESCRIPTION(board, first, id_len)  \
            /*  PARAMETRY ZWIĄZANE Z WYPISYWANĄ PLANSZĄ. */ \
            /* Liczba znaków przed zaznaczeniem: */ first, \
            /* Plansza: */ board, \
            /* Długość pola na planszy: */ id_len, \
            /* Wskaźnik na wyróżnione pole: */ board + first, \
            /* Reszta planszy do wypisania: */ board + first + id_len


/** @brief Makro ustawia parametry w taki sposób, żeby były zgodne z
 * @ref PLAYER_SIGNATURE.
 * @param[in] player_id         – identyfikator gracza,
 * @param[in] length_id         – maksymalna długość identyfikatora gracza,
 * @param[in] busy_fields       – liczba pól zajętych przez gracza,
 * @param[in] free_fields       – liczba pól które może zająć gracz,
 * @param[in] length_fields     – maksymalna długość liczby pól,
 * @param[in] golden_move       – informacja o dostępności złotego ruchu.
 */
#define PLAYER_DESCRIPTION(player_id, length_id, busy_fields, \
                           free_fields, length_fields, golden_move) \
            /* Długość identyfikatorów graczy: */ length_id, \
            /* Identyfikator aktualnego gracza: */ player_id, \
            /* Liczba cyfr w max. liczbie pól: */ length_fields, \
            /* Liczba zajętych pól: */ busy_fields, \
            /* Liczba cyfr w max. liczbie pól: */ length_fields, \
            /* Liczba możliwych do zajęcia pól: */ free_fields, \
            /* Czy dostępny złoty ruch: */ golden_move


/** Model rozgrywki w trybie interaktywnym.
 */
struct interactive_model {
    gamma_t *game; /**< Wskaźnik na silnik gry. */
    struct termios old_attr; /**< Poprzednie ustawienia terminala. */
    int player_len; /**< Długość identyfikatora gracza. */
    int fields_len; /**< Długość maksymalnej liczby pól. */
    uint32_t current_player; /**< Identyfikator gracza, którego trwa tura. */
    uint32_t current_column; /**< Numer kolumny w której znajduje się kursor. */
    uint32_t current_row; /**< Numer wiersza w którym znajduje się kursor. */
};


/** Globalny model funkcjonujący w programie.
 */
static struct interactive_model model;


/** @brief Funkcja czyszcząca terminal.
 */
static void interactive_clear() {
    printf(CLEAR_CONSOLE HIDE_CURSOR);
}


/** @brief Przywrócenie terminalowi właściwości sprzed przejścia do trybu
 * interaktywnego.
 * Funkcje należy wywołać pod koniec działania programu.
 */
static void finish_program() {
    printf(SHOW_CURSOR);
    tcsetattr(STDIN_FILENO, TCSANOW, &model.old_attr);
}


/** @brief Inicjacja modelu (@ref model) działającego w ramach trybu
 * interaktywnego, konfiguracja terminala.
 * @param[in] g                 – wskaźnik na strukturę silnika gry Gamma,
 * @param[out] m                – model do zainicjowania.
 */
static void interactive_init(gamma_t *g, struct interactive_model *m) {
    if (ISNULL(g) || ISNULL(m)) {
        return;
    }
    m->game = g;
    m->current_player = 1;
    m->current_column = (gamma_width(g) - 1) / 2;
    m->current_row = (gamma_height(g) - 1) / 2;
    m->player_len = uint64_length((uint64_t) gamma_players(g));
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


/** @brief Sprawdzenie, czy aktualny gracz może wykonać jakikolwiek ruch.
 * Funkcja sprawdza, czy dla aktualnego gracza funkcja @ref gamma_free_fields
 * lub @ref gamma_golden_possible zwróciły wartość umożliwiającą wykonanie ruchu.
 * @param[in] m                 – wskaźnik na model trybu interaktywnego.
 * @return Wartość @p true jeżeli gracz aktualny gracz może wykonać ruch,
 * wartość @p false jeżeli gracz nie może wykonać ruchu lub parametr jest
 * niepoprawny.
 */
static bool interactive_available_player(const struct interactive_model *m) {
    if (ISNULL(m)) {
        return false;
    }
    return gamma_golden_possible(m->game, m->current_player)
           || gamma_free_fields(m->game, m->current_player) > 0;
}


/** Zmiana położenia kursora na planszy.
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego,
 * @param[in] x                 – liczba pól do przesunięcia kursora w poziomie,
 * @param[in] y                 – liczba pól do przesunięcia kursora w pionie.
 */
static void interactive_cursor(struct interactive_model *m, int x, int y) {
    if (ISNULL(m)) {
        return;
    }
    uint32_t board_width = gamma_width(m->game);
    uint32_t board_height = gamma_height(m->game);
    // Sprawdzenie, czy kursor znajduje się na poprawnej pozycji.
    if (m->current_column + 1 + x >= 1 && m->current_column + x < board_width
         && m->current_row + 1 + y >= 1 && m->current_row + y < board_height) {
        m->current_column += x;
        m->current_row += y;
    }
}


/** @brief Wypisanie planszy do terminala na podstawie modelu.
 * @param[in] m                 – wskaźnik na model trybu interaktywnego.
 */
static void interactive_view(const struct interactive_model *m) {
    if (ISNULL(m)) {
        return;
    }
    char *board = gamma_board(m->game);
    uint64_t busy_fields = gamma_busy_fields(m->game, m->current_player);
    uint64_t free_fields = gamma_free_fields(m->game, m->current_player);
    const char *golden_move = gamma_golden_possible(m->game, m->current_player)
                              ? ANSWER_YES : ANSWER_NO;
    // Ile początkowych znaków planszy wypisać przed wyróżnieniem
    // aktualnie zaznaczonego kursorem pola:
    uint32_t first_chars = (gamma_width(m->game)) * m->current_row + m->current_column;
    first_chars *= uint64_length((uint64_t) gamma_players(m->game));
    first_chars += m->current_row; // Doliczamy endline'y (1 char na końcu
                                   // każdego wiersza).
    interactive_clear();
    printf(BOARD_SIGNATURE PLAYER_SIGNATURE,
           BOARD_DESCRIPTION(board, first_chars, m->player_len),
           PLAYER_DESCRIPTION(m->current_player, m->player_len,
                              busy_fields, free_fields,
                              m->fields_len, golden_move));
    free(board);
}


/** @brief Zakończenie rozgrywki prowadzonej w trybie interaktywnym.
 * @param[in] m                 – wskaźnik na model trybu interaktywnego.
 */
static void interactive_finish(struct interactive_model *m) {
    if (ISNULL(m)) {
        return;
    }
    char *board = gamma_board(m->game);
    interactive_clear();
    printf(BOARD_SIGNATURE, BOARD_DESCRIPTION(board, 0, 0));
    uint32_t players = gamma_players(m->game);
    for (uint32_t p = 1; p <= players; ++p) {
        printf("PLAYER %*.1d %"PRIu64"\n", m->player_len, p,
               gamma_busy_fields(m->game, p));
    }
    free(board);
    exit(EXIT_SUCCESS);
}


/** @brief Przejście do następnego w kolejności gracza, który może wykonać ruch.
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego.
 */
static void interactive_next_player(struct interactive_model *m) {
    uint32_t players = gamma_players(m->game);
    uint32_t no_move_players = 0;
    do {
        m->current_player = m->current_player == gamma_players(m->game) ?
                            1 : m->current_player + 1;
        no_move_players++;
    } while (!interactive_available_player(m) &&  no_move_players < players);
    if (no_move_players == players) {
        interactive_finish(m);
    }
}


/** @brief Wykonanie ruchu przy użyciu silnika gry Gamma.
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego,
 * @param[in] fun_move          – funkcja do wykonania na silniku gry Gamma.
 */
static void interactive_move(struct interactive_model *m,
                             bool (*fun_move)(gamma_t *, uint32_t, uint32_t, uint32_t)) {
    if (ISNULL(fun_move) || ISNULL(m)) {
        return;
    }
    if (fun_move(m->game, m->current_player,
                 m->current_column, gamma_height(m->game) - 1 - m->current_row)) {
        interactive_next_player(m);
    }
}


/** @brief Sterowanie w trybie interaktywnym.
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego.
 */
static void interactive_control(struct interactive_model *m) {
    if (ISNULL(m)) {
        return;
    }
    int c = getchar();
    if (c == 27 && (c = getchar()) == 91) {
        // Wciśnięcie strzałek.
        c = getchar();
        switch (c) {
            case RIGHT_KEY:
                interactive_cursor(m, 1, 0);
                break;
            case LEFT_KEY:
                interactive_cursor(m, -1, 0);
                break;
            case UP_KEY:
                interactive_cursor(m, 0, -1);
                break;
            case DOWN_KEY:
                interactive_cursor(m, 0, 1);
                break;
            default:
                break;
        }
    } else {
        // Wciśnięcie normalnych znaków.
        switch (c) {
            case 4:
                interactive_finish(m);
                break;
            case (int) 'G':
            case (int) 'g':
                interactive_move(m, gamma_golden_move);
                break;
            case (int) 'M':
            case (int) 'm':
                interactive_move(m, gamma_move);
                break;
            case (int) 'C':
            case (int) 'c':
                interactive_next_player(m);
                break;
            default:
                break;
        }
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