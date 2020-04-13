#ifndef USET_H
#define USET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct uset {
    struct field *field;
    struct uset *next;
    struct uset *previous;
    struct uset *repr;
    uint64_t size;
} uset_t;


static inline uint64_t uset_size(uset_t *uset) {
    return uset != NULL ? uset->repr->size : 0;
}


void uset_init(uset_t *uset);


static inline bool uset_test(uset_t *uset1, uset_t *uset2) {
    return uset1 == NULL || uset2 == NULL || uset1->repr == uset2->repr;
}


void uset_union(uset_t *uset1, uset_t *uset2);


void uset_split(uset_t *uset);


#endif
