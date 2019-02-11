#include <gtest/gtest.h>

TEST(MyTest, Multiply) {
    ASSERT_EQ(4, 2*2);
}

TEST(MyTest, Division) {
    ASSERT_EQ(2, 4/2);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
