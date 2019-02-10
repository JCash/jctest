
#include <string.h>
#include <stdio.h>
#include "testutil.h"

// To test the testing framework /////////////
void GlobalTestSetup(); // to silence a warning
void GlobalTestSetup() {
    printf("Setting up the test\n");
}
int GlobalTestTeardown(); // to silence a warning
int GlobalTestTeardown() {
    printf("Verifying the the test\n");
    return 0;
}
//////////////////////////////////////////////

struct TestClass1 {
    int value;
    int _pad;
    TestClass1() : value(2) {}
};

struct TestClass2 {
    int value;
    int _pad;
    TestClass2() : value(4) {}
};

//////////////////////////////////////////////

#if defined(USE_GTEST)
    typedef ::testing::Types<TestClass1, TestClass2> TestTypes;
#else
    typedef jc_test_type2<TestClass1, TestClass2> TestTypes;
#endif

TYPED_TEST_SUITE(TypedTest, TestTypes);

//////////////////////////////////////////////

template<typename T>
class TypedTest : public jc_test_base_class
{
protected:
    virtual void SetUp();
    virtual void TearDown();

    T instance;
};

template<typename T>
void TypedTest<T>::SetUp() {
}

template<typename T>
void TypedTest<T>::TearDown() {
}


TYPED_TEST(TypedTest, NonZero)
{
    ASSERT_NE(0, TestFixture::instance.value);
}

TYPED_TEST(TypedTest, Even)
{
    ASSERT_EQ(0, TestFixture::instance.value & 0x1);
}

//////////////////////////////////////////////

