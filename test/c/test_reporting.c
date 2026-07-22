#include <stddef.h>
#include <string.h>

static char g_output[32768];
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
#include <jc_test.h>

TEST(CReportSuite, MultipleFailures)
{
    EXPECT_EQ(1, 2);
    EXPECT_STREQ("expected", "actual");
}

TEST(CReportSuite, FatalFailure)
{
    ASSERT_TRUE(0);
    EXPECT_TRUE(0);
}

int main(int argc, char** argv)
{
    int result;
    jc_test_init(&argc, argv);
    result = jc_test_run_all();
    if (result != 2)
        return 1;
    if (strstr(g_output, "Failures:") == NULL)
        return 2;
    if (strstr(g_output, "CReportSuite.MultipleFailures") == NULL)
        return 3;
    if (strstr(g_output, "Expected: (1) == (2)") == NULL)
        return 4;
    if (strstr(g_output, "actual: \"expected\" vs \"actual\"") == NULL)
        return 5;
    if (strstr(g_output, "CReportSuite.FatalFailure") == NULL)
        return 6;
    if (strstr(g_output, "Value of: 0") == NULL)
        return 7;
    if (strstr(g_output, "CReportSuite.MultipleFailures FAIL") == NULL)
        return 8;
    return 0;
}
