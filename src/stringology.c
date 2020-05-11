#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <values.h>
#include <errno.h>
#include "stringology.h"

#define ISNULL(ptr) (ptr == NULL)

#define WHITE_SPACES " \t\v\f\r\n"
#define NUMBER_CHAR "0123456789"

uint32_t uint32_length(uint32_t number)  {
    if (number == 0) {
        return 1;
    }
    uint32_t result = 0;
    while (number > 0) {
        result++;
        number /= 10;
    }
    return result;
}


int player_write(char *buff, int n, uint32_t player, uint32_t num_len) {
    if (player == 0) {
        /** Jeżeli @p player jest równe `0` to znaczy że pole jest wolne
         * i do bufora tekstowego zostaje zapisana kropka `.`.
         */
        return snprintf(buff, n, "%*s", num_len, ".");
    } else {
        /** Dla @p player większego od zera liczba zostaje zapisana do bufora.
         */
        return snprintf(buff, n, "%*d", num_len, player);
    }
}


bool char_check(char c, const char *char_set) {
    if (ISNULL(char_set)) {
        return false;
    }
    int i = 0;
    while (char_set[i] != '\0') {
        if (char_set[i] == c) {
            return true;
        }
        i++;
    }
    return false;

}


bool string_to_uint32(const char *string, uint32_t *result) {
    if (ISNULL(string) || ISNULL(result)) {
        return false;
    }
    if (strspn(string, NUMBER_CHAR) != strlen(string)) {
        return false;
    }
    unsigned long conversion = strtoul(string, NULL, 10);
    if ((conversion == ULONG_MAX && errno == ERANGE) || conversion > UINT32_MAX) {
        return false;
    }
    *result = (uint32_t) conversion;
    return true;
}


bool check_valid_line(const char *line) {
    return !ISNULL(line) && line[strlen(line) - 1] == '\n';
}


bool check_blank_line(const char *line) {
    return !ISNULL(line) && strspn(line, WHITE_SPACES) == strlen(line);
}


bool check_comment_line(const char *line) {
    return !ISNULL(line) && line[0] == '#';
}