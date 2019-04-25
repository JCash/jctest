#include <string.h>
#include <stdio.h>
#include "testutil.h"

#include <assert.h>

// To test the testing framework /////////////
void GlobalTestSetup(); // to silence a warning
void GlobalTestSetup() {
    printf("Setting up calls\n");
}
int GlobalTestTeardown(); // to silence a warning
int GlobalTestTeardown() {
    printf("Verifying the calls\n");

    int fail = 0;
#if !defined(USE_GTEST)
    int expected_fails = 0;
    int actual_fails = jc_test_get_state()->stats.num_fail;

    if ((expected_fails != actual_fails) ? 1 : 0) {
        printf("Expecting %d test to fail, got %d failed tests\n", expected_fails, actual_fails);
        fail = 1;
    }

    int expected_tests = 0;
    int actual_num_tests = jc_test_get_state()->stats.num_tests;

    if ((expected_tests != actual_num_tests) ? 1 : 0) {
        printf("Expecting %d tests, got %d tests\n", expected_tests, actual_num_tests);
        fail = 1;
    }
#endif

    return fail;
}
//////////////////////////////////////////////

