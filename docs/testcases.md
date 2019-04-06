---
nav_order: 1
---

# API - Test Cases

## Test Fixtures

### TEST(FixtureName, TestName)

Tests a single function.

```cpp
TEST(MyTest, TestName) {
    ASSERT_EQ(4, 2*2);
    ASSERT_STREQ("a", "b");
}
```


### TEST_F(FixtureName, TestName)

Uses a base class:

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

## Run Time Control

### Command line options

#### `--test-filter <substring>`

If the substring matches part of the full test name, the test will be included.
The non matching tests will be skipped.

The complete name isn't written out during the tests, but is pieced together from the `fixture name`, `test name` and the `test number`. E.g.:

    FirstParamsTest/ParamsTest.Test/0
    FirstParamsTest/ParamsTest.Test/1
    SecondParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/1

The option `--test-filter Second` will match:

    SecondParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/1

The option `--test-filter Test/0` will match:

    FirstParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/0

