#ifndef FIELD_STRUCT_H
#define FIELD_STRUCT_H

#include <stdint.h>
#include "utilities/queue.h"
#include "utilities/uset.h"

struct field {
    uint32_t x; /**< x */
    uint32_t y;
    struct field *adjoining[4];
    uint32_t size_adjoining;
    queue_node_t bfs;
    uset_t area;
    uint32_t owner;
    uint8_t visited;
};

#endif // FIELD_STRUCT_H
