
#define JC_TEST_USE_COLORS 1
#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.h>

TEST(Color, AlwaysOn)
{
    ASSERT_EQ(1u, jc_test_get_state()->use_colors);
}

