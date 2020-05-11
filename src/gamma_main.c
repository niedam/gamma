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
            report_ok();
            break;
        }
    }
    global.game = gamma_new(params[0], params[1],
                            params[2], params[3]);
    switch (mode) {
        case 'B':
            batch_run(global.game);
            break;
        case 'I':
        default:
            break;
    }
}