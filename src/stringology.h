/** @file Biblioteka funkcji dotyczących tworzenia i obsługi stringów.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
 */

#ifndef STRINGOLOGY_H
#define STRINGOLOGY_H


/** @brief Długość zapisu dziesiętnego liczby.
 * @param[in] number        – liczba.
 * @return Długość zapisu dziesiętnego podanej liczby.
 */
uint32_t uint32_length(uint32_t number);


/** @brief Wypisanie gracza do bufora tekstowego.
 * Funkcja wypisuje identyfikator gracza do bufora.
 * @param[out] buff          – wskaźnik na bufor tekstowy,
 * @param[in] n              – długość bufora,
 * @param[in] player         – identyfikator gracza lub @p 0 jeżeli pole jest
 *                             wolne.
 * @return Ilość znakóœ wpisanych do bufora.
 */
int player_print(char *buff, int n, uint32_t player);

#endif /* STRINGOLOGY_H */
