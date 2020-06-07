/** @file
 * Implementacja trybu interaktywnego umożliwiającego rozgrywkę przy użyciu
 * silnika gry Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 12.06.2020
 */

/** Makro umożliwiające używanie funkcji `nanoslepp()`.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include "interactive_mode.h"
#include "stringology.h"
#include "gamma.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


/** Czas trwania jednej klatki w animacji robienia ruchu w nanosekundach.
 */
#define ANIMATION_DURATION 80000000L


/** Stały ciąg znaków reprezentujący pozytywną odpowiedź
 * na @ref gamma_golden_possible.
 */
#define ANSWER_YES "YES"


/** Stały ciąg znaków reprezentujący negatywną odpowiedź
 * na @ref gamma_golden_possible.
 */
#define ANSWER_NO "NO"


/** Kod ASCII klawisza `Escape`.
 */
#define ESC_KEY '\e'


/** Kod ASCII klawisza `[`.
 */
#define BRAC_KEY '['


/** Kod ANSI escape resetujący terminal.
 */
#define CLEAR_CONSOLE "\ec"


/** Kod ANSI escape pokazujący domyślny kursor terminala.
 */
#define SHOW_CURSOR "\e[?25h"


/** Kod ANSI escape ukrywający domyślny kursor terminala.
 */
#define HIDE_CURSOR "\e[?25l"


/** Kod ANSI escape zapobiegający zawijaniu wierszy w terminalu.
 */
#define DISABLED_WRAPLINE "\e[?7l"


/** Kod ANSI escape przywracający zawijanie wierszy w terminalu.
 */
#define ENABLED_WRAPLINE "\e[?7h"


/** Kod ANSI escape do wypisywania aktualnie wskazywanego pola.
 */
#define HIGHLINE_COLOR "\e[41m"


/** Kod ANSI escape do wypisywania pól parzystych.
 */
#define EVEN_COLOR "\e[104m"


/** Kod ANSI escape do wypisywania pól nieparzystych.
 */
#define ODD_COLOR "\e[105m"


/** Kod ANSI escape zaznaczający pole na złoto.
 */
#define GOLD_COLOR "\e[103m"


/** Kod ANSI escape zaznaczający pole na ciemno złoto.
 */
#define GOLD_COLOR_DARK "\e[43m"


/** Kod ANSI escape zaznaczający pole na zielono.
 */
#define GREEN_COLOR "\e[102m"


/** Kod ANSI escape zaznaczający pole na ciemno zielono.
 */
#define GREEN_COLOR_DARK "\e[42m"


/** Kod ANSI escape wyłączający podświetlanie wypisywanego tekstu w terminalu.
 */
#define DISABLE_EFFECTS "\e[0m"


/** @brief Sygnatura służąca do wypisywania przez funkcje z rodziny printf
 * statystyk dotyczących gracza, którego tura trwa.
 */
#define BOARD_SIGNATURE DISABLED_WRAPLINE "%s" ENABLED_WRAPLINE


/** @brief Sygnatura służąca do wypisywania przez funkcje z rodziny printf
 * statystyk dotyczących gracza, którego tura trwa.
 */
#define PLAYER_SIGNATURE \
"<PLAYER %*.d | Busy: %*.1"PRIu64" | " \
"Free: %*.1"PRIu64" | Golden move: %3.3s>\n"


/** Kod klawisza: strzałka w górę.
 */
#define UP_KEY 'A'


/** Kod klawisza: strzałka w dół.
 */
#define DOWN_KEY 'B'


/** Kod klawisza: strzałka w prawo.
 */
#define RIGHT_KEY 'C'


/** Kod klawisza: strzałka w lewo.
 */
#define LEFT_KEY 'D'


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
    char *board_buffer; /**< Bufor na wypisywane o planszy informacje. */
    char *result_buffer; /**< Bufor na planszę z kolorowymi efektami. */
    size_t board_buffer_size; /**< Rozmiar bufora tekstowego. */
    size_t result_buffer_size; /**< Rozmiar bufora planszy z kolorami. */
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
    free(model.board_buffer);
    free(model.result_buffer);
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
    m->board_buffer = gamma_board(g);
    if (ISNULL(m->board_buffer)) {
        exit(EXIT_FAILURE);
    }
    m->board_buffer_size = strlen(m->board_buffer) + 1;
    m->result_buffer_size = m->board_buffer_size + (((sizeof(EVEN_COLOR) - 1) *
            (gamma_width(g) + 1)) * (gamma_height(g)));
    m->result_buffer = calloc(m->result_buffer_size, sizeof(char));
    if (ISNULL(m->result_buffer)) {
        exit(EXIT_FAILURE);
    }
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
    atexit(finish_program);
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


/** @brief Zmiana położenia kursora na planszy.
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


/** @brief Wpisanie planszy do bufora z dodaniem efektów wizualnych (kolorowanie
 * pól, zaznaczanie wskazywanego pola).
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego,
 * @param[in] highlight         – kod ANSI escape koloru, którym ma być
 *                                w którym ma być wskazywane przez gracza pole.
 */
