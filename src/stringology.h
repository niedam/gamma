/** @file
 * Biblioteka funkcji dotyczących tworzenia i obsługi stringów.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
 */

#ifndef STRINGOLOGY_H
#define STRINGOLOGY_H

#include <stdbool.h>
#include <stdint.h>


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
 *                             wolne,
 * @param[in] num_len        – ilość miejsca jaką ma zająć identyfikator w
 *                             buforze.
 * @return Ilość znaków wpisanych do bufora.
 */
int player_write(char *buff, int n, uint32_t player, uint32_t num_len);

bool char_check(char c, const char *char_set);

bool string_to_uint32(const char *string, uint32_t *result);

bool check_valid_line(const char *line);

bool check_blank_line(const char *line);

bool check_comment_line(const char *line);

#endif /* STRINGOLOGY_H */
