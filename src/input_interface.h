#ifndef GAMMA_INPUT_INTERFACE_H
#define GAMMA_INPUT_INTERFACE_H

#include <stdint.h>

#define PARSE_CONTINUE -1
#define PARSE_ERROR -2
#define PARSE_END -3

typedef struct gamma gamma_t;

void report_ok();

void report_error();

int parse_line(char *cmd, int params_size, uint32_t params[params_size]);


#endif /* INPUT_INTERFACE_H */
