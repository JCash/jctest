---
title: "Features"
Categories: ["api"]
weight: 1
---

## API

This framework was designed to be a (subset) replacement of the Googletest.

<small>

| Features     | jc_test |  gtest |
|-------------:|---------|--------|
| ASSERT_EQ    |   Yes   |   Yes  |
| ASSERT_NE    |   Yes   |   Yes  |
| ASSERT_GT    |   Yes   |   Yes  |
| ASSERT_LT    |   Yes   |   Yes  |
| ASSERT_GE    |   Yes   |   Yes  |
| ASSERT_LE    |   Yes   |   Yes  |
| ASSERT_ARRAY_EQ |   Yes   |   No  |
| ASSERT_ARRAY_EQ_LEN |   Yes   |   No  |
| ASSERT_STREQ |   Yes   |   Yes  |
| ASSERT_STRNE |   Yes   |   Yes  |
| ASSERT_NEAR |   Yes   |   Yes  |
| ASSERT_DEATH |   Yes   |   Yes  |
| ASSERT_FLOAT_EQ |   No* | Yes |
| ASSERT_DOUBLE_EQ |   No* | Yes |
| ASSERT_TRUE    |   Yes   |   Yes  |
| ASSERT_FALSE    |   Yes   |   Yes  |
| EXPECT_* variants   |   Yes   |   Yes  |
| TEST   |   Yes   |   Yes  |
| TEST_F   |   Yes   |   Yes  |
| TEST_P  |   Yes   |   Yes  |
| TYPED_TEST**  |   Yes   |   Yes  |

</small>

*) The `ASSERT_FLOAT_EQ` is replaced by `ASSERT_EQ` in this framework, with the same margin of 4 ULP.

**) A maximum number of 4 test types per typed test


## Other frameworks

Here is a comparison with other C/C++ frameworks
regarding compile time and runtime.

<small><small>

| Feature vs Impl        | jc_test |  gtest  | greatest |  utest  | doctest |  catch2 | snow 2  |
|-----------------------:|---------|---------|----------|---------|---------|---------|---------|
| Header only            |   yes   |    no   |    yes   |   yes   |   yes   |   yes   |   yes   |
| Version                |  C++98  |  C++11  |    C89   |   C89   |  C++11  |  C++11  | c99/c11 |
| -Wall                  |   yes   |   yes   |    no    |   no    |   yes   |   yes   |   yes   |
| -Weverything           |   yes   |    no   |    no    |   no    |   yes   |    no   |    no   |
| -pedantic              |   yes*  |    no   |    no    |   no    |   yes   |    no   |    no   |
| Lines of Code**        |  ~1100  |  10000+ |   <1000  |  <400   |  4000+  |  11000+ |  ~1100  |
| Size of program***     |  27292  |  414608 |   19228  |  18280  |  146348 |  829572 |  23144  |
| Compile time***        |  217ms  |  600ms  |   141ms  |   86ms  |  1890ms | 10662ms |  216ms  |
| Run time***            |    3ms  |    3ms  |    6ms   |    5ms  |    3ms  |   4ms   |   3ms   |

</small></small>

1. * Needs -Wno-global-constructors
1. ** Counting code lines in C/C++ files using cloc
1. *** A minimal test example. "clang++ -O2". Picking the fastest time.
