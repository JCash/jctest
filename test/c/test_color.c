#include <stddef.h>
#include <string.h>

static char g_output[4096];
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
#define JC_TEST_USE_COLORS 1
#include <jc_test.h>

TEST(CColor, Enabled)
{
    ASSERT_TRUE(1);
}

int main(int argc, char** argv)
{
    int result;
    jc_test_init(&argc, argv);
    result = jc_test_run_all();
    if (result != 0)
        return 1;
    return strstr(g_output, "\x1b[") != NULL ? 0 : 2;
}
