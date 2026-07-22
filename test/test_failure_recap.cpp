#include <stddef.h>
#include <string.h>

static char g_output[65536];
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

TEST(Recap, MultipleFailures)
{
    EXPECT_EQ(1, 2);
    EXPECT_STREQ("expected", "actual");
}

struct RecapFixture : public jc_test_base_class {
    virtual ~RecapFixture() JC_OVERRIDE;
};

RecapFixture::~RecapFixture() {}

TEST_F(RecapFixture, FatalFailure)
{
    ASSERT_TRUE(0);
    EXPECT_TRUE(0);
}

static int g_ran_after_fatal;

TEST_F(RecapFixture, RunsAfterFatal)
{
    EXPECT_TRUE(1);
    g_ran_after_fatal = 1;
}

struct RecapParam : public jc_test_params_class<int> {
    virtual ~RecapParam() JC_OVERRIDE;
};

RecapParam::~RecapParam() {}

TEST_P(RecapParam, IncludesFullParameterizedName)
{
    EXPECT_EQ(7, GetParam());
}

static int g_values[] = { 9 };
INSTANTIATE_TEST_CASE_P(Values, RecapParam, jc_test_values_in(g_values));

int main(int argc, char** argv)
{
    int result;
    jc_test_init(&argc, argv);
    result = jc_test_run_all();
    if (result != 3)
        return 1;
    if (strstr(g_output, "Failures:") == 0)
        return 2;
    if (strstr(g_output, "Recap.MultipleFailures") == 0)
        return 3;
    if (strstr(g_output, "Expected: (1) == (2)") == 0)
        return 4;
    if (strstr(g_output, "Expected: expected") == 0)
        return 5;
    if (strstr(g_output, "Values/RecapParam.IncludesFullParameterizedName/0") == 0)
        return 6;
    if (strstr(g_output, "Expected: (7) == (GetParam())") == 0)
        return 7;
    if (strstr(g_output, "RecapFixture.FatalFailure") == 0)
        return 8;
    if (strstr(g_output, "Value of: 0") == 0)
        return 9;
    if (!g_ran_after_fatal)
        return 10;
    if (strstr(g_output, "RecapFixture.RunsAfterFatal PASS") == 0)
        return 11;
    if (strstr(g_output, "Values/RecapParam.IncludesFullParameterizedName/0 FAIL") == 0)
        return 12;
    return 0;
}
