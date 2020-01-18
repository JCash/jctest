#include <stdint.h>

#define JC_TEST_IMPLEMENTATION
#include <jc_test.h>

enum Result { RESULT_OK, RESULT_FAIL };

static const char* ResultToString(Result r) {
    switch(r) {
        case RESULT_OK: return "RESULT_OK";
        case RESULT_FAIL: return "RESULT_FAIL";
    }
}

template <> char* jc_test_print_value(char* buffer, size_t buffer_len, Result r) {
    return buffer + JC_TEST_SNPRINTF(buffer, buffer_len, "%s", ResultToString(r));
}

static Result ReturnFail() {
    return RESULT_FAIL;
}

TEST(Example, CustomPrint) {
    ASSERT_EQ(RESULT_OK, ReturnFail());
}

int main(int argc, char **argv)
{
    jc_test_init(&argc, argv);
    return jc_test_run_all();
}
