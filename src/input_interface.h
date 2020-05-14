/** @file
 * Moduł zapewniający obsługę standardowego wejścia oraz komunikaty o sukcesach
 * oraz błędach.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

#ifndef INPUT_INTERFACE_H
#define INPUT_INTERFACE_H

#include <stdint.h>


/** Stała oznaczająca wystąpienie ignorowanego wiersza na wejściu.
 * Zwracana przez @ref parse_line.
 */
#define PARSE_CONTINUE -1


/** Stała oznaczająca wystąpienie błędnego wiersza na wejściu.
 * Zwracana przez @ref parse_line.
 */
#define PARSE_ERROR -2


/** Stała oznaczająca zakończenie wczytywania standardowego wejścia.
 * Zwracana przez @ref parse_line.
 */
#define PARSE_END -3


/** Struktura przechowująca stan gry.
 */
typedef struct gamma gamma_t;


/** @brief Wypisuje na standardowe wyjście komunikat o sukcesie.
 */
void report_ok();


/** @brief Wypisuje na wyjście diagnostyczne informacje o niepowodzeniu.
 */
void report_error();


/** @brief Parsowanie wierszy ze standardowego wejścia.
 * @param[out] cmd          – wskaźnik na miejsce w pamięci w które funkcja
 *                            ma zapisać znak oznaczający wczytane polecenie,
 * @param[in] params_size   – maksymalna oczekiwana liczba parametrów,
 * @param[out] params       – tablica do której funkcja ma zapisać wczytane
 *                            liczby podane jako argumenty.
 * @return Liczba parametrów w podanym poleceniu lub: @ref PARSE_CONTINUE jeżeli
 * wiersz został zignorowany, @ref PARSE_ERROR jeżeli wiersz zawierał błąd,
 * @ref PARSE_END jeżeli zakończono wczytywanie ze standardowego wejścia.
 */
int parse_line(char *cmd, int params_size, uint32_t params[params_size]);


#endif /* INPUT_INTERFACE_H */
