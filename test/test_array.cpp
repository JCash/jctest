
#include <stdio.h>
#include "testutil.h"

// To test the testing framework /////////////
void GlobalTestSetup(); // to silence a warning
void GlobalTestSetup() {
    // Setting up the test
}
int GlobalTestTeardown(); // to silence a warning
int GlobalTestTeardown() {
    // Verifying the the test

    int fail = 0;
#if !defined(USE_GTEST)
    int expected_fails = 1;
    int actual_fails = jc_test_get_state()->stats.num_fail;
    // Expecting %d test to fail, got %d failed tests\n", expected_fails, actual_fa

    fail |= (expected_fails != actual_fails) ? 1 : 0;
#endif

    return fail;
}

template<typename T>
void fill_range(T* a, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        a[i] = static_cast<T>(i);
    }
}

TEST(Array, ExpectOk)
{
    // const char* arr_string = "HELLO";
    // EXPECT_ARRAY_EQ(arr_string, "HELLO");

    uint8_t a[] = {0,1,2,3,4,5,6,7,8,9};
    uint8_t b[10];
    fill_range(b, sizeof(b));
    EXPECT_ARRAY_EQ(a, b);
}

TEST(Array, ExpectFail)
{
    {
        // tests the error in a short array
        uint8_t a[] = {0,1,2,3,4,5,6,7,8,9};
        uint8_t b[10];
        fill_range(b, sizeof(b));
        b[3] = 4;
        b[4] = 3;
        b[6] = 5;
        EXPECT_ARRAY_EQ(a, b);
    }
    {
        // tests the error at the beginning of the array
        uint8_t a[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
        uint8_t b[40];
        fill_range(b, sizeof(b));
        b[3] = 4;
        b[4] = 3;
        b[6] = 5;
        EXPECT_ARRAY_EQ(a, b);
    }
    {
        // tests the error at the end of the array
        uint8_t a[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
        uint8_t b[40];
        fill_range(b, sizeof(b));
        b[33] = 4;
        b[34] = 3;
        b[36] = 5;
        EXPECT_ARRAY_EQ(a, b);
    }
    {
        // tests the error at the middle of the array
        uint8_t a[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
        uint8_t b[40];
        fill_range(b, sizeof(b));
        b[20] = 4;
        b[21] = 3;
        b[23] = 5;
        EXPECT_ARRAY_EQ(a, b);
    }
    {
        const char a[] = "HELLO WORLD";
        const char b[] = "HeLLo WoRLD";
        EXPECT_ARRAY_EQ(a, b);
    }
    {
        const uint8_t a_uint8_t[] = {0,1,2,3,4,5,6,7,8,9};
        const uint8_t b_uint8_t[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ(a_uint8_t, b_uint8_t);
    }
    {
        const uint16_t a_uint16_t[] = {0,1,2,3,4,5,6,7,8,9};
        const uint16_t b_uint16_t[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ(a_uint16_t, b_uint16_t);
    }
    {
        const uint32_t a_uint32_t[] = {0,1,2,3,4,5,6,7,8,9};
        const uint32_t b_uint32_t[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ(a_uint32_t, b_uint32_t);
    }
    {
        const float a_float[] = {0,1,2,3,4,5,6,7,8,9};
        const float b_float[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ(a_float, b_float);
    }
    {
        const double a_double[] = {0,1,2,3,4,5,6,7,8,9};
        const double b_double[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ(a_double, b_double);
    }
}

TEST(Array, ExpectFailLength)
{
    {
        const char a[] = "HELLO WORLD";
        const char b[] = "HeLLo WoRLD";
        EXPECT_ARRAY_EQ_LEN(a, b, 5);
    }
    {
        const uint8_t a_uint8_t[] = {0,1,2,3,4,5,6,7,8,9};
        const uint8_t b_uint8_t[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ_LEN(a_uint8_t, b_uint8_t, 5);
    }
    {
        const uint16_t a_uint16_t[] = {0,1,2,3,4,5,6,7,8,9};
        const uint16_t b_uint16_t[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ_LEN(a_uint16_t, b_uint16_t, 5);
    }
    {
        const uint32_t a_uint32_t[] = {0,1,2,3,4,5,6,7,8,9};
        const uint32_t b_uint32_t[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ_LEN(a_uint32_t, b_uint32_t, 5);
    }
    {
        const float a_float[] = {0,1,2,3,4,5,6,7,8,9};
        const float b_float[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ_LEN(a_float, b_float, 5);
    }
    {
        const double a_double[] = {0,1,2,3,4,5,6,7,8,9};
        const double b_double[] = {0,1,2,3,3,5,5,7,8,9};
        EXPECT_ARRAY_EQ_LEN(a_double, b_double, 5);
    }
}
