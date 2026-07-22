---
title: "Main"
weight: 1
---

`jctest` provides single-header C11 and C++11 test frameworks implementing a GTest-like API.
The libraries were built as lightweight replacements for `googletest` and similar C frameworks.

Use `jc_test.h` from C and `jc_test.hpp` from C++. A C++ translation unit that still includes
`jc_test.h` is forwarded to `jc_test.hpp` for compatibility. The implementations are separate
runtimes and cannot both define `JC_TEST_IMPLEMENTATION` in the same executable.

## Minimal C example

{{% codefile file="/static/code/example_minimal.c" language="c" %}}

## Minimal C++ example

{{% codefile file="/static/code/example_minimal.cpp" language="cpp" %}}

Failures are reported immediately. At the end of a long run, every failed check is repeated with
its full test name and source location before the totals.

{{% figure
    src="main/images/test_example.png"
    caption="An example where all tests pass"
%}}


{{% figure
    src="main/images/test_example_fail.png"
    caption="An example of how failing tests look"
%}}
