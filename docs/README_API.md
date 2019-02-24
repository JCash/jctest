---
nav_order: 2
---

# C++ API

## Test Fixtures

### TEST(FixtureName, TestName)

Tests a single function.

### TEST_F(FixtureName, TestName)

Uses a base class:

```cpp
// a comment
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

### TEST_P + INSTANTIATE_TEST_CASE_P

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

If you wish to use a different base class to use with your tests, you can do so with too.


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

## Run Time Control

### Command line options

TBD

### Environment variables

