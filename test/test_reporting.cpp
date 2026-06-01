#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct jc_test_entry;
struct jc_test_fixture;
struct jc_test_state;
struct jc_test_stats;

struct CapturingLogger
{
    CapturingLogger();
    ~CapturingLogger();

    void OnFixtureSetup(const jc_test_fixture* fixture);
    void OnFixtureTeardown(const jc_test_fixture* fixture);
    void OnTestSetup(const jc_test_fixture* fixture, const jc_test_entry* test);
    void OnTestTeardown(const jc_test_fixture* fixture, const jc_test_entry* test);
    void OnSummary(const jc_test_stats* stats, const jc_test_state* state);
    void OnTestFailed(const jc_test_fixture* fixture, const jc_test_entry* test, const char* format, ...);

    void Log(const char* str);
    void Log(const char* str, size_t len);
    void Logf(const char* format, ...);
};

#define JC_TEST_LOGGER_CLASS CapturingLogger
#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.h>

static char g_LastFailure[1024];
static int  g_CaptureOnly = 0;

CapturingLogger::CapturingLogger()
{
}

CapturingLogger::~CapturingLogger()
{
}

void CapturingLogger::OnFixtureSetup(const jc_test_fixture* fixture)
{
    (void)fixture;
}

void CapturingLogger::OnFixtureTeardown(const jc_test_fixture* fixture)
{
    (void)fixture;
}

void CapturingLogger::OnTestSetup(const jc_test_fixture* fixture, const jc_test_entry* test)
{
    (void)fixture;
    (void)test;
}

void CapturingLogger::OnTestTeardown(const jc_test_fixture* fixture, const jc_test_entry* test)
{
    (void)fixture;
    (void)test;
}

void CapturingLogger::OnSummary(const jc_test_stats* stats, const jc_test_state* state)
{
    (void)stats;
    (void)state;
}

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

void CapturingLogger::OnTestFailed(const jc_test_fixture* fixture, const jc_test_entry* test, const char* format, ...)
{
    char buffer[1024];
    size_t offset = 0;
    int n;

    (void)test;

    n = JC_TEST_SNPRINTF(buffer, sizeof(buffer), "\n%s:%d:", fixture->filename, fixture->line);
    if (n > 0)
        offset = (size_t)n;
    if (offset >= sizeof(buffer))
        offset = sizeof(buffer) - 1;

    if (format)
    {
        va_list args;
        va_start(args, format);
        n = vsnprintf(buffer + offset, sizeof(buffer) - offset, format, args);
        va_end(args);
        (void)n;
    }

    {
        const char* expected = strstr(buffer, "Expected:");
        size_t len;
        if (!expected)
            expected = buffer;
        len = strlen(expected);
        if (len >= sizeof(g_LastFailure))
            len = sizeof(g_LastFailure) - 1;
        memcpy(g_LastFailure, expected, len);
        g_LastFailure[len] = 0;
    }

    if (!g_CaptureOnly)
        Log(buffer);
}

void CapturingLogger::Log(const char* str)
{
    fputs(str, stdout);
    fflush(stdout);
}

void CapturingLogger::Log(const char* str, size_t len)
{
    fwrite(str, len, 1, stdout);
    fflush(stdout);
}

void CapturingLogger::Logf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

struct FailureState
{
    unsigned int test_fail;
    unsigned int fixture_fail;
    unsigned int fixture_fatal;
};

static void SaveFailureState(FailureState* state)
{
    state->test_fail = jc_test_get_test()->fail;
    state->fixture_fail = jc_test_get_fixture()->fail;
    state->fixture_fatal = jc_test_get_fixture()->fatal;
}

static void RestoreFailureState(const FailureState* state)
{
    jc_test_get_test()->fail = state->test_fail;
    jc_test_get_fixture()->fail = state->fixture_fail;
    jc_test_get_fixture()->fatal = state->fixture_fatal;
}

#define CHECK_EQ_FAILURE_TEXT(A, B, EXPECTED) \
    do { \
        FailureState state; \
        g_LastFailure[0] = 0; \
        g_CaptureOnly = 1; \
        SaveFailureState(&state); \
        EXPECT_EQ(A, B); \
        RestoreFailureState(&state); \
        g_CaptureOnly = 0; \
        EXPECT_STREQ(EXPECTED, g_LastFailure); \
    } while(0)

