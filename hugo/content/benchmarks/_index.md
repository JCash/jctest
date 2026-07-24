---
title: "Benchmarks"
weight: 4
---


## Methodology

The reproducible benchmark generates equivalent 5,000-test suites for each framework. C frameworks
are compiled as C11 with `clang -O2`; C++ frameworks are compiled as C++11 with `clang++ -O2`.

Each suite is measured in three runtime modes: one filtered passing test, all passing tests, and all
failing tests. The report also records preprocessed line count, compile time, object size, and
executable size. Runtime output is suppressed while measuring. Compile time uses one warmup and the
median of three measured runs; runtime uses two warmups and the median of ten measured runs.

The compile metric covers the generated test translation unit. Unity and GoogleTest normally use a
separately compiled support library, so that one-time library build is excluded from compile time.
Its object bytes are still included in the object-size total, and its linked code is included in the
executable size. Header-only frameworks compile their runner as part of the measured translation
unit. All runtime measurements execute tests in-process; Acutest is invoked with `--no-exec` to
disable its default per-test child-process isolation.

{{% details title="Minimal C example" closed="true" %}}
{{% codefile file="/static/code/example_minimal.c" language="c" %}}
{{% /details %}}

{{% details title="Minimal C++ example" closed="true" %}}
{{% codefile file="/static/code/example_minimal.cpp" language="cpp" %}}
{{% /details %}}

The all-fail jctest cases retain and print every failure again in the final recap. They therefore do
additional allocation and formatting work that the comparison frameworks may not perform. Results
are informational rather than timing gates in CI.

## Reproduce the benchmark

```bash
python3 test/comparisons/benchmark.py \
    --utest-dir /path/to/utest.h-checkout \
    --greatest-dir /path/to/greatest-checkout \
    --unity-dir /path/to/Unity-checkout \
    --acutest-dir /path/to/acutest-checkout \
    --gtest-dir /path/to/googletest-checkout \
    --doctest-dir /path/to/doctest-checkout \
    --catch2-dir /path/to/Catch2-v2-checkout \
    --output build/comparisons/results.md
```

The report records the platform, compiler versions, workload, and exact dependency commits. The
generated sources, object files, and executables are kept under `build/comparisons` for inspection.

## Reference run: 2026-07-23

Apple Clang 21.0.0 on arm64 macOS 27.0. GoogleTest 1.12.1 and Catch2 2.13.10
are the last release lines of those frameworks that support C++11. Exact revisions for every
dependency are recorded by the generated report.

{{% details title="External framework revisions" closed="true" %}}

- [utest.h](https://github.com/sheredom/utest.h): `ebb62ed381c4f32ef2ddafb56890bcae1472ad9d`
- [greatest](https://github.com/silentbicycle/greatest): `11a6af1919049df502405913da64fb385c219361`
- [Unity](https://github.com/ThrowTheSwitch/Unity): `3a6eb6dfd7706b703adf60f5ce3bcad57f94de4f`
- [Acutest](https://github.com/mity/acutest): `31751b4089c93b46a9fd8a8183a695f772de66de`
- [GoogleTest 1.12.1](https://github.com/google/googletest/tree/release-1.12.1): `58d77fa8070e8cec2dc1ed015d66b454c8d78850`
- [doctest 2.4.12](https://github.com/doctest/doctest/tree/v2.4.12): `1da23a3e8119ec5cce4f9388e91b065e20bf06f5`
- [Catch2 2.13.10](https://github.com/catchorg/Catch2/tree/v2.13.10): `182c910b4b63ff587a3440e08f84f70497e49a81`

{{% /details %}}

### C frameworks

| Framework | Preprocessed lines | Compile | Object | Executable | Single filter | All pass | All fail |
|---|---:|---:|---:|---:|---:|---:|---:|
| jc_test C | 8,675 | 11.053856s | 1,729,648 | 988,608 | 0.002821s | 0.003776s | 0.009019s |
| utest.h | 118,315 | 14.368594s | 4,441,472 | 2,239,112 | 0.003942s | 0.003859s | 0.005930s |
| greatest | 12,499 | 1.863712s | 607,464 | 317,136 | 0.003098s | 0.009336s | 0.010399s |
| Unity | 11,634 | 5.135419s | 1,859,872 | 749,816 | 0.003487s | 0.007473s | 0.010356s |
| Acutest | 15,172 | 0.674482s | 1,473,560 | 736,144 | 0.003074s | 0.008260s | 0.010067s |

### C++ frameworks

| Framework | Preprocessed lines | Compile | Object | Executable | Single filter | All pass | All fail |
|---|---:|---:|---:|---:|---:|---:|---:|
| jc_test C++ | 12,508 | 3.688477s | 4,705,720 | 3,272,704 | 0.014315s | 0.022721s | 0.029285s |
| GoogleTest | 75,200 | 25.334601s | 11,501,576 | 8,639,832 | 0.005366s | 0.013761s | 0.025260s |
| doctest | 174,853 | 26.525474s | 9,808,888 | 6,117,264 | 0.004898s | 0.013832s | 0.027865s |
| Catch2 v2 | 204,293 | 12.751911s | 6,255,336 | 4,120,632 | 0.005428s | 0.013625s | 0.049609s |

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
