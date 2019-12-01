---
title: "Benchmarks"
weight: 4
---


## Minimal test example

A minimal test example was written specifically for each framework.
You can find them in the [jctest/test/comparisons](https://github.com/JCash/jctest/tree/master/test/comparisons) folder. The tests were compiled with `clang++ -O2`

Comparisons are done using no configurations of the testing framework.
Also, since the goal was to eliminate a precompiled library, the header only version was chosen where available.

{{% expandable label="example_minimal.cpp" %}}
{{% codefile file="/static/code/example_minimal.cpp" language="cpp" %}}
{{% /expandable %}}


## Benchmark

<small><small>

|                 | jc_test |  gtest  | greatest |  utest  | doctest |  catch2 | snow 2  |
|----------------:|---------|---------|----------|---------|---------|---------|---------|
| Size of program |  27292  |  414608 |   19228  |  18280  |  146348 |  829572 |  23144  |
| Compile time    |  217ms  |  600ms  |   141ms  |   86ms  |  1890ms | 10662ms |  216ms  |
| Run time*       |    3ms  |    3ms  |    6ms   |    5ms  |    3ms  |   4ms   |   3ms   |

</small></small>

*) Picking the fastest time.



## Real life examples

In our case (the [Defold](www.defold.com) game engine), we wanted to replace `GTEST` with something new.

Here's a comparison between the `GTEST` and `jctest` frameworks when building the game engine tests.
We see a decrease of the compile time, ending up at ~54% of the original time.

![benchmark_enginetests.png](/images/benchmark_enginetests.png)

The footprint on disc is also important, as it takes up both space but also affects writing/reading times.
In this scenario the file sizes ended up at ~45% of the original sizes.

![benchmark_enginetests.png](/images/benchmark_enginetestsizes.png)

