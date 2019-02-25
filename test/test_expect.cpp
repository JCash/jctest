
#include <string.h>
#include <stdio.h>
#include "testutil.h"

// To test the testing framework /////////////
void GlobalTestSetup(); // to silence a warning
void GlobalTestSetup() {
    printf("Setting up the test\n");
}
int GlobalTestTeardown(); // to silence a warning
int GlobalTestTeardown() {
    printf("Verifying the the test\n");

    int fail = 0;
#if !defined(USE_GTEST)
    int expected_fails = 1;
    int actual_fails = jc_test_get_state()->stats.num_fail;
    printf("Expecting %d test to fail, got %d failed tests\n", expected_fails, actual_fails);

    fail |= (expected_fails != actual_fails) ? 1 : 0;
#endif

    return fail;
}


TEST(Assertions, ExpectOk)
{
    const int           zero_i = 0;
    const unsigned int  zero_u = 0u;
    const float         zero_f = 0.0f;
    const double        zero_d = 0.0;
    const char*         zero_s = "zero";
    EXPECT_TRUE(0 == zero_i);
    EXPECT_FALSE(0 != zero_i);
    EXPECT_EQ(0, zero_i);
    EXPECT_EQ(0u, zero_u);
    EXPECT_EQ(0.0f, zero_f);
    EXPECT_EQ(0.0, zero_d);
    EXPECT_NE(1, zero_i);
    EXPECT_NE(1u, zero_u);
    EXPECT_NE(1.0f, zero_f);
    EXPECT_NE(1.0, zero_d);
    EXPECT_NEAR(0.001, zero_d, 0.01);
    EXPECT_STREQ("zero", zero_s);
    EXPECT_STRNE("one", zero_s);
}

TEST(Assertions, ExpectFail)
{
    // here we run the same tests, but make sure they all fail
    const int           zero_i = 1;
    const unsigned int  zero_u = 1u;
    const float         zero_f = 1.0f;
    const double        zero_d = 1.0;
    const char*         zero_s = "one";
    EXPECT_TRUE(0 == zero_i);
    EXPECT_FALSE(0 != zero_i);
    EXPECT_EQ(0, zero_i);
    EXPECT_EQ(0u, zero_u);
    EXPECT_EQ(0.0f, zero_f);
    EXPECT_EQ(0.0, zero_d);
    EXPECT_NE(1, zero_i);
    EXPECT_NE(1u, zero_u);
    EXPECT_NE(1.0f, zero_f);
    EXPECT_NE(1.0, zero_d);
    EXPECT_NEAR(0.001, zero_d, 0.01);
    EXPECT_STREQ("zero", zero_s);
    EXPECT_STRNE("one", zero_s);
}

//////////////////////////////////////////////

