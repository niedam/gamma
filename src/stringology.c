/** @file
 * Implementacja funkcji do przetwarzania i obsługi ciągów znaków.
 *
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 17.05.2020
 */

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <errno.h>
#include "stringology.h"


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


int uint64_length(uint64_t number)  {
    if (number == 0) {
        return 1;
    }
    int result = 0;
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


bool string_to_uint32(const char *string, uint32_t *result) {
    if (ISNULL(string) || ISNULL(result)) {
        return false;
    }
    for (int i = 0; string[i] != '\0'; ++i) {
        if (!isdigit(string[i])) {
            return false;
        }
    }
    unsigned long conversion = strtoul(string, NULL, 10);
    if ((conversion == ULONG_MAX && errno == ERANGE) || conversion > UINT32_MAX) {
        return false;
    }
    *result = (uint32_t) conversion;
    return true;
}


bool check_valid_line(const char *line, ssize_t len) {
    /** Funkcja sprawdza, czy wiersz kończy się znakiem `\n`,
     * lub czy nie występują w nim nadmiarowe znaki `\0`.
     */
    if (ISNULL(line)) {
        return false;
    }
    for (ssize_t i = 0; i < len; i++) {
        if (line[i] == '\0') {
            return false;
        }
    }
    if (line[len - 1] == '\n') {
        return true;
    } else {
        return false;
    }
}


bool check_blank_line(const char *line) {
    return !ISNULL(line) && line[0] == '\n';
}


bool check_comment_line(const char *line) {
    return !ISNULL(line) && line[0] == '#';
}
