#include <stddef.h>
#include <string.h>

static char g_output[8192];
static size_t g_output_size;

static void capture_output(const char* text, size_t length)
{
    size_t available = sizeof(g_output) - g_output_size - 1;
    if (length > available)
        length = available;
    memcpy(g_output + g_output_size, text, length);
    g_output_size += length;
    g_output[g_output_size] = 0;
}

#define JC_TEST_OUTPUT_FN capture_output
#define JC_TEST_IMPLEMENTATION
#define JC_TEST_USE_COLORS 0
#include <jc_test.hpp>

TEST(FilterAlpha, One) { ASSERT_TRUE(1); }
TEST(FilterBeta, Two) { ASSERT_TRUE(1); }
TEST(FilterGamma, Three) { ASSERT_TRUE(0); }

int main()
{
    char program[] = "filter-test";
    char first_pattern[] = "--test-filter=FilterAlpha.*";
    char second_pattern[] = "--filter=*Beta.Two";
    char payload[] = "payload";
    char* argv[] = { program, first_pattern, second_pattern, payload };
    int argc = 4;
    int result;
    jc_test_init(&argc, argv);
    if (argc != 2 || strcmp(argv[1], "payload") != 0)
        return 1;
    result = jc_test_run_all();
    if (result != 0)
        return 2;
    if (strstr(g_output, "Ran 2 tests") == 0)
        return 3;
    if (strstr(g_output, "FilterGamma.Three") != 0)
        return 4;
    return 0;
}
