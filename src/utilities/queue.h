/** @file
 * Interfejs klasy reprezentującej kolejkę.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.04.2020
 * @defgroup queue Kolejka
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>


typedef struct queue_node queue_node_t;


/** Struktura kolejki.
 */
typedef struct queue {
    queue_node_t *begin; /**< Wskaźnik na aktualny początek kolejki. */
    queue_node_t *end; /**< Wskaźnik na aktualny koniec kolejki. */
} queue_t;


/** Struktura elementu kolejki.
 */
typedef struct queue_node {
    queue_t *queue; /**< Wskaźnik na kolejkę do której należy element. */
    queue_node_t *next; /**< Następny element w kolejce. */
    queue_node_t *previous; /**< Poprzedni element w kolejce. */
    void *link; /**< Dowiązanie do elementu kolejki. */
} queue_node_t;

/** @brief Inicjuje pustą kolejkę.
 * @param[out] queue - wskaźnik na kolejkę do zainicjowania
 */
void queue_init(queue_t *queue);


/** @brief Inicjuje element kolejki.
 * @param[out] node - wskaźnik na element do zainicjowania
 * @param[in] link - wskaźnik do pamięci związanej z inicjowanym elementem
 */
void queue_init_node(queue_node_t *node, void *link);


void queue_put_front(queue_t *q, queue_node_t *node);


/** @brief Dodaje element na koniec kolejki.
 * @param[in, out] q - kolejka do której dodawany jest element
 * @param[in] node - dodawany element
 */
void queue_put_back(queue_t *q, queue_node_t *node);


/** @brief Usunięcie elementu z kolejki.
 * Funkcja usuwa element z kolejki do której ten należy.
 * @param[in, out] node - wskaźnik na element
 * @return Wskaźnik //TODO
 */
void *queue_delete_node(queue_node_t *node);


/** @brief Usunięcie początkowego elementu kolejki.
 * Funkcja usuwa z kolejki pierwszy element i zwraca // Todo
 * @param queue - kolejka
 * @return
 */
static inline void *queue_pop_front(queue_t *queue) {
    return queue_delete_node(queue->begin);
}


/** @brief Test pustości kolejki.
 * @param queue - kolejka do sprawdzenia
 * @return @p true - jeżeli w kolejce nie ma elementów,
 *         @p false - w przeciwnym wypadku
 */
static inline bool queue_empty(const queue_t *queue) {
    return queue->begin == NULL && queue->end == NULL;
}


#endif // QUEUE_H
