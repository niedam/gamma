/** @file
 * Definicja pola na planszy gry Gamma oraz deklaracja związanych z nim funkcji.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
 */

#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <stdbool.h>
#include "utilities/queue.h"

typedef struct field field_t;

struct adjoining {
    field_t **adjoining;
    uint32_t size;
};

uint32_t field_owner(field_t *field);

void field_set_owner(field_t *field, uint32_t new_owner);

struct adjoining field_adjoining(field_t *field);

uint64_t field_area_size(field_t *field);

void field_connect_areas(field_t *field1, field_t *field2);

void field_split_area(field_t *field);

field_t *field_at_array(field_t **array, uint32_t x, uint32_t y);


/** @brief Tworzy dwuwymiarową tablicę pól.
 * Funkcja tworzy i inicjuje dwuwymiarową tablicę pól reprezentującą planszę
 * gry Gamma.
 * @param[in] width         – ilość kolumn,
 * @param[in] height        – ilość wierszy.
 * @return Wskaźnik do utworzonej planszy lub `NULL` jeżeli alokacja się
 * nie powiodła.
 */
field_t **field_board_new(uint32_t width, uint32_t height);


/** @brief Zliczenie sąsiednich obszarów należących do danego gracza.
 * Funkcja zlicza przylegające do danego pola obszary, które są własnością
 * danego gracza.
 * @param[in] field         – wskaźnik na pole,
 * @param[in] player_id     – identyfikator gracza.
 * @return Liczba obszarów przylegających do pola `field` należących do gracza
 * o identyfikatorze @p player_id, lub `0` gdy któryś parametr jest niepoprawny.
 */
uint32_t field_count_adjoining_areas(const field_t *field, uint32_t player_id);


/** @brief Zliczenie sąsiednich pól należących do danego gracza.
 * Funkcja zlicza pola przylegające do danego pola, które są własnością danego
 * gracza.
 * @param[in] field         – wskaźnik pola,
 * @param[in] player_id     – identyfikator gracza.
 * @return Liczba pól przylegających do pola `field` należących do gracza
 * o identyfikatorze @p player_id, lub `0` gdy któryś parametr jest niepoprawny.
 */
uint32_t field_count_adjoining_fields(const field_t *field, uint32_t player_id);


/** @brief Zliczenie obszarów, które powstaną po rozbiciu obszaru.
 * Funkcja podaje ile nowych obszarów powstanie po ściągnięciu pionka
 * zajmującego dane pole planszy.
 * @param[in, out] field    – wskaźnik pola.
 * @return Liczba powstałych obszarów przylegających do pola @p field należących
 * do gracza, którego pionek znajduje się na tym polu.
 */
uint32_t field_count_adjoining_areas_after_breaking(field_t *field);


/** @brief Łączy sąsiadujące pola danego gracza w obszary.
 * Procedura konstruuje obszary tworzone przez pola gracza o identyfikatorze
 * @p player_id sąsiadujące z polem @p field.
 * @param[in, out] field    – wskaźnik pola,
 * @param[in] player_id     – identyfikator gracza.
 */
void field_rebuild_areas_around(field_t *field, uint32_t player_id);

#endif /* FIELD_H */
