/** @file
 * Biblioteka funkcji dotyczących tworzenia i obsługi stringów.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

#ifndef STRINGOLOGY_H
#define STRINGOLOGY_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/param.h>


/** @brief Długość zapisu dziesiętnego liczby.
 * @param[in] number        – liczba.
 * @return Długość zapisu dziesiętnego podanej liczby.
 */
int uint64_length(uint64_t number);


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


/** @brief Sprawdzenie obecności znaku w słowie.
 * Funkcja sprawdza, czy znak @p c należy do słowa @p char_set.
 * @param[in] c             – sprawdzany znak,
 * @param[in] char_set      – słowo do przeszukania.
 * @return Wartość @p true jeżeli @p c należy do @p char_set lub @p w przeciwnym
 * wypadku oraz gdy wskaźnik na słowo jest niepoprawny.
 */
bool char_check(char c, const char *char_set);


/** @brief Interpretuje ciąg znaków jako liczbę.
 * @param[in] string        – interpretowane słowo,
 * @param[out] result       – wskaźnik do miejsca w pamięci, w którym ma zostać
 *                            zapisany wynik konwersji.
 * @return Wartość @p true, jeżeli konwersja przebiegła pomyślnie i do @p result
 * przypisano liczbę ze @p result, wartość @p false jeżeli konwersja się
 * nie powiodła lub podane parametry są niepoprawne.
 */
bool string_to_uint32(const char *string, uint32_t *result);


/** @brief Sprawdzenie poprawności wczytanego wiersza.
 * @param[in] line          – wczytany wiersz,
 * @param[in] len           – długość wczytanego wiersza.
 * @return Wartość @p true, jeżeli wczytany wiersz jest poprawny,
 * wartość @p false jeżeli wczytany wiersz jest niepoprawny lub podano
 * nieprawidłowy wskaźnik na wiersz.
 */
bool check_valid_line(const char *line, ssize_t len);


/** @brief Sprawdzenie czy wiersz jest pusty.
 * @param[in] line          – wczytany wiersz.
 * @return Wartość @p true, jeżeli podany wiersz zawiera jedynie znak końca
 * linii, wartość @p false w przeciwnym wypadku lub gdy podany parametr jest
 * niepoprawny.
 */
bool check_blank_line(const char *line);


/** @brief Sprawdzenie czy wiersz jest komentarzem.
 * @param[in] line          – wczytany wiersz.
 * @return Wartość @p true, jeżeli podany wiersz zaczyna się od znaku `#`,
 * wartość @p false w przeciwnym wypadku lub gdy podany parametr jest
 * niepoprawny.
 */
bool check_comment_line(const char *line);


#endif /* STRINGOLOGY_H */
