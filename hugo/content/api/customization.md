---
title: "Customization"
weight: 5
---

## Implementation and main

### JC_TEST_IMPLEMENTATION

Define `JC_TEST_IMPLEMENTATION` before including the appropriate header in exactly one translation
unit. Other translation units include the header without the definition. The C and C++ headers use
separate runtimes and must not both provide their implementations in one executable.

### JC_TEST_USE_DEFAULT_MAIN

Define `JC_TEST_USE_DEFAULT_MAIN` to provide `main()`, initialize jc_test, and run every registered
test. This also enables `JC_TEST_IMPLEMENTATION`, so do not provide another implementation in the
same executable.

## Output

### JC_TEST_OUTPUT_FN

Overrides the byte-oriented output function in both runtimes. Define it to the name of a function
with this signature before including the implementation:

```c
void custom_output(const char* text, size_t length);
```

The buffer is not required to be null-terminated; always honor `length`.

### JC_TEST_LOGGER_CLASS

C++ only. Replaces the default `jc_test_print_logger` class. The replacement must provide the same
constructor, destructor, fixture/test callbacks, summary callback, failure callback, and
`Log`/`Logf` interface. Prefer `JC_TEST_OUTPUT_FN` when only the output destination changes.

### JC_TEST_FLUSH_OUTPUT

C only. Overrides the function-like macro called after immediate failures and the final summary.
Custom buffered output functions can use it to flush their destination.

### JC_TEST_USE_PRINTF

Uses `printf` instead of `fwrite` in the default output implementation. It has no effect when
`JC_TEST_OUTPUT_FN` is supplied.

## Runtime configuration

Define these macros before including the implementation header.

| Macro | Availability | Purpose |
|-------|--------------|---------|
| `JC_TEST_USE_COLORS` | C and C++ | Set to `1` or `0` to force ANSI colors on or off instead of detecting a terminal. |
| `JC_TEST_NO_DEATH_TEST` | C and C++ | Disable death-test signal handling. Death assertions then compile as no-ops. |
| `JC_TEST_TIMING_FUNC` | C and C++ | Replace the microsecond timing function used around tests. |
| `JC_TEST_DBG_BREAK()` | C and C++ | Replace the debugger trap used by `--test-break-on-fail`. |
| `JCT_IS_DEBUGGER_ATTACHED()` | C and C++ | Return nonzero when break-on-failure should be enabled automatically. |
| `JC_TEST_FAILURE_MALLOC` | C and C++ | Allocate failure-recap records and messages. |
| `JC_TEST_FAILURE_REALLOC` | C and C++ | Grow formatted failure messages. |
| `JC_TEST_FAILURE_FREE` | C and C++ | Release failure-recap allocations. |
| `JC_TEST_ASSERT_FN` | C++ only | Replace internal invariant assertions. |
| `JC_TEST_EXIT` | C and C++ | Replace process termination used for invalid runtime arguments. |
| `JC_TEST_SNPRINTF` | C++ only | Replace the platform `snprintf` spelling. |
| `JC_TEST_ISATTY` | C++ only | Replace terminal detection used by automatic color selection. |

The three failure-allocation hooks must be supplied as a compatible set. If retaining detailed
failure records runs out of memory, tests continue and the summary reports how many details were
dropped.

When replacing `JC_TEST_TIMING_FUNC` in C++, define `jc_test_time_t` before including the header as
well. The function must return monotonically increasing microseconds in that type. C always uses
the provided `uint64_t` `jc_test_time_t`.

## Custom type formatting

Custom value formatters are a C++ feature of `jc_test.hpp`. The C11 generic assertions intentionally
accept the documented scalar set and reject aggregate values at compile time.

It's beneficial to be able to convert your custom types to a human readable string.
If your type doesn't match any of the supported types, it will default to the `"?"` output, which isn't very helpful.

All that is needed it to make a function available for the template matching to find it.

Here is an example that takes a custom enum and converts that to a readable output:

{{% codefile file="/static/code/example_custom_print.cpp" language="cpp" %}}

```bash
# Compile the app
$ clang++ -I src hugo/static/code/example_custom_print.cpp

# Run the app
$ ./a.out
```

{{% figure
    src="images/example_custom_print.png"
%}}
