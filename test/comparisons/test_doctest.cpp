
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"


TEST_CASE("mytest_multiply")
{
    CHECK_EQ(4, 2 * 2);
}

TEST_CASE("mytest_division")
{
    CHECK_EQ(2, 4 / 2);
}

int main(int argc, char **argv)
{
    return doctest::Context(argc, argv).run();
}
