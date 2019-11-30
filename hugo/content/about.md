---
title: "About"
date: "2019-11-10"
---




You can read in more detail about this library came to be in the blog post: [Replacing googletest with jctest](http://www.sizeofvoid.com/2019/04/13/replacing-googletest-with-jctest/)



Hi there, you just reached my blog.


```c
int a = 0;

void foo(int* b) {
    *b = a;
}

typedef struct jc_test_entry {
    jc_test_entry*            next;       // linked list
    const char*               name;
    jc_test_base_class*       instance;
    jc_test_factory_base_interface* factory;    // Factory for parameterized tests
    uint32_t                  fail:1;
    uint32_t                  skipped:1;
    uint32_t                  :30;
    #if defined(__x86_64__) || defined(__ppc64__) || defined(_WIN64)
    uint32_t                  :32;
    #endif
} jc_test_entry;
```