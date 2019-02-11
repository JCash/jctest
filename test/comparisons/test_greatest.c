#include "greatest.h"

// Add definitions that need to be in the test runner's main file.
GREATEST_MAIN_DEFS();

TEST mytest_multiplication(void) {
    ASSERT_EQ(4, 2*2);
    PASS();
}

TEST mytest_division(void) {
    ASSERT_EQ(2, 4/2);
    PASS();
}

SUITE(suite) {
    RUN_TEST(mytest_multiplication);
    RUN_TEST(mytest_division);
}

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(suite);
    GREATEST_MAIN_END();
}