#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.h>

TEST_CASE(multiply) {
  CHECK_EQ(4, 2*2);
}
