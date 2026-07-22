# AGENTS.md

These instructions apply to the entire repository.

## Language and Design

* Prefer C-like C++ ("Orthodox C++"): simple types, explicit ownership, RAII, and clear invariants.
* Target C++11 and avoid newer language features unless there is a clear need.
* Keep dependencies minimal and avoid template-heavy or meta-programming-heavy solutions.
* Do not rely on exceptions (the project builds with `-fno-exceptions`).

## Style

* Follow the existing style in nearby files. Do not reformat unrelated code.
* Use 4 spaces for indentation and keep whitespace changes minimal.
* Keep include order consistent with existing files: system headers first, then local headers.
* Prefer project-established forms such as `#if defined(...)` preprocessor guards and uppercase macro names.
* Use simple, readable control flow; avoid clever constructs.

## Compatibility Guidelines

* Prefer lightweight C-style headers and APIs already used in this repo (`<stdio.h>`, `<string.h>`, etc.) unless there is a reason to use something else.
* Do not add template-heavy C++ features or standard-library machinery without measuring compile time, runtime, and binary size.
* Keep code warning-clean under the current compile flags.

## Validation

* Build with C++11 first when touching C++ code:
  * `STDVERSION=c++11 scripts/compile.sh`
* Run tests after changes:
  * `scripts/run_tests.sh`
