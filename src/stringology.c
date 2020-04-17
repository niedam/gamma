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


int player_print(char *buff, int n, uint32_t player) {
    if (player == 0) {
        /** Jeżeli @p player jest równe \( 0 \) to znaczy że pole jest wolne
         * i do bufora tekstowego zostaje zapisana kropka "`.`".
         */
        return snprintf(buff, n, ".");
    } else if (player < 10) {
        /** Jeżeli identyfikator @p player jest mniejszy od \( 10 \) to można
         * go w czytelny sposób wypisać jako pojedyńczą cyfrę.
         */
        return snprintf(buff, n, "%d", player);
    } else {
        /** Jeżeli identyfikator jest równy lub większy od \( 10 \) to do jego
         * zapisu potrzebne są nawiasy. Np. `[42]` - zapis gracza o
         * identyfikatorze \( 42 \).
         */
        return snprintf(buff, n, "[%d]", player);
    }
}