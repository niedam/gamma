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
#include "utilities/uset.h"


/** Struktura pola planszy w grze Gamma.
 * Aby korzystać z pola należy go odpowiednio zainicjować.
 * Tablica zainicjowanych pól jest tworzona w funkcji @ref field_board_new()
 * i jest to jedyny zalecany sposób uzyskiwania struktury pól.
 * Korzystanie z niezainicjowanych pól może mieć nieokreślone skutki.
 */
typedef struct field {
    struct field *adjoining[4]; /**< Tablica wszystkich sąsiadujących pól. */
    uint32_t size_adjoining; /**< Ilość sąsiadujących pól. */
    queue_node_t bfs; /**< Węzeł potrzebny do zorganizowania kolejki do algorytmu BFS. */
    uset_t area; /**< Obszar do którego należy pole. */
    uint32_t owner; /**< Identyfikator właściciela pola. */
    bool visited; /**< Informacja o tym czy odwiedzono pole algorytmem BFS. */
} field_t;


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
