#include <string.h>

#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.h>

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
