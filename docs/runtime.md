---
nav_order: 3
---

# API - Run time

## Command line options

### `--test-filter <substring>`

If the substring matches part of the full test name, the test will be included.
The non matching tests will be skipped.

The complete name isn't written out during the tests, but is pieced together from the `fixture name`, `test name` and the `test number`. E.g.:

    FirstParamsTest/ParamsTest.Test/0
    FirstParamsTest/ParamsTest.Test/1
    SecondParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/1

The option `--test-filter Second` will match:

    SecondParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/1

The option `--test-filter Test/0` will match:

    FirstParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/0

