/** @file
 * Program obsługujący rozgrywkę w grę Gamma.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */
#include <stdlib.h>
#include "gamma.h"
#include "input_interface.h"
#include "batch_mode.h"
#include "interactive_mode.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


/** Silnik gry Gamma działający w ramach programu.
 */
static gamma_t *engine = NULL;


/** Zwolnienie zaalokowanych zasobów silnika Gamma.
 * Funckje należy wywołać pod koniec działania programu.
 */
static void finish_program() {
    gamma_delete(engine);
}


/** Główna funkcja programu Gamma. */
int main() {
    atexit(finish_program);
    uint32_t params[GAMMA_NEW_PARAMS_SIZE];
    int resp;
    char mode;
    while (true) {
        resp = parse_line(&mode, GAMMA_NEW_PARAMS_SIZE, params);
        if (resp == PARSE_END) {
            // Koniec standardowego wejścia.
            exit(EXIT_SUCCESS);
        } else if (resp == GAMMA_NEW_PARAMS_SIZE && (mode == 'I' || mode == 'B')) {
            engine = gamma_new(params[0], params[1], params[2], params[3]);
            if (!ISNULL(engine)) {
                // Deklaracja planszy jest poprawna.
                report_ok();
                break;
            } else {
                // Niepoprawne parametry planszy.
                report_error();
            }
        } else if (resp >= 0) {
            // Zła liczba parametrów lub nieodpowiedni tryb programu.
            report_error();
        }
    }
    switch (mode) {
        case 'B':
            // Przejście do trybu wsadowego.
            batch_run(engine);
            break;
        case 'I':
            // Przejście do trybu interaktywnego.
            interactive_run(engine);
            break;
        default:
            break;
    }
}
