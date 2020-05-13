#include <stdlib.h>
#include "gamma.h"
#include "input_interface.h"
#include "batchmode.h"
#include "interactivemode.h"

#define ISNULL(ptr) (ptr == NULL)

static struct {
    gamma_t *game;
} global = { .game = NULL };

static void finish_program() {
    gamma_delete(global.game);
    global.game = NULL;
}

static bool check_gamma_new(const uint32_t params[]) {
    if (ISNULL(params)) {
        return false;
    }
    for (size_t i = 0; i < 4; i++) {
        if (params[i] == 0) {
            return false;
        }
    }
    return true;
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
            if (check_gamma_new(params)) {
                report_ok();
                break;
            } else {
                report_error();
            }
        } else if (resp >= 0) {
            report_error();
        }
    }
    global.game = gamma_new(params[0], params[1],
                            params[2], params[3]);
    switch (mode) {
        case 'B':
            batch_run(global.game);
            break;
        case 'I':
            interactive_run(global.game);
            break;
        default:
            break;
    }
}