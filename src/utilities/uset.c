#include "uset.h"

/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr - sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


void uset_init(uset_t *uset) {
    if (uset != NULL) {
        uset->repr = uset;
        uset->next = uset;
        uset->previous = uset;
        uset->size = 1;
    }
}


void uset_union(uset_t *uset1, uset_t *uset2) {
    if (ISNULL(uset1) || ISNULL(uset2) || uset1->repr == uset2->repr) {
        return;
    }
   if (uset_size(uset1) < uset_size(uset2)) {
       uset_union(uset2, uset1);
       return;
    }
    uset1->repr->size += uset_size(uset2);
    uset2->repr->size = 0;
    uset_t *curr = uset2;
    do {
        curr->repr = uset1->repr;
        curr = curr->next;
    } while (curr != uset2);

    uset_t *next1 = uset1->next;
    uset_t *prev2 = uset2->previous;

    uset1->next = uset2;
    uset2->previous = uset1;
    next1->previous = prev2;
    prev2->next = next1;
}


void uset_split(uset_t *uset) {

}
