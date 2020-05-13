//
// Created by adam on 13.05.2020.
//

#ifndef GAMMA_MODEL_H
#define GAMMA_MODEL_H

#include "gamma.h"

typedef union {
    bool (*move_function)(gamma_t *, uint32_t, uint32_t, uint32_t);
    uint64_t (*fields_function)(gamma_t *, uint32_t);
    bool (*check_function)(gamma_t *, uint32_t);
    char *(*string_function)(gamma_t *);
} gamma_function_t;

#endif //GAMMA_GAMMA_MODEL_H
