---
title: "Runtime"
Categories: ["api"]
weight: 4
---

## Public functions

### jc_test_init

```c
void jc_test_init(int* argc, char** argv);
```

Initializes the runtime, detects color and debugger support, parses jc_test command-line options,
and removes recognized options from `argv`. Call it once before `jc_test_run_all()`.

### jc_test_run_all

```c
int jc_test_run_all(void);
```

Runs all registered tests that match the active filters, prints the failure recap and totals, and
returns the number of failed tests. A zero result means that no test failed.

These are the supported direct-call runtime functions. Other `jc_test_*` functions declared by the
headers support macro expansion and runtime internals and are not part of the stable user API.

## Command line options

### `--test-filter <pattern>`

If a pattern matches the full test name, the test will be included. Patterns containing `*` use
wildcard matching. Patterns without `*` retain substring matching for compatibility. Repeating
the option combines patterns with OR semantics.

The equivalent `--test-filter=<pattern>` and utest-compatible `--filter=<pattern>` forms are also
accepted. Recognized test arguments are removed from `argv`.

The complete name isn't written out during the tests, but is pieced together from the components making up the test. E.g.:

```text
[[[FirstParamsTest]]]/ParamsTest.Test/0
FirstParamsTest/[[[ParamsTest]]].Test/1
SecondParamsTest/ParamsTest.[[[Test]]]/0
SecondParamsTest/ParamsTest.Test/[[[1]]]
```

1. Fixture name
1. Test case name
1. Test name
1. Test number (for parameterized tests)

Given the tests above, the option `--test-filter Second` will match:

```text
SecondParamsTest/ParamsTest.Test/0
SecondParamsTest/ParamsTest.Test/1
```

The option `--test-filter Test/0` will match:

    FirstParamsTest/ParamsTest.Test/0
    SecondParamsTest/ParamsTest.Test/0

The option `--filter=*ParamsTest.Test1/0` uses wildcard matching.

### `--test-break-on-fail`

Triggers `JC_TEST_DBG_BREAK()` whenever an assertion fails. The runtime also enables this behavior
automatically when `JCT_IS_DEBUGGER_ATTACHED()` reports an attached debugger.
