/** @file
 * Interfejs klasy reprezentującej kolejkę.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.04.2020
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
    void *link; /**< Dodatkowy wskaźnik przechowywany w kolejce. */
} queue_node_t;


/** @brief Inicjuje pustą kolejkę.
 * @param[out] queue    – wskaźnik na inicjowaną kolejkę
 */
void queue_init(queue_t *queue);


/** @brief Inicjuje element kolejki.
 * @param[out] node     – wskaźnik inicjowany element,
 * @param[in] link      – wskaźnik na inicjowany element.
 */
void queue_init_node(queue_node_t *node, void *link);


/** @brief Dodanie elementu na początek kolejki.
 * @param[in, out] q    – wskaźnik do kolejki,
 * @param[in] node      – wskaźnik na element dodawany do kolejki, wcześniej
 *                        zainicjalizowany.
 */
void queue_put_front(queue_t *q, queue_node_t *node);


/** @brief Dodaje element na koniec kolejki.
 * @param[in, out] q    – wskaźnik do kolejki,
 * @param[in] node      – wskaźnik na element dodawany do kolejki, wcześniej
 *                        zainicjalizowany.
 */
void queue_put_back(queue_t *q, queue_node_t *node);


/** @brief Usunięcie elementu z kolejki.
 * Funkcja usuwa element z kolejki do której należy.
 * @param[in, out] node – wskaźnik na element należący do pewnej kolejki.
 * @return Wskaźnik przechowywany w elemencie kolejki @ref queue_node::link, lub
 * @p NULL jeżeli kolejka jest pusta.
 */
void *queue_delete_node(queue_node_t *node);


/** @brief Usunięcie początkowego elementu kolejki.
 * Funkcja usuwa z kolejki pierwszy element i zwraca związany z nim wskaźnik.
 * @param[in, out] q    – wskaźnik do kolejki.
 * @return Wskaźnik przechowywany w elemencie kolejki @ref queue_node::link, lub
 * @p NULL jeżeli kolejka jest pusta.
 */
static inline void *queue_pop_front(queue_t *q) {
    return q != NULL ? queue_delete_node(q->begin) : NULL;
}


/** @brief Test pustości kolejki.
 * @param[in] queue     – wskaźnik do kolejki.
 * @return Wartość @p true jeżeli w kolejce nie ma elementów, @p false
 * w przeciwnym wypadku.
 */
static inline bool queue_empty(const queue_t *queue) {
    return queue == NULL || (queue->begin == NULL && queue->end == NULL);
}


#endif /* QUEUE_H */
