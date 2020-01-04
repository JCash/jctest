#define JC_TEST_IMPLEMENTATION_WITH_MAIN
#include <jc_test.h>

TEST(my, test) {
  ASSERT_EQ(4, 2*2);
}
