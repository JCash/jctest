#define _stricmp strcmp // Because it wasn't included by utest.h !!?

#define UTEST_C_IMPLEMENTATION
#include "utest.h"

TEST(mytest_multiplication)
{
    TEST_ASSERT_EQUAL_INT32(4, 2 * 2);
}

TEST(mytest_division)
{
    TEST_ASSERT_EQUAL_INT32(2, 4 / 2);
}

TEST_FIXTURE_BEGIN(mytest_fixture)
    TEST_FIXTURE_TEST(mytest_multiplication)
    TEST_FIXTURE_TEST(mytest_division)
TEST_FIXTURE_END()

int main(int argc, char const *argv[])
{
    utest_init();
    return TEST_RUN_FIXTURE(mytest_fixture);
}