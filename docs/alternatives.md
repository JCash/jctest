---
nav_order: 5
---

# Alternatives

There are many alternative test frameworks out there. Here are some.
See [benchmarks](./benchmarks.md) for performance comparisons.

* [googletest](https://github.com/google/googletest) : Maintained by Google, C++11

* [greatest](https://github.com/silentbicycle/greatest) : Very small, Single header only, Easy to add tester functions, C89

* [utest](https://github.com/evolutional/utest) : Very small, Single header only, C89

* [doctest](https://github.com/onqtam/doctest) : Header Only, C++11

* [catch2](https://github.com/catchorg/Catch2) : Header Only, C++11


## Special notes

### utest

* utest doesn't compile unless you include "\_stricmp" before the header

### Coding style

* jctest was designed to mimic gtest as closely as possible, to ease the transition.

* greatest and utest are quite alike in the setup and testing.

* doctest and catch2 has a similar code style (BDD?)
