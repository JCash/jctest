# Benchmarks

# Feature comparisons

| Feature vs Impl        | jc_test |  gtest  | greatest |  utest  | doctest |  catch2 |
|-----------------------:|---------|---------|----------|---------|---------|---------|
| Header only            |   yes   |    no   |    yes   |   yes   |   yes   |   yes   |
| C version              |   C99   |    -    |    C89   |   C89   |    -    |    -    |
| C++ version            |  C++98  |  C++11  |     -    |    -    |  C++11  |  C++11  |
| -Wall                  |   yes   |   yes   |    no    |   no    |   yes   |   yes   |
| -Weverything           |   yes   |    no   |    no    |   no    |   yes   |    no   |
| -pedantic              |   yes*  |    no   |    no    |   no    |   yes   |    no   |
| Lines of Code**        |  <800   |  10000+ |   <1000  |  <400   |  4000+  |  11000+ |
| Size of program***     |  16144  |  414608 |   19228  |  18280  |  146348 |  829572 |
| Compile time***        |  145ms  |  639ms  |   168ms  |   89ms  |  1891ms | 10965ms |


    *) Needs -Wno-global-constructors 
    **) Counting code lines in C/C++ files using cloc
    ***) A minimal test example equivalent

## Special notes

### utest

utest doesn't compile unless you include "\_stricmp" before the header


### Coding style

* jctest was designed to mimic gtest as closely as possible, to ease the transition.

* greatest and utest are quite alike in the setup and testing.

* doctest has a distinctly unique code structure.

* 