/** @file
 * Nagłówek modułu implementującego tryb interaktywny programu symulującego
 * rozgrywkę przy użyciu silnika Gamma.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

#ifndef INTERACTIVEMODE_H
#define INTERACTIVEMODE_H


/** Struktura przechowująca stan gry.
 */
typedef struct gamma gamma_t;


/** @brief Uruchomienie i przejście do trybu interaktywnego.
 * @param[in, out] g            – wskaźnik na strukturę silnika gry Gamma.
 */
void interactive_run(gamma_t *g);


#endif /* GAMMA_INTERACTIVEMODE_H */
