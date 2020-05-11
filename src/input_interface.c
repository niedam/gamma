#define  _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include "input_interface.h"
#include "stringology.h"
#include "gamma.h"

#define ISNULL(ptr) (ptr == NULL)

#define WHITE_SPACES " \t\v\f\r\n"
#define INITIAL_BUFFER_SIZE 32

static struct {
    int count_read_lines;
    size_t buffer_size;
    ssize_t line_length;
    char *buffer;
} global = { .buffer_size = 0, .line_length = 0, .count_read_lines = 0, .buffer = NULL };


void print_error() {
    fprintf(stderr, "ERROR %d\n", global.count_read_lines);
}

void print_ok() {
    printf("OK %d\n", global.count_read_lines);
}

static void finish_program() {
    free(global.buffer);
    global.buffer_size = 0;
    global.buffer = NULL;
}

static void init_buffer() {
    if (ISNULL(global.buffer) && global.buffer_size == 0) {
        global.buffer_size = INITIAL_BUFFER_SIZE;
        global.buffer = malloc(sizeof(char) * global.buffer_size);
        if (ISNULL(global.buffer)) {
            exit(EXIT_FAILURE);
        }
        atexit(finish_program);
    }
}


int parse_line(char *cmd, int params_size, uint32_t params[params_size]) {
    if (ISNULL(params) || ISNULL(cmd)) {
        return PARSE_ERROR;
    }
    global.count_read_lines++;
    init_buffer();
    global.line_length = getline(&global.buffer, &global.buffer_size, stdin);
    if (global.line_length == -1 && errno == ENOMEM) {
        exit(EXIT_FAILURE);
    } else if (global.line_length < 0) {
        return -2;
    }
    if (!check_valid_line(global.buffer)) {
        // Linia niezakończona `\n`.
        print_error();
        return PARSE_ERROR;
    }
    if (check_blank_line(global.buffer) || check_comment_line(global.buffer)) {
        // Komentarz lub pusty wiersz.
        return PARSE_ERROR;
    }
    char *str_mode = strtok(global.buffer, WHITE_SPACES);
    if (ISNULL(str_mode) && strlen(str_mode) != 1) {
        print_error();
        return PARSE_ERROR;
    }
    int result = 0;
    for (int i = 0; i < params_size; i++) {
        char *str = strtok(NULL, WHITE_SPACES);
        if (ISNULL(str)) {
            break;
        }
        result++;
        if (!string_to_uint32(str, &params[i])) {
            print_error();
            return PARSE_ERROR;
        }
    }
    if (strtok(NULL, WHITE_SPACES) != NULL) {
        print_error();
        return PARSE_ERROR;
    }
    *cmd = str_mode[0];
    return result;
}