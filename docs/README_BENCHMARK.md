# Benchmarks

# Feature comparisons

| Feature vs Impl        | jc_test |  gtest  | greatest |  utest  | doctest |
|-----------------------:|---------|---------|----------|---------|---------|
| Header only            |   yes   |    no   |    yes   |   yes   |   yes   |
| C version              |   C99   |    -    |    c89   |   c89   |    -    |
| C++ version            |  C++98  |  C++11  |     -    |    -    |  C++11  |
| -Wall                  |   yes   |   yes   |    no    |   no    |         |
| -Weverything           |   yes   |    no   |    no    |   no    |         |
| -pedantic              |   yes*  |    no   |    no    |   no    |         |
| Lines of Code**        |  <800   |  10000+ |   <1000  |  <400   |  4000+  |
| Size of program***     |  16660  |  437100 |   19580  |  22464  | 305944  |
| Compile time***        |   91ms  |  606ms  |    92ms  |  78ms   |  899ms  |


    *) Needs -Wno-global-constructors 
    **) Counting C/C++ files using cloc
    ***) A minimal test example equivalent

## Special notes

### utest

utest doesn't compile unless you include "\_stricmp" before the header


### Coding style

jctest was designed to mimic gtest as closely as possible, to ease the transition.

greatest and utest are quite alike in the setup and testing.

doctest has a distinctly unique code structure.