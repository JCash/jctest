#include <jc_test.h>

#include <math.h>
#include <signal.h>

typedef enum Number {
    NUMBER_FOUR = 4
} Number;

TEST(CAssertions, Scalars)
{
    signed char signed_value = -2;
    unsigned long unsigned_value = 4;
    Number number = NUMBER_FOUR;
    int value = 4;
    int* pointer = &value;

    ASSERT_TRUE(value);
    ASSERT_FALSE(0);
    ASSERT_EQ(-2, signed_value);
    ASSERT_EQ(4U, unsigned_value);
    ASSERT_EQ(4, number);
    ASSERT_NE(3, value);
    ASSERT_LT(3, value);
    ASSERT_GT(value, 3);
    ASSERT_LE(4, value);
    ASSERT_GE(value, 4);
    ASSERT_EQ(pointer, &value);
    ASSERT_NE(pointer, NULL);
    ASSERT_EQ((int*)NULL, NULL);
}

TEST(CAssertions, UsualIntegerConversions)
{
    signed char negative_byte = -1;
    unsigned char maximum_byte = UCHAR_MAX;
    int negative_int = -1;
    unsigned int maximum_int = UINT_MAX;

    ASSERT_LT(negative_byte, maximum_byte);
    ASSERT_EQ(negative_int, maximum_int);
}

TEST(CAssertions, FloatingPoint)
{
    float a = 1.0f;
    double b = 2.0;
    ASSERT_EQ(a, 1.0f);
    ASSERT_EQ(b, 2.0);
    ASSERT_NEAR(10.0, 10.001, 0.01);
    ASSERT_NE(NAN, NAN);
    ASSERT_NE(INFINITY, FLT_MAX);
}

TEST(CAssertions, StringsAndArrays)
{
    const char* text = "hello";
    int expected[] = { 1, 2, 3, 4 };
    int actual[] = { 1, 2, 3, 4 };
    ASSERT_STREQ("hello", text);
    ASSERT_STRNE("world", text);
    ASSERT_STREQ(NULL, NULL);
    ASSERT_ARRAY_EQ(expected, actual);
    ASSERT_ARRAY_EQ_LEN(expected, actual, 3);
}

TEST(CAssertions, SingleEvaluation)
{
    int a = 0;
    int b = 0;
    EXPECT_EQ(0, a++);
    EXPECT_TRUE(++b);
    ASSERT_EQ(1, a);
    ASSERT_EQ(1, b);
}

#if !defined(JC_TEST_NO_DEATH_TEST)
TEST(CAssertions, Death)
{
    ASSERT_DEATH(raise(SIGABRT), "");
}
#endif
