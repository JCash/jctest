---
nav_order: 2
---

# API - Assertions

There are two main type of assertions:

* ASSERT_* - fatal asserts
* EXPECT_* - non fatal asserts

The general format of each assertion is that the expected value is to the left,
and the value to test is on the right.

```cpp
    ASSERT_EQ(expected, actual);
```

Below is a full list of assertions

## ASSERT_* | EXPECT_*

The `ASSERT_*` macros are fatal, the `EXPECT_*`macros are non fatal.

Boolean assertions:

* ASSERT_TRUE(expr)
* ASSERT_FALSE(expr)

Value A vs B checks. E.g. int, float, double or any type that has the correct operator defined

* ASSERT_EQ(expected, value) -> ==
* ASSERT_NE(expected, value) -> !=
* ASSERT_LT(expected, value) ->  <
* ASSERT_GT(expected, value) ->  >
* ASSERT_LE(expected, value) -> <=
* ASSERT_GE(expected, value) -> >=
* ASSERT_NEAR(expected, value, epsilon) -> abs(expected - value) < epsilon

## String assertions:

* ASSERT_STREQ(expected, value) -> Tests two null terminated strings for equality
* ASSERT_STRNE(expected, value) -> Tests two null terminated strings for inequality

## Floating point assertions

### ASSERT_EQ, ASSERT_NE

In this framework the comparison is done by comparing the bits in each float number.

### ASSERT_NEAR( A, B, EPSILON )

Asserts that `fabs(b - a) <= epsilon`

## ASSERT_DEATH( STATEMENT, STRING )

Tests that the `STATEMENT` causes a signal error (e.g SIGABORT)

*NOTE: The STRING argument is there to make the transition from GTEST easier. It is not used in this framework*
