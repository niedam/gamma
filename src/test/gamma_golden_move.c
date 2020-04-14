#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include "../gamma.h"
#include "cmocka.h"


static void gamma_golden1(void **state) {
    (void) state;
    static const char board[] = ".........\n"
                                "..2..1...\n"
                                ".222.....\n"
                                "..2..2.1.\n"
                                ".........\n";
    gamma_t *gamma = gamma_new(9, 5, 2, 3);
    assert_non_null(gamma);
    assert_true(gamma_move(gamma, 2, 2, 2));
    assert_true(gamma_move(gamma, 2, 2, 3));
    assert_true(gamma_move(gamma, 2, 2, 1));
    assert_true(gamma_move(gamma, 2, 1, 2));
    assert_true(gamma_move(gamma, 2, 3, 2));
    assert_true(gamma_move(gamma, 2, 5, 1));
    assert_true(gamma_move(gamma, 1, 5, 3));
    assert_true(gamma_move(gamma, 1, 7, 1));
    assert_false(gamma_golden_move(gamma, 1, 2, 2));
    assert_string_equal(board, gamma_board(gamma));
}


int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(gamma_golden1),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}