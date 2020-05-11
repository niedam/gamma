#define  _GNU_SOURCE
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "gamma.h"
#include "stringology.h"
#include "batchmode.h"
#include "input_interface.h"

#define ISNULL(ptr) (ptr == NULL)

#define WHITE_SPACES " \t\v\f\r\n"
#define NUMBER_CHAR "0123456789"


struct batch_command {
    char command;
    int param_size;
    enum function_signature {
        move_function,
        fields_function,
        check_function,
        string_function
    } signature;
    union {
        bool (*move_function)(gamma_t *, uint32_t, uint32_t, uint32_t);
        uint64_t (*fields_function)(gamma_t *, uint32_t);
        bool (*check_function)(gamma_t *, uint32_t);
        char *(*string_function)(gamma_t *);
    };
};


#define BATCH_COMMAND(char_cmd, parsize, funsign, funx) \
    (struct batch_command) { \
        .command = char_cmd, \
        .param_size = parsize, \
        .signature = funsign, \
        .funsign = funx \
    }


static const struct batch_command commands[] = {
        BATCH_COMMAND('m', 3, move_function, gamma_move),
        BATCH_COMMAND('g', 3, move_function, gamma_golden_move),
        BATCH_COMMAND('b', 1, fields_function, gamma_busy_fields),
        BATCH_COMMAND('f', 1, fields_function, gamma_free_fields),
        BATCH_COMMAND('q', 1, check_function, gamma_golden_possible),
        BATCH_COMMAND('p', 0, string_function, gamma_board)
};


static void batch_command_run(gamma_t *g, char command, int param_size, const uint32_t params[param_size]) {
    if (ISNULL(g) || ISNULL(params)) {
        return;
    }
    size_t cmd_id;
    for (cmd_id = 0; cmd_id < sizeof(commands); cmd_id++) {
        if (commands[cmd_id].command == command) {
            break;
        }
    }
    if (cmd_id >= sizeof(commands) || param_size != commands[cmd_id].param_size) {
        print_error();
        return;
    }
    switch (commands[cmd_id].signature) {
        case move_function:
            printf("%d\n", commands[cmd_id].move_function(g, params[0], params[1], params[2]));
            break;
        case fields_function:
            printf("%" PRId64 "\n", commands[cmd_id].fields_function(g, params[0]));
            break;
        case check_function:
            printf("%d", commands[cmd_id].check_function(g, params[0]));
            break;
        case string_function:
            printf("%s", commands[cmd_id].string_function(g));
            break;
        default:
            break;
    }
}


void batch_run(gamma_t *g) {
    if (ISNULL(g)) {
        return;
    }
    uint32_t param[3];
    char cmd;
    int resp;
    while (true) {
        resp = parse_line(&cmd, 3, param);
        if (resp == PARSE_END) {
            exit(EXIT_SUCCESS);
        } else if (resp == PARSE_ERROR) {
            print_error();
        } else {
            batch_command_run(g, cmd, resp, param);
        }
    }
}