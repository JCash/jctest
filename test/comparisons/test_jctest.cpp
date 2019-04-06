#define JC_TEST_IMPLEMENTATION
#include <jc_test.h>

TEST(MyTest, Multiply) {
    ASSERT_EQ(4, 2*2);
}

TEST(MyTest, Division) {
    ASSERT_EQ(2, 4/2);
}

int main(int argc, char *argv[])
{
    jc_test_init(&argc, argv);
    return jc_test_run_all();
}
