|Branch      | OSX / Linux / Windows     |
|------------|---------------------------|
|master      | [![Build Status](https://travis-ci.org/JCash/jctest.svg?branch=master)](https://travis-ci.org/JCash/jctest?branch=master) |
|dev         | [![Build Status](https://travis-ci.org/JCash/jctest.svg?branch=dev)](https://travis-ci.org/JCash/jctest?branch=dev) |

# Documentation

https://jcash.github.io/jctest/ (or [here](./docs/index.md))

# jctest

A C++ header only test framework

See [documentation](https://jcash.github.io/jctest) for a full list of features

# Disclaimer

This software is supplied "AS IS" without any warranties and support

# License

[The MIT license](http://choosealicense.com/licenses/mit/)

# Contributing

* Make pull requests towards the `dev` branch.
* All tests must pass (on Travis-CI)
* No added warnings to the code (according to Travis-CI)
* New features must have unit tests

I don't want to put performance (compile time/code size/runtime) explicitly in the list, since
one might sometimes need to fix a bug, which might have those effects.
But as a general rule, take care to keep the performance high in this library.

# Code of conduct

[Contributor covenant](./CODE_OF_CONDUCT.md)

