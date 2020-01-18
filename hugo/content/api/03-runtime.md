---
title: "Runtime"
Categories: ["api"]
weight: 4
---

## Command line options

### `--test-filter <substring>`

If the substring matches part of the full test name, the test will be included.
The non matching tests will be skipped.

The complete name isn't written out during the tests, but is pieced together from the components making up the test. E.g.:

{{< code numbered=true >}}
[[[FirstParamsTest]]]/ParamsTest.Test/0
FirstParamsTest/[[[ParamsTest]]].Test/1
SecondParamsTest/ParamsTest.[[[Test]]]/0
SecondParamsTest/ParamsTest.Test/[[[1]]]
{{< /code >}}

1. Fixture name
1. Test case name
1. Test name
1. Test number (for parameterized tests)

Given the tests above, the option `--test-filter Second` will match:

{{< code language='c++' >}}
SecondParamsTest/ParamsTest.Test/0
SecondParamsTest/ParamsTest.Test/1
{{< /code >}}

The option `--test-filter Test/0` will match:

    FirstParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/0

