
#include <stdio.h>
#include <stdarg.h> //va_list
#include <string.h>

static size_t  g_WriteBufferLen = 0;
static char    g_WriteBuffer[1024];

// static void CustomLogFn(const char* text, size_t text_len)
// {
//     size_t max_len = sizeof(g_WriteBuffer)-1;
//     size_t remaining = max_len - g_WriteBufferLen;
//     if (remaining < text_len)
//         text_len = remaining;
//     strlcpy(g_WriteBuffer+g_WriteBufferLen, text, text_len);
//     g_WriteBufferLen += text_len;

//     printf("HELLO: %s", text);
// }

//#define JC_TEST_OUTPUT_FN CustomLogFn

#include <jc_test.h> // no implementation

struct Logger : public jc_test_print_logger
{
    Logger();
    void FlushBuffer() override;
};

Logger::Logger()
{
    printf("Logger()\n");
    fflush(stdout);
}

void Logger::FlushBuffer()
{
    const char* expected = strstr(str->buffer, "Expected:");
    if (!expected)
        expected = "Unknown";
    size_t len = strlen(expected);

    size_t max_len = sizeof(g_WriteBuffer)-1;
    if (len > max_len)
        len = max_len;
    strlcpy(g_WriteBuffer, expected, len);
    g_WriteBufferLen = len;

    jc_test_print_logger::FlushBuffer();
}

#define JC_TEST_LOGGER_CLASS Logger
#define JC_TEST_USE_COLORS 1
#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.h>

// To test the testing framework /////////////
void GlobalTestSetup(); // to silence a warning
void GlobalTestSetup() {
    printf("Setting up the test\n");
}
int GlobalTestTeardown(); // to silence a warning
int GlobalTestTeardown() {
    printf("Verifying the the test\n");

    int fail = 0;
#if !defined(USE_GTEST)
    int expected_fails = 0;
    int actual_fails = jc_test_get_state()->stats.num_fail;
    printf("Expecting %d test to fail, got %d failed tests\n", expected_fails, actual_fails);

    fail |= (expected_fails != actual_fails) ? 1 : 0;
#endif

    return fail;
}

struct Foo
{
    int i;
};

struct TestPrintValue { int value; };
struct TestPrintUnknownType {};
template <> char* jc_test_print_value(char* buffer, size_t buffer_len, TestPrintValue v) {
    return buffer + JC_TEST_SNPRINTF(buffer, buffer_len, "TestPrintValue(%d)", v.value);
}

enum TestEnum
{
    TESTENUM_OK,
    TESTENUM_VALUE1
};

TEST(Print, PrintValue)
{
    // test printouts of pointers
    char buffer[1024];

    Foo* p1 = reinterpret_cast<Foo*>(1);
    jc_test_print_value(buffer, sizeof(buffer), p1);
    EXPECT_STREQ("0x1", buffer);

    TestPrintValue v = { 2 };
    jc_test_print_value(buffer, sizeof(buffer), v);
    EXPECT_STREQ("TestPrintValue(2)", buffer);

    #define TEST_PRINT(VALUE, EXPECTEDSTR) \
        jc_test_print_value(buffer, sizeof(buffer), VALUE); \
        EXPECT_STREQ(EXPECTEDSTR, buffer);

    TEST_PRINT(float(3), "3.000000");
    TEST_PRINT(double(4), "4.000000");
    TEST_PRINT(uint8_t(4), "4");
    TEST_PRINT(uint16_t(5), "5");
    TEST_PRINT(uint32_t(6), "6");
    TEST_PRINT(uint64_t(7), "7");
    TEST_PRINT(int8_t(-4), "-4");
    TEST_PRINT(int16_t(-5), "-5");
    TEST_PRINT(int32_t(-6), "-6");
    TEST_PRINT(int64_t(-7), "-7");

    TEST_PRINT(nullptr, "nullptr");

    TEST_PRINT(TestPrintUnknownType(), "?");

    #undef TEST_PRINT
}

struct save_state
{
    uint32_t test_fail;
    uint32_t fixture_fail;
    uint32_t fixture_fatal;
};

static void SaveState(save_state* state)
{
    state->test_fail    = jc_test_get_test()->fail;
    state->fixture_fail = jc_test_get_fixture()->fail = 1;
    state->fixture_fatal= jc_test_get_fixture()->fatal;
}

static void LoadState(save_state* state)
{
    jc_test_get_test()->fail = state->test_fail;
    jc_test_get_fixture()->fail = state->fixture_fail;
    jc_test_get_fixture()->fatal = state->fixture_fatal;
}

TEST(Print, Comparisons)
{
    #define TEST_PRINT(_VALUEA, _VALUEB, EXPECTEDSTR) \
        { \
            save_state state; \
            SaveState(&state); \
                EXPECT_EQ(_VALUEA, _VALUEB); \
            LoadState(&state); \
            ASSERT_STREQ(EXPECTEDSTR, g_WriteBuffer); \
        }

    TEST_PRINT(0, 1, "Expected: (0) == (1), actual: 0 vs 1");
    TEST_PRINT(0u, 1, "Expected: (0u) == (1), actual: 0 vs 1");

    // Testing the printing of null pointer types
    TEST_PRINT(0, (void*)1, "Expected: (0) == ((void*)1), actual: 0x0 vs 0x1");
    TEST_PRINT(0, (int*)1, "Expected: (0) == ((int*)1), actual: 0x0 vs 0x1");
    TEST_PRINT(0u, (int*)1, "Expected: (0u) == ((int*)1), actual: 0x0 vs 0x1");
    TEST_PRINT(nullptr, (int*)1, "Expected: (nullptr) == ((int*)1), actual: nullptr vs 0x1");

    //printf("result: '%s'\n", g_WriteBuffer);

    #undef TEST_PRINT
}

