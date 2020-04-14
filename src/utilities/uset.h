/** @file
 * Interfejs klasy reprezentującej złączalne zbiory.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.04.2020
 */

#ifndef USET_H
#define USET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** Struktura zbiorów rozłącznych implementowanych jako listy cykliczne.
 */
typedef struct uset {
    struct uset *next; /**< Następny element należący do zbioru. */
    struct uset *previous; /**< Poprzedni element należący do zbioru. */
    struct uset *repr; /**< Wskaźnik na reprezentanta zbioru. */
    uint64_t size; /**< Rozmiar zbioru (jeżeli element jest reprezentantem). */
} uset_t;


/** @brief Inicjuje złączalny singleton.
 * @param[out] uset - element to zainicjowania
 */
void uset_init(uset_t *uset);


/** @brief Zwraca rozmiar zbioru, do którego należy element.
 * @param[in] uset - wskaźnik na element zbioru złączalnego
 * @return Rozmiar zbioru, lub @p 0 gdy argument uset jest `NULL`-em.
 */
static inline uint64_t uset_size(const uset_t *uset) {
    return uset != NULL ? uset->repr->size : 0;
}


/** @brief Sprawdza czy dwa elementy należą do jednego zbioru.
 * @param[in] uset1 - pierwszy element zbioru złączalnego
 * @param[in] uset2 - drugi element zbioru złączalnego
 * @return @p true - jeżeli elementy należą do jednego zbioru,
 *         @p false - w przeciwnym wypadku lub gdy któryś argument jest `NULL`-em.
 */
static inline bool uset_test(const uset_t *uset1, const uset_t *uset2) {
    return uset1 != NULL && uset2 != NULL && uset1->repr == uset2->repr;
}


/** @brief Złączenie dwóch zbiorów.
 * Funkcja złącza zbiory do których należą podane jako parametry elementy w jeden.
 * @param[in, out] uset1 - pierwszy element zbioru złączalnego
 * @param[in, out] uset2 - drugi element zbioru złączalnego
 */
void uset_union(uset_t *uset1, uset_t *uset2);


/** @brief Rozbicie zbioru na singletony.
 * Funkcja rozbija zbiór do którego należy element na singletony.
 * @param uset - element zbioru złączalnego
 */
void uset_split(uset_t *uset);


#endif
