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


/** Struktura przechowująca informacje o pojedyńczym polu w grze.
 */
typedef struct field field_t;


/** @brief Identyfikator gracza zajmującego pole.
 * @param[in] field         – wskaźnik na pole.
 * @return Funkcja zwraca identyfikator gracza, którego pionek zajmuje pole lub
 * `0` jeżeli pole jest wolne.
 */
uint32_t field_owner(field_t *field);


/** @brief Ustawienie identyfikatora gracza zajmującego pole.
 * @param[in, out] field    – wskaźnika na pole,
 * @param[in] new_owner     – identyfikator gracza lub `0` jeżeli pole ma zostać
 *                            zwolnione.
 */
void field_set_owner(field_t *field, uint32_t new_owner);


/** @brief Tablica sąsiedztwa pola.
 * Funkcja wpisuje do tablicy podanej jako parametr @p adjoining wskaźniki pól
 * z którymi sąsiaduje pole @p field. Jeżeli @p field ma mniej niż czterech
 * sąsiadów, reszta tablicy uzupełniana jest wartościami `NULL`.
 * @param[in] field         – wskaźnik na pole,
 * @param[out] adjoining    – tablica, do której zostaną zapisani sąsiedzi pola
 *                            @p field.
 */
void field_adjoining(field_t *field, field_t *adjoining[4]);


/** @brief Ilość pól sąsiadujących.
 * @param[in] field         – wskaźnik na pole.
 * @return Funkcja zwraca liczbę pól z którymi styka się pole @p field.
 */
uint32_t field_adjoining_size(field_t *field);


/** @brief Tworzy dwuwymiarową tablicę pól.
 * Funkcja tworzy i inicjuje dwuwymiarową tablicę pól reprezentującą planszę
 * gry Gamma.
 * @param[in] width         – ilość kolumn,
 * @param[in] height        – ilość wierszy.
 * @return Wskaźnik do utworzonej planszy lub `NULL` jeżeli alokacja się
 * nie powiodła.
 */
field_t ***field_board_new(uint32_t width, uint32_t height);


/** @brief Złączenie dwóch obszarów w jeden.
 * Funkcja złącza dwa obszary do których należą @p field1 i @p field2.
 * @param[in, out] field1   – wskaźnik na pierwsze pole,
 * @param[in, out] field2   – wskaźnik na drugie pole.
 */
void field_connect_area(field_t *field1, field_t *field2);


/** @brief Rozbicie obszaru na pojedyńcze pola.
 * W wyniku działania funkcji obszar do którego należy pole @p field zostaje
 * rozbity na obszary składające się z pojedyńczych pól.
 * @param[in, out] field    – wskaźnik na pole.
 */
void field_split_area(field_t *field);


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
