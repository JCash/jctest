# Benchmarks

# Feature comparisons

| Feature vs Impl        |  gtest  | jc_test |
|-----------------------:|---------|---------|
| Header only            |    no   |   yes   |
| C++98                  |    no   |   yes   |
| -Wall                  |    no   |   yes   |
| -Weverything           |    no   |   yes   |
| -pedantic              |    no   |   yes*  |
| Lines of Code*         |  10000+ |  <800   |
| Size of program**      | ~1200kb |  ~30kb  |

*) Needs -Wno-global-constructors 
*) Counting C/C++ files using cloc
**) A minimal test example with one test