#include <stdlib.h>
#include "gamma.h"
#include "input_interface.h"
#include "batchmode.h"


static struct {
    gamma_t *game;
} global = { .game = NULL };

void finish_program() {
    gamma_delete(global.game);
    global.game = NULL;
}

int main() {
    atexit(finish_program);
    uint32_t params[GAMMA_PARAMS_SIZE];
    int resp;
    char mode;
    while (true) {
        resp = parse_line(&mode, GAMMA_PARAMS_SIZE, params);
        if (resp == PARSE_END) {
            exit(EXIT_SUCCESS);
        } else if (resp == 4 && (mode == 'I' || mode == 'B')) {
            print_ok();
            break;
        } else {
            print_error();
        }
    }
    global.game = gamma_new(params[gamma_width], params[gamma_height],
                            params[gamma_players], params[gamma_areas]);
    switch (mode) {
        case 'B':
            batch_run(global.game);
            break;
        case 'I':
        default:
            break;
    }
}