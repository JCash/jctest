
#define JC_TEST_USE_COLORS 0
#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.h>

TEST(Color, AlwaysOff)
{
    ASSERT_EQ(0u, jc_test_get_state()->use_colors);
}

