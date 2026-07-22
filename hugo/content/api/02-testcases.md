---
title: "Test Cases"
Categories: ["api"]
weight: 3
---

## Test Fixtures

`TEST` has the same spelling in C and C++. C uses function-pointer test descriptors; C++ uses
the class-based implementation in `jc_test.hpp`.

### TEST(FixtureName, TestName)

Tests a single function.

```cpp
TEST(MyTest, TestName) {
    ASSERT_EQ(4, 2*2);
    ASSERT_STREQ("a", "b");
}
```


### TEST_F(FixtureName, TestName)

In C++, `TEST_F` uses a base class:

```cpp
struct MyTest : public jc_test_base_class {
    static void SetUpTestCase()     {};
    static void TearDownTestCase()  {};
    void SetUp()                    {};
    void TearDown()                 {};
};
TEST_F(MyTest, TestName) {
    ASSERT_EQ(4, 2*2);
    ASSERT_STREQ("a", "b");
}
```

In C11, fixtures are zero-initialized structures. Setup and teardown definitions are required,
even when their bodies are empty. Each body receives an explicit, typed `jc_test_fixture` pointer.

```c
typedef struct MyTest {
    int value;
} MyTest;

JC_TEST_FIXTURE_SETUP(MyTest) {
    jc_test_fixture->value = 42;
}

JC_TEST_FIXTURE_TEARDOWN(MyTest) {
    EXPECT_EQ(42, jc_test_fixture->value);
}

TEST_F(MyTest, TestName) {
    ASSERT_EQ(42, jc_test_fixture->value);
}
```

Teardown runs after setup begins, including after a fatal setup or test-body assertion. A fatal
setup assertion skips the body.

### TEST_P + INSTANTIATE_TEST_CASE_P

Parameterized tests are available in C++ only.

Using a templated base class which accepts a parameter type,
it's easy to use a test case with multiple values.

```cpp
struct TestParam {
    int value;
};
struct MyParamTest : public jc_test_params_class<TestParam> {
public:
    static void SetUpTestCase()     {};
    static void TearDownTestCase()  {};
    void SetUp()                    {};
    void TearDown()                 {};
};
TEST_P(MyParamTest, TestParamIsEven) {
    ParamType value = GetParam();
    ASSERT_EQ(0, value & 1);
}
const TestParam low_values[] = { {0}, {2}, {4} };
INSTANTIATE_TEST_CASE_P(LowNumbers, MyParamTest, jc_test_values_in(low_values));
const TestParam high_values[] = { {100}, {102}, {104} };
INSTANTIATE_TEST_CASE_P(HighNumbers, MyParamTest, jc_test_values_in(high_values));
```

### TYPED_TEST + INSTANTIATE_TEST_CASE_P

Typed tests are available in C++ only.

If you wish to use a different base class to use with your tests, you can do so.
You can specify up to 4 classes via the constructs `jc_test_type1` ... `jc_test_type4`

```cpp
// First, create the classes you wish to test:
struct TestClass1 {
    int value;
    TestClass1() : value(2) {}
};
struct TestClass2 {
    int value;
    TestClass2() : value(4) {}
};
typedef jc_test_type2<TestClass1, TestClass2> TestTypes;
TYPED_TEST_SUITE(TypedTest, TestTypes);

//Then, create the test cases:

TYPED_TEST(TypedTest, NonZero)
{
    ASSERT_NE(0, TestFixture::instance.value);
}
TYPED_TEST(TypedTest, Even)
{
    ASSERT_EQ(0, TestFixture::instance.value & 0x1);
}
```

## Base classes

### jc_test_base_class

Used in conjunction with `TEST_F`

### template <typename ParamType> jc_test_params_class<ParamType>

Only used in conjunction with `TEST_P`

This class implements a function `GetParam()` which is used to get the current parameter value.

```cpp
template <typename ParamType>
jc_test_params_class {
    const ParamType& GetParam();
}
```
