#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <stdbool.h>


typedef struct field field_t;
typedef struct gamma gamma_t;

field_t **field_board_init(uint32_t width, uint32_t height);

uint32_t field_count_adjoining_areas(field_t *field, uint32_t player_id);

uint32_t field_count_adjoining_fields(field_t *field, uint32_t player);

uint32_t field_count_adjoining_areas_after_breaking(field_t *field);

void field_rebuild_areas_around(field_t *field, uint32_t player);

#endif //GAMMA_FIELD_H
