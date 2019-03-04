---
nav_order: 4
---

# Benchmarks

# Feature comparisons

| Feature vs Impl        | jc_test |  gtest  | greatest |  utest  | doctest |  catch2 |
|-----------------------:|---------|---------|----------|---------|---------|---------|
| Header only            |   yes   |    no   |    yes   |   yes   |   yes   |   yes   |
| C version              |    -    |    -    |    C89   |   C89   |    -    |    -    |
| C++ version            |  C++98  |  C++11  |     -    |    -    |  C++11  |  C++11  |
| -Wall                  |   yes   |   yes   |    no    |   no    |   yes   |   yes   |
| -Weverything           |   yes   |    no   |    no    |   no    |   yes   |    no   |
| -pedantic              |   yes*  |    no   |    no    |   no    |   yes   |    no   |
| Lines of Code**        |  <900   |  10000+ |   <1000  |  <400   |  4000+  |  11000+ |
| Size of program***     |  22808  |  414608 |   19228  |  18280  |  146348 |  829572 |
| Compile time***        |  145ms  |  639ms  |   168ms  |   89ms  |  1891ms | 10965ms |
| Run time***            |    3ms  |    3ms  |    6ms   |    5ms  |    3ms  |   4ms   |


1. * Needs -Wno-global-constructors 
1. ** Counting code lines in C/C++ files using cloc
1. *** A minimal test example. "clang++ -O2". Picking the fastest time.
