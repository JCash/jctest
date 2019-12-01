---
title: "About"
date: "2019-11-10"
---

## Why

The main reason this library was created, was to minimize code maintenance for an existing code base (where we compiled GTEST for many platforms), and to see if the build pipeline could be optimized.

You can read in more detail about how this library came to be in the blog post:<br>
[Replacing googletest with jctest](http://www.sizeofvoid.com/2019/04/13/replacing-googletest-with-jctest/)

## Goals

This library was created with requirements and guidelines in place:

{{% ticks %}}
* Replacement for googletest, with minimal changes
* Support a few use cases: TEST, TEST_F, TEST_P, TYPED_TEST
* As few templates as possible
* No STL
* As small as possible
* Single header only library
{{% /ticks %}}

