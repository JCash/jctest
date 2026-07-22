#include <jc_test.h>

TEST(CompileFailure, OrderedPointers)
{
    int a;
    int b;
    ASSERT_LT(&a, &b);
}
