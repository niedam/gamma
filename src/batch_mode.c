/** @file
 * Implementacja trybu wsadowego umożliwiającego symulację rozgrywki przy użyciu
 * silnika gry Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "gamma.h"
#include "batch_mode.h"
#include "input_interface.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


/** Enumeratory typów funkcji obsługiwanych przez tryb wsadowy.
 */
enum function_signature {
    move_function, /**< Funkcja wykonuje ruch, zwraca wartość bool. */
    fields_function, /**< Funkcja zlicza pola, zwraca wartość uint32_t. */
    check_function, /**< Funckja dokonuje sprawdzenia, zwraca wartość bool. */
    string_function /**< Funkcja zwraca opis planszy (char *). */
};


/** Typy funkcji obsługiwanych przez tryb wsadowy.
 */
union function_type {
    bool (*move_function)(gamma_t *, uint32_t, uint32_t, uint32_t); /**< Funkcja wykonuje ruch, zwraca wartość bool. */
    uint64_t (*fields_function)(gamma_t *, uint32_t); /**< Funkcja zlicza pola, zwraca wartość uint32_t. */
    bool (*check_function)(gamma_t *, uint32_t); /**< Funckja dokonuje sprawdzenia, zwraca wartość bool. */
    char *(*string_function)(gamma_t *); /**< Funkcja zwraca opis planszy (char *). */
};


/** Struktura polecenia do wykonania w trybie wsadowym.
 */
struct batch_command {
    char command; /**< Znak polecenia. */
    int param_size; /**< Liczba parametrów. */
    enum function_signature signature; /**< Sygnatura funkcji związanej
                                        * z poleceniem. */
    union function_type fun; /**< Wskaźnik na funkcje. */
};


/** @brief Makro opisujące strukturę polecenia @ref batch_command.
 * @param[in] char_cmd          – znak polecenia,
 * @param[in] parsize           – liczba parametrów,
 * @param[in] funenum           – enumerator funkcji,
 * @param[in] funx              – wskaźnik na funkcję.
 */
#define BATCH_COMMAND(char_cmd, parsize, funenum, funx) \
    (struct batch_command) { \
        .command = char_cmd, \
        .param_size = parsize, \
        .signature = funenum, \
        .fun.funenum = funx \
    }


/** Polecenia dostępne w trybie wsadowym.
 */
static const struct batch_command commands[] = {
        BATCH_COMMAND('m', 3, move_function, gamma_move),
        BATCH_COMMAND('g', 3, move_function, gamma_golden_move),
        BATCH_COMMAND('b', 1, fields_function, gamma_busy_fields),
        BATCH_COMMAND('f', 1, fields_function, gamma_free_fields),
        BATCH_COMMAND('q', 1, check_function, gamma_golden_possible),
        BATCH_COMMAND('p', 0, string_function, gamma_board)
};


/** Rozpoznanie polecenia w trybie wsadowym.
 * @param[in] command           – znak polecenia.
 * @return Wskaźnik na strukturę opisującą procedurę do wykonania.
 */
static const struct batch_command *batch_command_select(char command) {
    size_t cmd_id;
    for (cmd_id = 0; cmd_id < sizeof(commands); cmd_id++) {
        if (commands[cmd_id].command == command) {
            return &commands[cmd_id];
        }
    }
    return NULL;
}


/** Wykonanie polecenia w trybie wsadowym.
 * @param[in, out] g            – wskaźnik na strukturę silnika gry,
 * @param[in] command           – wskaźnik na strukturę polecenia do wykonania,
 * @param[in] param_size        – liczba przekazanych w poleceniu parametrów,
 * @param[in] params            – liczbowe parametry polecenia.
 */
static void batch_command_run(gamma_t *g, const struct batch_command *command,
                              int param_size, const uint32_t params[param_size]) {
    if (ISNULL(g) || ISNULL(params) || ISNULL(command)
        || command->param_size != param_size) {
        report_error();
        return;
    }
    char *string;
    switch (command->signature) {
        case move_function:
            printf("%d\n", command->fun.move_function(g, params[0], params[1],
                   params[2]));
            break;
        case fields_function:
            printf("%ld\n", command->fun.fields_function(g, params[0]));
            break;
        case check_function:
            printf("%d\n", command->fun.check_function(g, params[0]));
            break;
        case string_function:
            string = command->fun.string_function(g);
            printf("%s", string);
            free(string);
            break;
        default:
            break;
    }
}


void batch_run(gamma_t *g) {
    if (ISNULL(g)) {
        return;
    }
    uint32_t param[3];
    char cmd;
    int resp;
    while (true) {
        resp = parse_line(&cmd, 3, param);
        if (resp == PARSE_END) {
            exit(EXIT_SUCCESS);
        } else if (resp != PARSE_ERROR && resp != PARSE_CONTINUE) {
            const struct batch_command *command = batch_command_select(cmd);
            batch_command_run(g, command, resp, param);
        }
    }
}
