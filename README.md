
![License](https://img.shields.io/github/license/JCash/jctest?label=License) ![Version](https://img.shields.io/github/v/release/JCash/jctest?color=blue&label=Version&sort=semver) ![C header size](https://img.shields.io/github/size/JCash/jctest/src/jc_test.h?color=green&label=jc_test.h) ![C++ header size](https://img.shields.io/github/size/JCash/jctest/src/jc_test.hpp?color=green&label=jc_test.hpp) [![Build Status](https://github.com/JCash/jctest/actions/workflows/build.yml/badge.svg)](https://github.com/JCash/jctest/actions/workflows/build.yml)

# Documentation

https://jcash.github.io/jctest/ (or [here](./docs/index.html))

# jctest

A pair of single-header test frameworks with a GTest-like API:

* `jc_test.h` is a C11 function-pointer implementation.
* `jc_test.hpp` is the C++11 implementation with fixtures, parameterized tests, and typed tests.

For compatibility, including `jc_test.h` from C++ forwards to `jc_test.hpp`. The C and C++ implementations are separate runtimes and must not both define `JC_TEST_IMPLEMENTATION` in one executable.

```c
#define JC_TEST_IMPLEMENTATION
#include <jc_test.h>

TEST(Math, Multiply) {
    ASSERT_EQ(4, 2 * 2);
}

int main(int argc, char** argv) {
    jc_test_init(&argc, argv);
    return jc_test_run_all();
}
```

Failed assertions are printed immediately and repeated with their full test names in the final summary.

Build and run the C tests with:

```bash
scripts/compile_c.sh
scripts/run_c_tests.sh
```

Build and run the C++ tests separately with:

```bash
STDVERSION=c++11 scripts/compile.sh
scripts/run_tests.sh
```

Set `CSTDVERSION` when checking a newer C language mode, for example:

```bash
CSTDVERSION=c23 scripts/compile_c.sh
scripts/run_c_tests.sh
```

See [documentation](https://jcash.github.io/jctest) for a full list of features

# Disclaimer

This software is supplied "AS IS" without any warranties and support

# License

[The MIT license](http://choosealicense.com/licenses/mit/)

# Contributing

* Make pull requests towards the `dev` branch.
* All tests must pass (on Travis-CI)
* No added warnings to the code (according to Travis-CI)
* New features must have unit tests

As a general rule, take care to keep the performance high in this library.

# Code of conduct

[Contributor covenant](./CODE_OF_CONDUCT.md)
