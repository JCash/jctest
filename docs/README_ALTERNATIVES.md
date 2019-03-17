---
nav_order: 5
---

# Alternatives

There are many alternative test frameworks out there

* [googletest](https://github.com/google/googletest) : Big Corp, Large Executable Size, Slow headers, C++11

* [greatest](https://github.com/silentbicycle/greatest) : Small, Header only, Easy to add testers, C89

* [utest](https://github.com/evolutional/utest) : Small, Header only, C89

* [doctest](https://github.com/onqtam/doctest) : Medium, Medium executable, Header Only, Slow headers, C++11

* [catch2](https://github.com/catchorg/Catch2) : Large, Very large executable, Header Only, Very Slow headers, C++11


## Special notes

### utest

* utest doesn't compile unless you include "\_stricmp" before the header

* catch2 is extremely slow, and the code size is extreme too

### Coding style

* jctest was designed to mimic gtest as closely as possible, to ease the transition.

* greatest and utest are quite alike in the setup and testing.

* doctest and catch2 has a similar code style (BDD?)