TEST(Reporting, NumericComparisonsPrintActualValues)
{
    const size_t size_a = 3;
    const size_t size_b = 4;
    const int8_t signed8_a = -3;
    const int8_t signed8_b = -4;
    const uint8_t unsigned8_a = 3;
    const uint8_t unsigned8_b = 4;
    const int16_t signed16_a = -3;
    const int16_t signed16_b = -4;
    const uint16_t unsigned16_a = 3;
    const uint16_t unsigned16_b = 4;
    const int32_t signed32_a = -3;
    const int32_t signed32_b = -4;
    const uint32_t unsigned32_a = 3;
    const uint32_t unsigned32_b = 4;
    const int64_t signed64_a = -3;
    const int64_t signed64_b = -4;
    const uint64_t unsigned64_a = 3;
    const uint64_t unsigned64_b = 4;
    const float float_a = 3.0f;
    const float float_b = 4.0f;
    const double double_a = 3.0;
    const double double_b = 4.0;

    CHECK_EQ_FAILURE_TEXT(size_a, size_b, "Expected: (size_a) == (size_b), actual: 3 vs 4\n");
    CHECK_EQ_FAILURE_TEXT(size_b, size_a, "Expected: (size_b) == (size_a), actual: 4 vs 3\n");

    CHECK_EQ_FAILURE_TEXT(signed8_a, signed8_b, "Expected: (signed8_a) == (signed8_b), actual: -3 vs -4\n");
    CHECK_EQ_FAILURE_TEXT(signed8_b, signed8_a, "Expected: (signed8_b) == (signed8_a), actual: -4 vs -3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned8_a, unsigned8_b, "Expected: (unsigned8_a) == (unsigned8_b), actual: 3 vs 4\n");
    CHECK_EQ_FAILURE_TEXT(unsigned8_b, unsigned8_a, "Expected: (unsigned8_b) == (unsigned8_a), actual: 4 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(signed8_a, unsigned8_a, "Expected: (signed8_a) == (unsigned8_a), actual: -3 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned8_a, signed8_a, "Expected: (unsigned8_a) == (signed8_a), actual: 3 vs -3\n");

    CHECK_EQ_FAILURE_TEXT(signed16_a, signed16_b, "Expected: (signed16_a) == (signed16_b), actual: -3 vs -4\n");
    CHECK_EQ_FAILURE_TEXT(signed16_b, signed16_a, "Expected: (signed16_b) == (signed16_a), actual: -4 vs -3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned16_a, unsigned16_b, "Expected: (unsigned16_a) == (unsigned16_b), actual: 3 vs 4\n");
    CHECK_EQ_FAILURE_TEXT(unsigned16_b, unsigned16_a, "Expected: (unsigned16_b) == (unsigned16_a), actual: 4 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(signed16_a, unsigned16_a, "Expected: (signed16_a) == (unsigned16_a), actual: -3 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned16_a, signed16_a, "Expected: (unsigned16_a) == (signed16_a), actual: 3 vs -3\n");

    CHECK_EQ_FAILURE_TEXT(signed32_a, signed32_b, "Expected: (signed32_a) == (signed32_b), actual: -3 vs -4\n");
    CHECK_EQ_FAILURE_TEXT(signed32_b, signed32_a, "Expected: (signed32_b) == (signed32_a), actual: -4 vs -3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned32_a, unsigned32_b, "Expected: (unsigned32_a) == (unsigned32_b), actual: 3 vs 4\n");
    CHECK_EQ_FAILURE_TEXT(unsigned32_b, unsigned32_a, "Expected: (unsigned32_b) == (unsigned32_a), actual: 4 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(signed32_a, unsigned32_a, "Expected: (signed32_a) == (unsigned32_a), actual: -3 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned32_a, signed32_a, "Expected: (unsigned32_a) == (signed32_a), actual: 3 vs -3\n");

    CHECK_EQ_FAILURE_TEXT(signed64_a, signed64_b, "Expected: (signed64_a) == (signed64_b), actual: -3 vs -4\n");
    CHECK_EQ_FAILURE_TEXT(signed64_b, signed64_a, "Expected: (signed64_b) == (signed64_a), actual: -4 vs -3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned64_a, unsigned64_b, "Expected: (unsigned64_a) == (unsigned64_b), actual: 3 vs 4\n");
    CHECK_EQ_FAILURE_TEXT(unsigned64_b, unsigned64_a, "Expected: (unsigned64_b) == (unsigned64_a), actual: 4 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(signed64_a, unsigned64_a, "Expected: (signed64_a) == (unsigned64_a), actual: -3 vs 3\n");
    CHECK_EQ_FAILURE_TEXT(unsigned64_a, signed64_a, "Expected: (unsigned64_a) == (signed64_a), actual: 3 vs -3\n");

    CHECK_EQ_FAILURE_TEXT(float_a, float_b, "Expected: (float_a) == (float_b), actual: 3.000000 vs 4.000000\n");
    CHECK_EQ_FAILURE_TEXT(float_b, float_a, "Expected: (float_b) == (float_a), actual: 4.000000 vs 3.000000\n");
    CHECK_EQ_FAILURE_TEXT(double_a, double_b, "Expected: (double_a) == (double_b), actual: 3.000000 vs 4.000000\n");
    CHECK_EQ_FAILURE_TEXT(double_b, double_a, "Expected: (double_b) == (double_a), actual: 4.000000 vs 3.000000\n");
}

#undef CHECK_EQ_FAILURE_TEXT
