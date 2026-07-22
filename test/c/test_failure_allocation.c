#include <stddef.h>
#include <string.h>

static char g_output[8192];
static size_t g_output_size;

static void* fail_malloc(size_t size) { (void)size; return NULL; }
static void* fail_realloc(void* pointer, size_t size) { (void)pointer; (void)size; return NULL; }
static void fail_free(void* pointer) { (void)pointer; }

static void capture_output(const char* text, size_t length)
{
    size_t available = sizeof(g_output) - g_output_size - 1;
    if (length > available)
        length = available;
    memcpy(g_output + g_output_size, text, length);
    g_output_size += length;
    g_output[g_output_size] = 0;
}

#define JC_TEST_FAILURE_MALLOC fail_malloc
#define JC_TEST_FAILURE_REALLOC fail_realloc
#define JC_TEST_FAILURE_FREE fail_free
#define JC_TEST_OUTPUT_FN capture_output
#define JC_TEST_IMPLEMENTATION
#define JC_TEST_USE_COLORS 0
#include <jc_test.h>

TEST(CAllocationFailure, StillCompletes)
{
    EXPECT_EQ(1, 2);
}

int main(int argc, char** argv)
{
    int result;
    jc_test_init(&argc, argv);
    result = jc_test_run_all();
    if (result != 1)
        return 1;
    if (strstr(g_output, "message unavailable") == NULL)
        return 2;
    if (strstr(g_output, "1 failure detail(s) could not be retained") == NULL)
        return 3;
    return 0;
}
