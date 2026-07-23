---
title: "Features"
Categories: ["api"]
weight: 1
---

## API

These frameworks were designed as subset replacements for Googletest. The assertion names are
shared by C11 and C++11; C generic comparisons accept scalar values, while C++ also accepts types
that provide the corresponding operator.

### C

The utest column refers to [sheredom/utest.h](https://github.com/sheredom/utest.h).

<div class="feature-table">

| Macro or capability | jc_test C | utest.h |
|---------------------:|-----------|---------|
| ASSERT_EQ/NE/LT/GT/LE/GE | Yes | Yes |
| ASSERT_ARRAY_EQ / ASSERT_ARRAY_EQ_LEN | Yes | `ASSERT_MEMEQ` |
| ASSERT_STREQ / ASSERT_STRNE | Yes | Yes |
| ASSERT_NEAR | Yes | Yes |
| ASSERT_DEATH | Yes | No |
| ASSERT_TRUE / ASSERT_FALSE | Yes | Yes |
| EXPECT_* variants | Yes | Yes |
| Skip current test | `SKIP` | `UTEST_SKIP` |
| Basic tests | `TEST` | `UTEST` |
| Fixtures | `TEST_F` | `UTEST_F` |
| Parameterized tests | No | No |
| Typed tests | No | No |

</div>

### C++

<div class="feature-table">

| Macro or capability | jc_test C++ | GoogleTest |
|---------------------:|-------------|------------|
| ASSERT_EQ/NE/LT/GT/LE/GE | Yes | Yes |
| ASSERT_ARRAY_EQ / ASSERT_ARRAY_EQ_LEN | Yes | No |
| ASSERT_STREQ / ASSERT_STRNE | Yes | Yes |
| ASSERT_NEAR | Yes | Yes |
| ASSERT_DEATH | Yes | Yes |
| ASSERT_TRUE / ASSERT_FALSE | Yes | Yes |
| EXPECT_* variants | Yes | Yes |
| Skip current test | `SKIP` | `GTEST_SKIP` |
| TEST | Yes | Yes |
| TEST_F | Yes | Yes |
| TEST_P | Yes | Yes |
| TYPED_TEST** | Yes | Yes |

</div>

`ASSERT_FLOAT_EQ` and `ASSERT_DOUBLE_EQ` are represented by `ASSERT_EQ`, with a margin of 4 ULP.

**) A maximum number of 4 test types per typed test


## Other frameworks

Here is a comparison with other C/C++ frameworks
regarding compile time and runtime.

<div class="feature-table feature-table-wide">

| Feature vs Impl        | jc_test |  gtest  | greatest |  utest  | doctest |  catch2 | snow 2  |
|-----------------------:|---------|---------|----------|---------|---------|---------|---------|
| Header only            |   yes   |    no   |    yes   |   yes   |   yes   |   yes   |   yes   |
| Version                | C11/C++11 | C++11 |    C89   |   C89   |  C++11  |  C++11  | c99/c11 |
| -Wall                  |   yes   |   yes   |    no    |   no    |   yes   |   yes   |   yes   |
| -Weverything           |   yes   |    no   |    no    |   no    |   yes   |    no   |    no   |
| -pedantic              |   yes*  |    no   |    no    |   no    |   yes   |    no   |    no   |
| Lines of Code**        |  ~1300  |  10000+ |   <1000  |  <400   |  4000+  |  11000+ |  ~1100  |
| Size of program***     |  27292  |  414608 |   19228  |  18280  |  146348 |  829572 |  23144  |
| Compile time***        |  217ms  |  600ms  |   141ms  |   86ms  |  1890ms | 10662ms |  216ms  |
| Run time***            |    3ms  |    3ms  |    6ms   |    5ms  |    3ms  |   4ms   |   3ms   |

</div>

1. * Needs -Wno-global-constructors
1. ** Counting code lines in C/C++ files using cloc
1. *** A minimal test example. "clang++ -O2". Picking the fastest time.
