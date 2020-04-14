#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "stringology.h"


size_t uint32_length(uint32_t number)  {
    if (number == 0) {
        return 1;
    }
    size_t result = 0;
    while (number > 0) {
        result++;
        number /= 10;
    }
    return result;
}


int player_print(char *buff, int n, uint32_t player) {
    if (player == 0) {
        return snprintf(buff, n, ".");
    } else if (player < 10) {
        return snprintf(buff, n, "%d", player);
    } else {
        return snprintf(buff, n, "[%d]", player);
    }
}