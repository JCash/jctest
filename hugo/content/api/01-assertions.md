---
title: "Assertions"
Categories: ["api"]
weight: 2
---

There are two main types of assertions: fatal (`ASSERT_*`) and non-fatal (`EXPECT_*`).

The general format of each assertion is that the expected value is to the left,
and the value to test is on the right. This is good to know so that the error reporting doesn't get confusing.

```cpp
    ASSERT_EQ(expected, actual);
```

## Boolean checks

<small>

|       | Fatal           | Non fatal       | Expression     |
|------:|-----------------|-----------------|----------------|
| True  | ASSERT_TRUE(V)  | EXPECT_TRUE(V)  | V is TRUE      |
| False | ASSERT_FALSE(V) | EXPECT_FALSE(V) | !(V is TRUE)   |

</small>

```cpp
TEST(Example, TestValue) {
    int number = 17;
    ASSERT_TRUE(IsOdd(number));
    ASSERT_FALSE(IsEven(number));
}
```

## Value comparison checks

Value A vs B checks.
Supports any type that has the correct operator defined.
It also supports types `float` and `double` within the precision of 4 `ulp`.

<small>

|                    | Fatal           | Non fatal       | Expression |
|-------------------:|-----------------|-----------------|------------|
| Equal              | ASSERT_EQ(A, B) | EXPECT_EQ(A, B) | A == B     |
| Not Equal          | ASSERT_NE(A, B) | EXPECT_NE(A, B) | A != B     |
| Less Than          | ASSERT_LT(A, B) | EXPECT_LT(A, B) | A  < B     |
| Greater Than       | ASSERT_GT(A, B) | EXPECT_GT(A, B) | A  > B     |
| Less Than Equal    | ASSERT_LE(A, B) | EXPECT_LE(A, B) | A <= B     |
| Greater Than Equal | ASSERT_GE(A, B) | EXPECT_GE(A, B) | A >= B     |

</small>


```cpp
TEST(Example, TestValue) {
    int number = 21;
    ASSERT_EQ(42, Multiply(number, 2));
    ASSERT_NE(42, Div(number, 3));

    ASSERT_EQ(0,       GetPointer());
    ASSERT_EQ(nullptr, GetPointer()); // If C++11
}
```

## Floating point comparison

Compares the difference of two floating point values and an error limit.

<small><small>

|                    | Fatal           | Non fatal       | Expression |
|-------------------:|-------------------|-----------------|------------|
| Near               | ASSERT_NEAR(A, B, E) | EXPECT_NEAR(A, B, E) | ABS(A - B) <= EPSILON     |

</small></small>


```cpp
TEST(Example, TestValue) {
    ASSERT_NEAR(10.0f, Multiply(5.0f, 2.0f), 0.001f);
}
```

## String assertions

Compares two null terminated strings.
The comparisons are case sensitive.

<small><small>

|              | Fatal              | Non fatal          | Expression |
|-------------:|--------------------|--------------------|------------|
| Equal        | ASSERT_STREQ(A, B) | EXPECT_STREQ(A, B) | A == B     |
| Not Equal    | ASSERT_STRNE(A, B) | EXPECT_STRNE(A, B) | A != B     |

</small></small>

```cpp
TEST(Example, TestStrings) {
    const char* s = "Hello World!";
    ASSERT_STREQ("World!", p+6);
    ASSERT_STRNE("World!", p+8);
}
```

## Array assertions

Tests two arrays of equal length `N`, or `LEN` if specified.


<small><small><small>

|              | Fatal              | Non fatal          | Expression |
|-------------:|--------------------|--------------------|------------|
| Equal | ASSERT_ARRAY_EQ(A, B) | EXPECT_ARRAY_EQ(A, B) | A[0..N] == B[0..N]     |
| Equal | ASSERT_ARRAY_EQ_LEN(A, B, N) | EXPECT_ARRAY_EQ_LEN(A, B, N) | A[0..N] == B[0..N]    |

</small></small></small>

```cpp
TEST(Example, TestArrays) {
    const char a[] = "HELLO WORLD";
    const char b[] = "HELLO WORLD";
    ASSERT_ARRAY_EQ(a, b);

    const float a_float[] = {0,1,2,3,4,5,6,7,8,9};
    const float b_float[] = {0,1,2,3,4,5,6,7,8,9};
    EXPECT_ARRAY_EQ_LEN(a_float, b_float, 5);
}
```

## Death test

Tests that the `STATEMENT` causes a signal error (e.g `SIGABORT`).
Uses `setjmp`/`longjmp` to recover from the signal handling.

Note: The STRING argument is there to make the transition from GTEST easier. It is not used in this framework


<small><small><small>

|               | Fatal              | Non fatal          | Expression |
|--------------:|--------------------|--------------------|------------|
| Raises signal | ASSERT_DEATH(STATEMENT, STRING) | ASSERT_DEATH(STATEMENT, STRING) | STATEMENT     |

</small></small></small>

```cpp
void call_assert(bool v) {
    assert(v && "call_assert called with false");
}

TEST(Example, TestDeath)
{
    ASSERT_DEATH(call_assert(false),"");
    //.. code continues
    ASSERT_EQ(4, 2 * 2);
}
```