#ifndef GAMMA_INPUT_INTERFACE_H
#define GAMMA_INPUT_INTERFACE_H

#include <stdint.h>

#define PARSE_ERROR -1
#define PARSE_END -2

typedef struct gamma gamma_t;

void print_ok();

void print_error();

int parse_line(char *cmd, int params_size, uint32_t params[params_size]);


#endif /* INPUT_INTERFACE_H */
