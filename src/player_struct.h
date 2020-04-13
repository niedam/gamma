#ifndef PLAYER_STRUCT_H
#define PLAYER_STRUCT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct player player_t;

struct player {
    uint32_t id;
    bool golden_move_done;
    uint64_t occupied_fields;
    uint64_t free_adjoining;
    uint32_t areas;
};


#endif // PLAYER_STRUCT_H
