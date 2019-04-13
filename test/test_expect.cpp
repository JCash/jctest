
#include <stdio.h>
#include "testutil.h"

#if defined(USE_GTEST)
    #define JC_TEST_IS_NULL_LITERAL GTEST_IS_NULL_LITERAL_
#endif

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

struct Foo
{
    Foo(int a) : i(a) { assert(i & 1 && "This should fail"); }
    int i;
};

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
    EXPECT_EQ(0.0f, zero_d);
    EXPECT_EQ(-0.0, zero_d);
    EXPECT_NE(1, zero_i);
    EXPECT_NE(1u, zero_u);
    EXPECT_NE(1.0f, zero_f);
    EXPECT_NE(1.0, zero_d);
    EXPECT_NEAR(0.001, zero_d, 0.01);
    EXPECT_STREQ("zero", zero_s);
    EXPECT_STRNE("one", zero_s);

    EXPECT_TRUE(JC_TEST_IS_NULL_LITERAL(0));
    EXPECT_TRUE(JC_TEST_IS_NULL_LITERAL(NULL));
    EXPECT_TRUE(JC_TEST_IS_NULL_LITERAL(0U));
    EXPECT_TRUE(JC_TEST_IS_NULL_LITERAL(0L));
    EXPECT_FALSE(JC_TEST_IS_NULL_LITERAL(1));
    EXPECT_FALSE(JC_TEST_IS_NULL_LITERAL(0.0));
    EXPECT_FALSE(JC_TEST_IS_NULL_LITERAL('a'));

    // float/double checks
    const float twothirds_f = 2.0f/3.0f;
    EXPECT_EQ(twothirds_f, 1.0f - 1.0f/3.0f );
    const double twothirds_d = 2.0/3.0;
    EXPECT_EQ(twothirds_d, 1.0 - 1.0/3.0 );

    #if !defined(USE_GTEST)
    EXPECT_TRUE(jc_test_is_pointer<char*>::value);
    EXPECT_FALSE(jc_test_is_pointer<char>::value);
    #endif

    const char* cow = "cow";
    EXPECT_STREQ("cow", cow);
    EXPECT_STRNE(NULL, cow);
    EXPECT_STRNE(0, cow);

    const char* nullstr = 0;
    EXPECT_STREQ(NULL, nullstr);
    EXPECT_STREQ(0, nullstr);

    printf("EXPECTED ASSERT ->\n");
    EXPECT_DEATH(Foo f(16), "");
    printf("<- END OF EXPECTED ASSERT\n");
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

