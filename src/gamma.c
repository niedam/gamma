
#include <stdlib.h>
#include <stdio.h>
#include "gamma.h"
#include "player_struct.h"
#include "field.h"
#include "field_struct.h"

#define ISNULL(ptr) (ptr == NULL)



struct gamma {
    uint32_t height;
    uint32_t width;
    field_t **fields;
    uint32_t no_players;
    uint32_t areas_limit;
    player_t *players;
    uint64_t ocupied_fields;
};



static bool test_player(gamma_t *g, uint32_t player) {
    return !ISNULL(g) && player > 0 && player <= g->no_players;
}

static bool test_field(gamma_t *g, uint32_t x, uint32_t y) {
    return !ISNULL(g) && x < g->width && y < g->height;
}


static field_t *gamma_get_field(gamma_t *g, uint32_t x, uint32_t y) {
    return test_field(g, x, y) ? &g->fields[y][x] : NULL;
}

static player_t *gamma_get_player(gamma_t *g, uint32_t player) {
    return test_player(g, player) ? &g->players[player - 1] : NULL;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (width == 0 || height == 0 || players == 0 || areas == 0) {
        return NULL;
    }
    gamma_t *g = malloc(sizeof(struct gamma));
    if (ISNULL(g)) {
        return NULL;
    }
    g->players = calloc(sizeof(player_t), players);
    for (size_t i = 0; i < players; ++i) {
        g->players[i].id = i + 1;
        g->players[i].areas = 0;
        g->players[i].occupied_fields = 0;
        g->players[i].free_adjoining = 0;
        g->players[i].golden_move_done = false;
    }
    g->areas_limit = areas;
    /**Inicjacja informacji o graczach.
     */
    g->no_players = players;

    g->ocupied_fields = 0;
    /** Inicjacja pól na planszy.
     */
    g->width = width;
    g->height = height;
    g->fields = field_board_init(width, height);
    if (ISNULL(g->fields)) {
        free(g);
    }
    return g;
}


static void gamma_take_field(gamma_t *g, player_t *player, field_t *field) {
    if (ISNULL(g) || ISNULL(player) || ISNULL(field) || field->owner != 0) {
        return;
    }
    field->owner = player->id;
    g->ocupied_fields++;
    player->occupied_fields++;
    player->areas += 1 - field_count_adjoining_areas(field, player->id);
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner != 0) {
            size_t diff = 1;
            for (size_t j = i + 1; j < field->size_adjoining; ++j) {
                if (field->adjoining[i]->owner == field->adjoining[j]->owner) {
                    diff = 0;
                    break;
                }
            }
            player_t *current = gamma_get_player(g, field->adjoining[i]->owner);
            if (ISNULL(current)) {
                return;
            }
            current->free_adjoining -= diff;
        }
    }
    uint32_t adjoining;
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        adjoining = field_count_adjoining_fields(field->adjoining[i], player->id);
        if (field->adjoining[i]->owner == 0 && adjoining == 1) {
            player->free_adjoining++;
        } else if (field->adjoining[i]->owner == player->id) {
            uset_union(&field->adjoining[i]->area, &field->area);
        }
    }

}


bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    field_t *field = gamma_get_field(g, x, y);
    player_t *player_info = gamma_get_player(g, player);
    if (ISNULL(g) || ISNULL(field) || ISNULL(player_info) || field->owner != 0) {
        return false;
    }
    uint32_t my_adjoining_areas = field_count_adjoining_areas(field, player);
    if (player_info->areas == g->areas_limit && my_adjoining_areas == 0) {
        return false;
    }
    gamma_take_field(g, player_info, field);
    /*field->owner = player;
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner == 0) {
            continue;
        }
        size_t diff = 1;
        for (size_t j = i + 1; j < field->size_adjoining; ++j) {
            if (field->adjoining[i]->owner == field->adjoining[j]->owner) {
                diff = 0;
            }
        }
        gamma_get_player(g, field->adjoining[i]->owner)->free_adjoining -= diff;
    }
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner == 0
                && field_count_adjoining_fields(field->adjoining[i], player) == 1) {
            player_info->free_adjoining++;
        }

        if (field->adjoining[i]->owner == player) {
            uset_union(&field->adjoining[i]->area, &field->area);
        }
    }
    g->ocupied_fields++;
    player_info->occupied_fields++;
    player_info->areas += 1 - my_adjoining_areas;*/
    return true;
}


