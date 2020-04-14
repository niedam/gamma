/** @file
 * Interfejs klasy reprezentującej złączalne zbiory.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
 */

#ifndef USET_H
#define USET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/** Struktura zbiorów rozłącznych implementowanych jako listy cykliczne
 * singletonów.
 * Aby używać struktury złączalnych zbiorów, należy zainicjować singletony,
 * które mają być w ramach niej złączane przy pomocy @ref uset_init()
 */
typedef struct uset {
    struct uset *next; /**< Następny singleton dołączony do zbioru. */
    struct uset *previous; /**< Poprzedni singleton dołączony do zbioru. */
    struct uset *repr; /**< Wskaźnik na reprezentanta zbioru. */
    uint64_t size; /**< Rozmiar zbioru (jeżeli singleton jest reprezentantem). */
} uset_t;


/** @brief Inicjuje złączalny singleton.
 * @param[out] uset     – wskaźnik na singleton do zainicjowania
 */
void uset_init(uset_t *uset);


/** @brief Zwraca rozmiar zbioru, do którego włączono singleton.
 * @param[in] uset      – wskaźnik na singleton
 * @return Rozmiar zbioru, lub @p 0 gdy argument @p uset jest `NULL`-em.
 */
static inline uint64_t uset_size(const uset_t *uset) {
    return uset != NULL ? uset->repr->size : 0;
}


/** @brief Sprawdza czy dwa singletony są podzbioram jednego zbioru.
 * @param[in] uset1     – wskaźnik na pierwszy singleton,
 * @param[in] uset2     – wskaźnik na drugi singleton
 * @return Wartość @p true jeżeli elementy należą do jednego zbioru, @p false
 * w przeciwnym wypadku lub gdy któryś singleton jest `NULL`-em.
 */
static inline bool uset_test(const uset_t *uset1, const uset_t *uset2) {
    return uset1 != NULL && uset2 != NULL && uset1->repr == uset2->repr;
}


/** @brief Złączenie dwóch zbiorów.
 * Funkcja złącza dwa zbiory, których podzbiorami są singletony podane
 * jako parametry.
 * @param[in, out] uset1 – wskaźnik na pierwszy singleton,
 * @param[in, out] uset2 – wskaźnik na drugi singleton
 */
void uset_union(uset_t *uset1, uset_t *uset2);


/** @brief Rozbicie zbioru na singletony.
 * Funkcja rozbija zbiór do którego należy wskazany singleton na niezależne
 * singletony.
 * @param[in, out] uset  – wskaźnik na singleton
 */
void uset_split(uset_t *uset);


#endif /* USET_H */
