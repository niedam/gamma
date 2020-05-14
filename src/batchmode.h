/** @file
 * Nagłówek modułu implementującego tryb wsadowy programu symulującego
 * rozgrywkę przy użyciu silnika Gamma.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

#ifndef BATCHMODE_H
#define BATCHMODE_H


/** Struktura przechowująca stan gry.
 */
typedef struct gamma gamma_t;


/** @brief Uruchomienie i przejście do trybu wsadowego.
 * @param[in, out] g            – wskaźnik na strukturę silnika gry Gamma.
 */
void batch_run(gamma_t *g);


#endif /* BATCHMODE_H */
