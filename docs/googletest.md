---
nav_order: 4
---

# jc_test vs googletest

## Design

This testing framework software was written to speed up the compilation
times of a particular software.

It may very well be the case that there are nuances to the supported test classes and functions
that are intentionally/unintentionally left out or simply work slightly differently.

While `jc_test` is designed to be a replacement for gtest,
the base classes are named differently, so you need some setup to make it work.

The major goal with this software was to see if it was possible to decrease the compilation
times when compiling tests. See [./benchmarks.md](the benchmarks) for the results.

A secondary goal was to keep the code reasonably small, in order to make it into a single header only library.
As such, it wouldn't be necessary to cross compile the testing framework for many different target platforms.


## Base classes

The classes correspond like so:

    class ::testing::Test  -> class jc_test_base_class

    template<typename T>                    template<typename T>
    public ::testing::TestWithParam<T> ->   class jc_test_params_class

    ::testing::ValuesIn -> jc_test_values_in

## ASSERT_DEATH_IF_SUPPORTED

`jc_test` is using a signal handler to catch errors such as SIGABRT.
The application is not forked, and the output from assert() is output in the log.

##