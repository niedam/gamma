#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "stringology.h"


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