static void gamma_release_field(gamma_t *g, field_t *field) {
    if (ISNULL(g) || ISNULL(field)) {
        return;
    }
    player_t *owner = gamma_get_player(g, field->owner);
    if (ISNULL(owner)) {
        return;
    }
    field->owner = 0;
    uset_split(&field->area);
    field_rebuild_areas_around(field, owner->id);
    size_t diff;
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        if (field->adjoining[i]->owner != 0) {
            diff = 1;
            for (size_t j = i + 1; j < field->size_adjoining; ++j) {
                if (field->adjoining[i]->owner == field->adjoining[j]->owner) {
                    diff = 0;
                    break;
                }
            }
            player_t *current = gamma_get_player(g, field->adjoining[i]->owner);
            if (ISNULL(current)) {
                return;
            }
            current->free_adjoining += diff;
        }
    }
    uint32_t adjoining;
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        adjoining = field_count_adjoining_fields(field->adjoining[i], owner->id);
        if (field->adjoining[i]->owner == 0 && adjoining == 0) {
            owner->free_adjoining--;
        }
    }
    owner->areas -= 1 - field_count_adjoining_areas(field, owner->id);
    owner->occupied_fields--;
    g->ocupied_fields--;

}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (ISNULL(g) || !test_player(g, player) || !test_field(g, x, y)) {
        return false;
    }
    player_t *player_link = gamma_get_player(g, player);
    field_t *field = gamma_get_field(g, x, y);
    if (player_link->golden_move_done) {
        /* Złoty ruch został już wykonany przez tego gracza.
         */
        return false;
    }
    if (player_link->areas == g->areas_limit
        && field_count_adjoining_areas(field, player) == 0) {
        /* Gracz osiągnął limit obszarów i nie powiększy żadnego istniejącego.
        */
        return false;

    }
    player_t *owner = gamma_get_player(g, field->owner);
    size_t areas_after_breaking = field_count_adjoining_areas_after_breaking(field);
    if (g->areas_limit - owner->areas < 4 && g->areas_limit <
                                    areas_after_breaking - 1 + owner->areas) {
        /* Zdjęcie pionka innemu graczu stworzyłoby mu obszary ponad limit.
         */
        return false;
    }
    gamma_release_field(g, field);
    gamma_take_field(g, player_link, field);
    player_link->golden_move_done++;
    return true;
}


bool gamma_golden_move2(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (ISNULL(g) || !test_player(g, player) || !test_field(g, x, y)) {
        return false;
    }
    player_t *player_link = gamma_get_player(g, player);
    field_t *field = gamma_get_field(g, x, y);
    if (player_link->golden_move_done) {
        /** Złoty ruch został już wykonany przez tego gracza.
         */
        return false;
    }
    if (player_link->areas == g->areas_limit
            && field_count_adjoining_areas(field, player) == 0) {
        /** Gracz osiągnął limit obszarów i nie powiększy żadnego istniejącego.
        */
        return false;
    }
    player_t *owner = gamma_get_player(g, field->owner);
    size_t areas_after_breaking = field_count_adjoining_areas_after_breaking(field);
    if (g->areas_limit - owner->areas < 4 && g->areas_limit <
            field_count_adjoining_areas_after_breaking(field) - 1 + owner->areas) {
        /** Zdjęcie pionka innemu graczu stworzyłoby mu obszary ponad limit.
         */
        return false;
    }
    uint32_t my_adjoining_areas = field_count_adjoining_areas(field, player);
    player_link->areas += 1 - my_adjoining_areas;
    owner->areas += areas_after_breaking - 1;
    uset_split(&field->area);
    field->owner = player;
    field_rebuild_areas_around(field, owner->id);
    size_t my_fields;
    size_t former_owner_fields;
    for (size_t i = 0; i < field->size_adjoining; ++i) {
        my_fields = field_count_adjoining_fields(field->adjoining[i], player);
        former_owner_fields = field_count_adjoining_areas(field->adjoining[i], owner->id);
        if (field->adjoining[i]->owner == 0 && my_fields == 1) {
            player_link->free_adjoining++;
        }
        if (former_owner_fields == 0) {
            owner->free_adjoining--;
        }
    }
    player_link->occupied_fields++;
    owner->occupied_fields--;
    player_link->golden_move_done = true;
    return true;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (ISNULL(g) || !test_player(g, player)) {
        return 0;
    }
    return gamma_get_player(g, player)->occupied_fields;
}


uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    player_t *player_info = gamma_get_player(g, player);
    if (player_info->areas == g->areas_limit) {
        return player_info->free_adjoining;
    } else {
        return (uint64_t) g->width * g->height - g->ocupied_fields;
    }
}
