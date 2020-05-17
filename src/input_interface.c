/** @file
 * Implementacja obsługi standardowego wejścia oraz wypisywania komunikatów.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

/** Makro umożliwiające używanie funkcji `getline()`.
 */
#define  _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include "input_interface.h"
#include "stringology.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


/** Stała reprezentująca ciąg białych znaków.
 */
#define WHITE_SPACES " \t\v\f\r\n"


/** Początkowa wielkość bufora wczytywanych danych.
 */
#define INITIAL_BUFFER_SIZE 32


/** Anonimowa struktura przechowująca zmienne bufora.
 */
static struct {
    int count_read_lines;
    size_t buffer_size;
    ssize_t line_length;
    char *buffer;
} global = { .buffer_size = 0, .line_length = 0, .count_read_lines = 0, .buffer = NULL };


void report_error() {
    fprintf(stderr, "ERROR %d\n", global.count_read_lines);
}


void report_ok() {
    printf("OK %d\n", global.count_read_lines);
}


/** @brief Zwolnienie zasobów związanych z buforem.
 * Funkcje należy wywołać przed zakończeniem programu.
 */
static void finish_program() {
    free(global.buffer);
    global.buffer_size = 0;
    global.buffer = NULL;
}


/** @brief Zainicjowanie bufora.
 */
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
        return PARSE_END;
    }
    if (check_blank_line(global.buffer) || check_comment_line(global.buffer)) {
        // Komentarz lub pusty wiersz.
        return PARSE_CONTINUE;
    }
    if (!check_valid_line(global.buffer, global.line_length)) {
        // Wiersz niepoprawny.
        report_error();
        return PARSE_ERROR;
    }
    if (isspace(global.buffer[0]) || !isalpha(global.buffer[0])) {
        report_error();
        return PARSE_ERROR;
    }
    char *str_mode = strtok(global.buffer, WHITE_SPACES);
    size_t size = strlen(str_mode);
    if (ISNULL(str_mode) || size != 1) {
        report_error();
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
            report_error();
            return PARSE_ERROR;
        }
    }
    if (strtok(NULL, WHITE_SPACES) != NULL) {
        report_error();
        return PARSE_ERROR;
    }
    *cmd = str_mode[0];
    return result;
}