static void interactive_compose_board(struct interactive_model *m,
                                      const char *highlight) {
    if (ISNULL(m)) {
        return;
    }
    char *current_field = m->board_buffer;
    char *current_buffer = m->result_buffer;
    size_t rest = m->result_buffer_size;
    for (uint64_t i = 0; i < gamma_height(m->game); ++i) {
        for (uint64_t j = 0; j < gamma_width(m->game); ++j) {
            int l;
            if (m->current_column == j && m->current_row == i) {
                l = snprintf(current_buffer, rest, "%s%.*s", highlight,
                             m->player_len, current_field);
            } else {
                const char *effect = (i + j) % 2 == 0 ? EVEN_COLOR : ODD_COLOR;
                l = snprintf(current_buffer, rest, "%s%.*s", effect,
                             m->player_len, current_field);
            }
            current_field += m->player_len;
            current_buffer += l;
            rest -= l;
        }
        int l = snprintf(current_buffer, rest, DISABLE_EFFECTS "\n");
        rest -= l;
        current_field++;
        current_buffer += l;
    }
}


/** @brief Wypisanie planszy do terminala na podstawie modelu.
 * @param[in] m                 – wskaźnik na model trybu interaktywnego,
 * @param[in] effect            – kod ANSI escape koloru, w jakim jest pole
 *                                wskazywane przez gracza.
 */
static void interactive_view(struct interactive_model *m, const char *effect) {
    if (ISNULL(m) || ISNULL(effect)) {
        return;
    }
    if (!gamma_board_buffer(m->game, m->board_buffer, m->board_buffer_size)) {
        exit(EXIT_FAILURE);
    }
    uint64_t busy_fields = gamma_busy_fields(m->game, m->current_player);
    uint64_t free_fields = gamma_free_fields(m->game, m->current_player);
    const char *golden_move = gamma_golden_possible(m->game, m->current_player)
                              ? ANSWER_YES : ANSWER_NO;
    interactive_compose_board(m, effect);
    interactive_clear();
    printf(BOARD_SIGNATURE PLAYER_SIGNATURE, m->result_buffer,
           PLAYER_DESCRIPTION(m->current_player, m->player_len,
                              busy_fields, free_fields,
                              m->fields_len, golden_move));
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
    printf("%s", m->board_buffer);
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


/** @brief Wyświetlenie planszy z animacją zmieniania koloru pola wskazywanego
 * przez gracza.
 * @param[in] m                 – wskaźnik na model trybu interaktywnego,
 * @param[in] effect1           – kod ANSI escape pierwszego koloru,
 * @param[in] effect2           – kod ANSI escape drugiego koloru.
 */
static void interactive_animation(struct interactive_model *m,
                                  const char *effect1, const char *effect2) {
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = ANIMATION_DURATION;
    const char *effects[] = { effect1, effect2, effect1 };
    for (size_t i = 0; i < sizeof(effects) / sizeof(effects[0]); ++i) {
        interactive_view(m, effects[i]);
        if (nanosleep(&tim, &tim2) < 0) {
            exit(EXIT_FAILURE);
        }
    }
}


/** @brief Wykonanie ruchu przy użyciu silnika gry Gamma.
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego,
 * @param[in] fun_move          – funkcja do wykonania na silniku gry Gamma,
 * @param[in] effect1           – pierwszy kolor animacji towarzyszącej ruchowi,
 * @param[in] effect2           – drugi kolor animacji towarzyszącej ruchowi.
 */
static void interactive_move(struct interactive_model *m,
                             bool (*fun_move)(gamma_t *, uint32_t, uint32_t, uint32_t),
                             const char *effect1, const char *effect2) {
    if (ISNULL(fun_move) || ISNULL(m)) {
        return;
    }
    if (fun_move(m->game, m->current_player,
                 m->current_column, gamma_height(m->game) - 1 - m->current_row)) {
        interactive_animation(m, effect1, effect2);
        interactive_next_player(m);
    }
}


/** @brief Obsługa wciśnięcia normalnego klawisza (innego niż strzałka).
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego,
 * @param[in] c                 – kod wczytanego klawisza.
 */
static void interactive_regular_key(struct interactive_model *m, int c) {
    if (ISNULL(m)) {
        return;
    }
    switch (c) {
        case 4:
            interactive_finish(m);
            break;
        case (int) 'G':
        case (int) 'g':
            interactive_move(m, gamma_golden_move, GOLD_COLOR, GOLD_COLOR_DARK);
            break;
        case (int) ' ':
            interactive_move(m, gamma_move, GREEN_COLOR, GREEN_COLOR_DARK);
            break;
        case (int) 'C':
        case (int) 'c':
            interactive_next_player(m);
            break;
        default:
            break;
    }
}


/** @brief Obsługa wciśnięcia strzałki.
 * @param[in, out] m            – wskaźnik na model trybu interaktywnego,
 * @param[in] c                 – kod wczytanego klawisza.
 */
static void interactive_arrow_key(struct interactive_model *m, int c) {
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
            interactive_regular_key(m, c);
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
    if (c == ESC_KEY && (c = getchar()) == BRAC_KEY) {
        // Wczytano dwa z trzech kodów sygnalizujących możliwość wystąpienia
        // strzałki w ANSI escape codes. Wczytanie trzeciego i przekazanie
        // do obsługi.
        c = getchar();
        interactive_arrow_key(m, c);
    } else {
        // Wciśnięcie normalnych znaków.
        interactive_regular_key(m, c);
    }
}


void interactive_run(gamma_t *g) {
    if (ISNULL(g)) {
        exit(EXIT_FAILURE);
    }
    struct interactive_model *m = &model;
    interactive_init(g, m);
    interactive_view(m, HIGHLINE_COLOR);
    while (true) {
        interactive_control(m);
        interactive_view(m, HIGHLINE_COLOR);
    }
}
