#define JC_TEST_IMPLEMENTATION
#include <jc_test.h>

TEST(MyTest, Multiplication) {
    ASSERT_EQ(4, 2 * 2);
}
TEST(MyTest, Division) {
    ASSERT_EQ(2, 4 / 2);
}

int main(int argc, char *argv[]) {
    // Parses and removes any jc_test specific arguments
    jc_test_init(&argc, argv);
    // ... Do your test initialization
    return jc_test_run_all();
}
