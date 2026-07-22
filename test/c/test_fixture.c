#include <jc_test.h>

typedef struct CFixture {
    int value;
    int torn_down;
} CFixture;

JC_TEST_FIXTURE_SETUP(CFixture)
{
    jc_test_fixture->value = 42;
}

JC_TEST_FIXTURE_TEARDOWN(CFixture)
{
    jc_test_fixture->torn_down = 1;
    EXPECT_EQ(42, jc_test_fixture->value);
}

TEST_F(CFixture, Initialized)
{
    ASSERT_EQ(42, jc_test_fixture->value);
    jc_test_fixture->value = 42;
}

TEST(CFixtureBehavior, Skip)
{
    SKIP();
}
