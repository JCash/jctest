
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

struct Foo
{
    Foo(int a) : i(a) { assert(i & 1 && "This should fail"); }
    int i;
};
#if !defined(USE_GTEST)
    struct TestPrintValue { int value; };
    struct TestPrintUnknownType {};
    template <> char* jc_test_print_value(char* buffer, size_t buffer_len, TestPrintValue v) {
        return buffer + JC_TEST_SNPRINTF(buffer, buffer_len, "TestPrintValue(%d)", v.value);
    }
#endif

enum TestEnum
{
    TESTENUM_OK,
    TESTENUM_VALUE1
};


#if !defined(USE_GTEST)
TEST(Assertions, Print)
{
    // test printouts of pointers
    char buffer[1024];

    Foo* p1 = reinterpret_cast<Foo*>(1);
    jc_test_print_value(buffer, sizeof(buffer), p1);
    EXPECT_STREQ("0x1", buffer);

    TestPrintValue v = { 2 };
    jc_test_print_value(buffer, sizeof(buffer), v);
    EXPECT_STREQ("TestPrintValue(2)", buffer);

    #define TEST_PRINT(VALUE, EXPECTEDSTR) \
        jc_test_print_value(buffer, sizeof(buffer), VALUE); \
        EXPECT_STREQ(EXPECTEDSTR, buffer);

    TEST_PRINT(float(3), "3.000000");
    TEST_PRINT(double(4), "4.000000");
    TEST_PRINT(uint8_t(4), "4");
    TEST_PRINT(uint16_t(5), "5");
    TEST_PRINT(uint32_t(6), "6");
    TEST_PRINT(uint64_t(7), "7");
    TEST_PRINT(int8_t(-4), "-4");
    TEST_PRINT(int16_t(-5), "-5");
    TEST_PRINT(int32_t(-6), "-6");
    TEST_PRINT(int64_t(-7), "-7");

    TEST_PRINT(TestPrintUnknownType(), "?");
}
#endif

TEST(Assertions, ExpectOk)
{
    const int           zero_i = 0;
    const unsigned int  zero_u = 0u;
    const float         zero_f = 0.0f;
    const double        zero_d = 0.0;
    const char*         zero_s = "zero";
    const uint64_t      zero_u64 = 0u;
    const int64_t       zero_i64 = 0;

    EXPECT_TRUE(0 == zero_i);
    EXPECT_FALSE(0 != zero_i);
    EXPECT_EQ(0, zero_i);
    EXPECT_EQ(0u, zero_u);
    EXPECT_EQ(0u, zero_u64);
    EXPECT_EQ(0, zero_i64);
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

    // float/double checks
    const float twothirds_f = 2.0f/3.0f;
    EXPECT_EQ(twothirds_f, 1.0f - 1.0f/3.0f );
    const double twothirds_d = 2.0/3.0;
    EXPECT_EQ(twothirds_d, 1.0 - 1.0/3.0 );

    const char* cow = "cow";
    EXPECT_STREQ("cow", cow);
    EXPECT_STRNE(NULL, cow);
    EXPECT_STRNE(0, cow);

    const char* nullstr = 0;
    EXPECT_STREQ(NULL, nullstr);
    EXPECT_STREQ(0, nullstr);
    EXPECT_EQ(nullptr, nullstr);

    EXPECT_EQ(TESTENUM_OK, zero_i);



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
    const uint64_t      zero_u64 = 1u;
    const int64_t       zero_i64 = 1;
    EXPECT_TRUE(0 == zero_i);
    EXPECT_FALSE(0 != zero_i);
    EXPECT_EQ(0, zero_i);
    EXPECT_EQ(0u, zero_u);
    EXPECT_EQ(0.0f, zero_f);
    EXPECT_EQ(0.0, zero_d);
    EXPECT_EQ(static_cast<uint64_t>(0u), zero_u64);
    EXPECT_EQ(static_cast<int64_t>(0), zero_i64);
    EXPECT_NE(1, zero_i);
    EXPECT_NE(1u, zero_u);
    EXPECT_NE(1.0f, zero_f);
    EXPECT_NE(1.0, zero_d);
    EXPECT_NEAR(0.001, zero_d, 0.01);
    EXPECT_STREQ("zero", zero_s);
    EXPECT_STRNE("one", zero_s);
    EXPECT_EQ(TESTENUM_OK, zero_i);

    // issue 28: make sure different types can be compared and printed out correctly
    EXPECT_EQ(uint32_t(0), zero_i);
    EXPECT_EQ(int32_t(0), zero_u);

    // Pointers
#if __cplusplus > 199711L
    EXPECT_EQ(nullptr, zero_s);
#endif

    Foo* p = reinterpret_cast<Foo*>(1);
    EXPECT_EQ(0, p);
    EXPECT_EQ(0U, p);
    EXPECT_EQ(reinterpret_cast<void*>(0), reinterpret_cast<void*>(p));
}

//////////////////////////////////////////////
