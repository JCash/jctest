#include <string.h>

#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.hpp>

TEST(BufferedString, AppendExactFitKeepsTerminatorInBounds)
{
    jc_buffered_string str(4);
    const char text[] = {'a', 'b', 'c', 'd'};

    str.Append(text, sizeof(text));

    ASSERT_EQ(sizeof(text), str.Size());
    ASSERT_LT(str.Size(), str.capacity);
    ASSERT_EQ(0, str.buffer[str.Size()]);
    ASSERT_EQ(0, memcmp(str.buffer, text, sizeof(text)));
}

TEST(BufferedString, AppendStringDoesNotStoreTerminator)
{
    jc_buffered_string str(4);
    const char* expected = "one\ntwo\n";

    str.Append("one\n");
    str.Append("two\n");

    ASSERT_EQ(strlen(expected), str.Size());
    ASSERT_EQ(0, memcmp(str.buffer, expected, str.Size()));
    ASSERT_EQ(0, str.buffer[str.Size()]);
}

TEST(BufferedString, AppendfPreservesArgumentsWhenGrowing)
{
    jc_buffered_string str(4);
    const char* expected = "prefix 42 suffix";

    str.Appendf("%s %d %s", "prefix", 42, "suffix");

    ASSERT_EQ(strlen(expected), str.Size());
    ASSERT_EQ(0, strcmp(str.buffer, expected));
}
