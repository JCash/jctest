#include <jc_test.h>

typedef struct Value {
    int member;
} Value;

TEST(CompileFailure, StructValues)
{
    Value a = { 1 };
    Value b = { 1 };
    ASSERT_EQ(a, b);
}
