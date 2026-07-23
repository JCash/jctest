---
title: "Benchmarks"
weight: 4
---


## Minimal test example

A minimal test example was written specifically for each framework.
You can find them in the [jctest/test/comparisons](https://github.com/JCash/jctest/tree/master/test/comparisons) folder. The tests were compiled with `clang++ -O2`

Comparisons are done using no configurations of the testing framework.
Also, since the goal was to eliminate a precompiled library, the header only version was chosen where available.

{{% details title="example_minimal.cpp" closed="true" %}}
{{% codefile file="/static/code/example_minimal.cpp" language="cpp" %}}
{{% /details %}}


## Benchmark

|                 | jc_test |  gtest  | greatest |  utest  | doctest |  catch2 | snow 2  |
|----------------:|---------|---------|----------|---------|---------|---------|---------|
| Size of program |  27292  |  414608 |   19228  |  18280  |  146348 |  829572 |  23144  |
| Compile time    |  217ms  |  600ms  |   141ms  |   86ms  |  1890ms | 10662ms |  216ms  |
| Run time*       |    3ms  |    3ms  |    6ms   |    5ms  |    3ms  |   4ms   |   3ms   |

*) Picking the fastest time.

## Reproducible large-suite benchmark

`test/comparisons/benchmark.py` generates equivalent C jc_test, C++ jc_test, and utest suites.
Its default workload follows the 5,000-test comparison: one filtered test, all passing tests, and
all failing tests. It also reports preprocessed lines, compile time, object size, and executable
size. Runtime output is suppressed while measuring.

```bash
python3 test/comparisons/benchmark.py \
    --utest-dir /path/to/utest.h-checkout \
    --output build/comparisons/results.md
```

The report records the platform, compiler versions, flags implied by the script, and utest commit.
Each runtime metric uses two warmups and the median of ten measured runs. Results are informational
and are not used as timing gates in CI.

### Reference run: 2026-07-22

Apple Clang 21.0.0 on arm64 macOS 27.0, using utest commit
`ebb62ed381c4f32ef2ddafb56890bcae1472ad9d`:

| Framework | Preprocessed lines | Compile | Object | Executable | Single filter | All pass | All fail |
|---|---:|---:|---:|---:|---:|---:|---:|
| jc_test C | 8,675 | 11.821657s | 1,729,648 | 988,608 | 0.003759s | 0.003367s | 0.009799s |
| jc_test C++ | 12,530 | 3.899378s | 4,704,784 | 3,272,496 | 0.015803s | 0.022645s | 0.029211s |
| utest C | 118,315 | 14.978207s | 4,441,472 | 2,239,112 | 0.004511s | 0.004451s | 0.006410s |

The jc_test all-fail cases also retain and print every failure again in the final recap, so they
perform additional allocation and formatting work not present in the utest workload.



## Real life examples

In our case (the [Defold](https://www.defold.com) game engine), we wanted to replace `GTEST` with something new.

Here's a comparison between the `GTEST` and `jctest` frameworks when building the game engine tests.
We see a decrease of the compile time, ending up at ~54% of the original time.


{{% figure
    src="benchmarks/images/benchmark_enginetests.png"
    caption="Compile times"
%}}

The footprint on disc is also important, as it takes up both space but also affects writing/reading times.
In this scenario the file sizes ended up at ~45% of the original sizes.

{{% figure
    src="benchmarks/images/benchmark_enginetestsizes.png"
    caption="Unit test sizes"
%}}
