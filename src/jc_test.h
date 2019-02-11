/* test.h    Copyright 2018 Mathias Westerdahl
 *
 * https://github.com/JCash/containers
 *
 * BRIEF:
 *
 *      A small, single header C/C++ test framework
 *      Made sure to compile with hardest warning/error levels possible
 *
 * HISTORY:
 *
 *      0.3     2019-01-19  Added GTEST-like C++ interface
 *      0.2     2016-12-29  Added stdbool.h. Some C99 compile warnings
 *      0.1                 Initial version
 *
 * LICENSE:
 *
 *     The MIT License (MIT)
 *
 *     Copyright (c) 2018 Mathias Westerdahl
 *
 *     Permission is hereby granted, free of charge, to any person obtaining a copy
 *     of this software and associated documentation files (the "Software"), to deal
 *     in the Software without restriction, including without limitation the rights
 *     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *     copies of the Software, and to permit persons to whom the Software is
 *     furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included in all
 *     copies or substantial portions of the Software.
 *
 *     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *     SOFTWARE.
 *
 * DISCLAIMER:
 *
 *     This software is supplied "AS IS" without any warranties and support
 *
 * USAGE:
 *
 *
 */

// WIP!
// This new GTEST-like api needs a lot of testing
// and also making sure the C-API is still intact.
//
// TODOS:
//  * Print erroneous values, not just the expression
//  * SCOPED_TRACE
//  * Log start and end of each test (to easier see what output comes from each test)
//  * Create log events, and a log_event function that can be replaced by user
//  * Handle input parameters (e.g. --jctest_filter=FooBar*)
//  * Support ASSERT_DEATH_IF_SUPPORTED
//  * Support ASSERT_NO_FATAL_FAILURE
//  * Check for TTY
//  * Add UNUSED macro


#ifndef JC_TEST_H
#define JC_TEST_H

#ifndef JC_TEST_ATEXIT
    #include <stdlib.h>
    #define JC_TEST_ATEXIT atexit
#endif

#ifndef JC_TEST_PRINTF
    #include <stdio.h>
    #define JC_TEST_PRINTF printf
#endif

#ifndef JC_TEST_ASSERT_FN
    #include <assert.h>
    #define JC_TEST_ASSERT_FN assert
#endif

#ifndef JC_TEST_FMT
    #define JC_TEST_FMT
#endif

#define JC_TEST_PASS    0
#define JC_TEST_FAIL    1

#ifndef JC_TEST_MAX_NUM_TESTS_PER_FIXTURE
    #define JC_TEST_MAX_NUM_TESTS_PER_FIXTURE 128
#endif

#ifndef JC_TEST_MAX_NUM_FIXTURES
    #define JC_TEST_MAX_NUM_FIXTURES 256
#endif

#ifndef JC_TEST_TIMING_FUNC
    #if defined(_MSC_VER)
        #include <Windows.h>
    #else
        #include <sys/time.h>
    #endif
    #define JC_TEST_TIMING_FUNC jc_test_get_time // returns micro seconds

    typedef unsigned long jc_test_time_t;
    extern jc_test_time_t jc_test_get_time(void);
#endif


typedef void (*jc_test_func)(void* ctx);

// Returns the user defined context for the fixture
typedef void* (*jc_test_fixture_setup_func)();

#if defined(__cplusplus)
    class jc_test_base_class;
    typedef void (*jc_test_void_staticfunc)();
    typedef void (jc_test_base_class::*jc_test_void_memberfunc)();
#else
    typedef int jc_test_base_class;
    typedef void (*jc_test_void_staticfunc)();
    typedef void (*jc_test_void_memberfunc)();
#endif

typedef struct jc_test_entry
{
    const char*         name;
    jc_test_base_class* instance;
    union {
        jc_test_func            test;
        jc_test_void_memberfunc member_test;
    };
} jc_test_entry;

typedef struct jc_test_stats
{
    int num_pass;
    int num_fail;
    int num_assertions;
    int num_tests;
    jc_test_time_t totaltime;
} jc_test_stats;

struct jc_test_funcs_c {
    jc_test_fixture_setup_func  fixture_setup;
    jc_test_func                fixture_teardown;
    jc_test_func                test_setup;
    jc_test_func                test_teardown;
};

struct jc_test_funcs_cpp {
    jc_test_void_staticfunc     fixture_setup;
    jc_test_void_staticfunc     fixture_teardown;
};

typedef struct jc_test_fixture
{
    const char*             name;
    void*                   ctx;        // The test case instance
    struct jc_test_fixture* parent;     // In case of parameterized tests, this points to the first test
    union {
        jc_test_funcs_c     c;
        jc_test_funcs_cpp   cpp;
    };
    jc_test_stats           stats;
    unsigned int            fail:28;
    unsigned int            type:2;     // 0: function, 1: class, 2: params class, 3: params instance
    unsigned int            first:1;    // If it's the first in a range of fixtures
    unsigned int            last:1;     // If it's the last in a range of fixtures
    unsigned int            index;      // the index of the param in the original params array
    unsigned int            num_tests;
    unsigned int            _pad;
    jc_test_entry   tests[JC_TEST_MAX_NUM_TESTS_PER_FIXTURE];

    #if defined(__cplusplus)
    virtual ~jc_test_fixture();
    virtual void SetParam();
    #endif
} jc_test_fixture;

typedef struct jc_test_state
{
    jc_test_stats       stats;
    int num_fixtures;
    int _padding[3];
    jc_test_fixture*    current_fixture;
    jc_test_fixture*    fixtures[JC_TEST_MAX_NUM_FIXTURES];
} jc_test_state;


// May modify the argument list, to remove the test specific arguments
void jc_test_init(int* argc, char** argv);

#ifdef JC_TEST_IMPLEMENTATION
static jc_test_state jc_test_global_state = { {0, 0, 0, 0, 0}, 0, {0, 0, 0}, 0, {0} };
#else
extern jc_test_state jc_test_global_state;
#endif

#if defined(_MSC_VER)
  #pragma section(".CRT$XCU",read)
  #define JC_TEST_INITIALIZER(_NAME_) \
    static void __cdecl jc_test_global_init_##_NAME_(void); \
    __declspec(allocate(".CRT$XCU")) void (__cdecl* jc_test_global_init_##_NAME_##_)(void) = jc_test_global_init_##_NAME_; \
    static void __cdecl jc_test_global_init_##_NAME_(void)
#else
  #define JC_TEST_INITIALIZER(_NAME_) \
    static void jc_test_global_init_##_NAME_(void) __attribute__((constructor)); \
    static void jc_test_global_init_##_NAME_(void)
#endif

// C TEST API Begin -->

#ifdef __cplusplus // casting operators to silence warnings
#define JC_TEST_CAST(_TYPE_, _EXPR_)            reinterpret_cast< _TYPE_ >( _EXPR_ )
#define JC_TEST_STATIC_CAST(_TYPE_, _EXPR_)     static_cast< _TYPE_ >( _EXPR_ )
#else
#define JC_TEST_CAST(_TYPE_, _EXPR_)            ((_TYPE_)(_EXPR_))
#define JC_TEST_STATIC_CAST(_TYPE_, _EXPR_)     ((_TYPE_)(_EXPR_))
#endif

#ifdef __x86_64__
#define JC_TEST_BEGIN(_NAME_, _FIXTURESETUP_, _FIXTURETEARDOWN_, _TESTSETUP_, _TESTTEARDOWN_)  \
    static jc_test_fixture __jc_test_fixture_##_NAME_ = { #_NAME_, 0, \
            JC_TEST_CAST(jc_test_fixture_setup_func, (_FIXTURESETUP_)), JC_TEST_CAST(jc_test_func, (_FIXTURETEARDOWN_)), \
            JC_TEST_CAST(jc_test_func, (_TESTSETUP_)), JC_TEST_CAST(jc_test_func, (_TESTTEARDOWN_)), \
            {0, 0, 0, 0, 0}, 0, 0, {
#else

#define JC_TEST_BEGIN(_NAME_, _FIXTURESETUP_, _FIXTURETEARDOWN_, _TESTSETUP_, _TESTTEARDOWN_)  \
    static jc_test_fixture __jc_test_fixture_##_NAME_ = { #_NAME_, 0, \
            JC_TEST_CAST(jc_test_fixture_setup_func, (_FIXTURESETUP_)), JC_TEST_CAST(jc_test_func, (_FIXTURETEARDOWN_)), \
            JC_TEST_CAST(jc_test_func, (_TESTSETUP_)), JC_TEST_CAST(jc_test_func, (_TESTTEARDOWN_)), \
            {0, 0, 0, 0, 0}, 0, {
#endif

#define JC_TEST_END(_NAME_)        {0, 0} } }; \
                                JC_TEST_INITIALIZER(_NAME_) \
                                { \
                                    jc_test_global_state.fixtures[jc_test_global_state.num_fixtures] = &__jc_test_fixture_##_NAME_; \
                                    ++jc_test_global_state.num_fixtures; \
                                }

#define JC_TEST_FN(_TEST_)         { #_TEST_, JC_TEST_CAST(jc_test_func, (_TEST_)) },

#define JC_TEST_RUN(_NAME_)         jc_test_run_test_fixture( & __jc_test_fixture_##_NAME_ )
#define JC_TEST_RUN_ALL()           jc_test_run_all_tests(&jc_test_global_state)

#define JC_TEST_FILE_AND_LINE       const int jc_test_line = __LINE__; const char* jc_test_filename = __FILE__;

extern void jc_test_set_test_fail();
extern void jc_test_increment_assertions();
extern int jc_test_streq(const char* a, const char* b);

#define JC_TEST_FAILm(_MSG)                                                             \
    {                                                                                   \
        JC_TEST_FILE_AND_LINE;                                                          \
        JC_TEST_PRINTF("\n\t%s( %d ): %s\n", jc_test_filename, jc_test_line, (_MSG));   \
        jc_test_set_test_fail();                                                        \
    }

#define JC_TEST_ASSERT_TRUEm(_VALUE, _MSG)                                 \
    do { jc_test_increment_assertions();                                   \
        if( !(_VALUE) ) { JC_TEST_FAILm(_MSG) }                            \
    } while(0)

#define JC_TEST_ASSERT_FALSEm( _VALUE, _MSG) \
    do { jc_test_increment_assertions();                                      \
        if( (_VALUE) ) { JC_TEST_FAILm(_MSG) }                             \
    } while(0)

#define JC_TEST_ASSERT_EQm( _EXPECTED, _VALUE, _MSG) \
    do { jc_test_increment_assertions();                                    \
        if( !((_EXPECTED) == (_VALUE)) ) { JC_TEST_FAILm(_MSG) }            \
    } while(0)

#define JC_TEST_ASSERT_NEm( _EXPECTED, _VALUE, _MSG) \
    do { jc_test_increment_assertions();                                    \
        if( ((_EXPECTED) == (_VALUE)) ) { JC_TEST_FAILm(_MSG) }             \
    } while(0)

#define JC_TEST_ASSERT_GTm( _EXPECTED, _VALUE, _MSG )                       \
    do { jc_test_increment_assertions();                                    \
        if ((_EXPECTED) <= (_VALUE)) { JC_TEST_FAILm(_MSG) }                \
    } while(0)

#define JC_TEST_ASSERT_LTm( _EXPECTED, _VALUE, _MSG )                       \
    do { jc_test_increment_assertions();                                    \
        if ((_EXPECTED) >= (_VALUE)) { JC_TEST_FAILm(_MSG) }                \
    } while(0)

#define JC_TEST_ASSERT_GEm( _EXPECTED, _VALUE, _MSG )                       \
    do { jc_test_increment_assertions();                                    \
        if ((_EXPECTED) < (_VALUE)) { JC_TEST_FAILm(_MSG) }                 \
    } while(0)

#define JC_TEST_ASSERT_LEm( _EXPECTED, _VALUE, _MSG )                       \
    do { jc_test_increment_assertions();                                    \
        if ((_EXPECTED) > (_VALUE)) { JC_TEST_FAILm(_MSG) }                 \
    } while(0)

#define JC_TEST_ABS(A) ((A) < 0 ? -(A) : (A))

#define JC_TEST_ASSERT_NEARm( _EXPECTED, _VALUE, _EPSILON, _MSG)                            \
    do { jc_test_increment_assertions();                                                    \
        double _expected = (_EXPECTED);                                                     \
        double _value = (_VALUE);                                                           \
        double _epsilon = (_EPSILON);                                                       \
        if( JC_TEST_ABS(_expected - _value) > _epsilon ) { JC_TEST_FAILm(_MSG) }            \
     } while(0)

#define JC_TEST_ASSERT_STREQm( _EXPECTED, _VALUE, _MSG )                        \
    do { jc_test_increment_assertions();                                        \
        if ( !jc_test_streq(_EXPECTED, _VALUE) ) { JC_TEST_FAILm(_MSG) }        \
    } while(0)

#define JC_TEST_ASSERT_STRNEm( _EXPECTED, _VALUE, _MSG )                        \
    do { jc_test_increment_assertions();                                        \
        if ( jc_test_streq(_EXPECTED, _VALUE) ) { JC_TEST_FAILm(_MSG) }         \
    } while(0)

#define JC_TEST_ASSERT_TRUE( _VALUE )          JC_TEST_ASSERT_TRUEm( _VALUE, "" )
#define JC_TEST_ASSERT_FALSE( _VALUE )         JC_TEST_ASSERT_FALSEm( _VALUE, "" )
#define JC_TEST_ASSERT_EQ( _A, _B )            JC_TEST_ASSERT_EQm( _A, _B, #_A " != " #_B )
#define JC_TEST_ASSERT_NE( _A, _B )            JC_TEST_ASSERT_NEm( _A, _B, #_A " == " #_B )
#define JC_TEST_ASSERT_GT( _A, _B )            JC_TEST_ASSERT_GTm( _A, _B, #_A " <= " #_B )
#define JC_TEST_ASSERT_LT( _A, _B )            JC_TEST_ASSERT_LTm( _A, _B, #_A " >= " #_B )
#define JC_TEST_ASSERT_GE( _A, _B )            JC_TEST_ASSERT_GEm( _A, _B, #_A " < " #_B )
#define JC_TEST_ASSERT_LE( _A, _B )            JC_TEST_ASSERT_LEm( _A, _B, #_A " > " #_B )
#define JC_TEST_ASSERT_NEAR( _A, _B, _EPS )    JC_TEST_ASSERT_NEARm( _A, _B, _EPS, "ABS(" #_A " - " #_B ") > " #_EPS )
#define JC_TEST_ASSERT_STREQ( _A, _B )         JC_TEST_ASSERT_STREQm( _A, _B, #_A " != " #_B )
#define JC_TEST_ASSERT_STRNE( _A, _B )         JC_TEST_ASSERT_STRNEm( _A, _B, #_A " == " #_B )

#define JC_TEST_SCOPED_TRACE(_MSG)


// <-- End C TEST API

extern void jc_test_run_test_fixture(jc_test_fixture* fixture);
extern int jc_test_run_all_tests(jc_test_state* state);

#if defined(__cplusplus)

class jc_test_base_class {
public:
    virtual ~jc_test_base_class();
    static void SetUpTestCase() {}      // The UserClass::SetUpTestCase is called before each test case runs
    static void TearDownTestCase() {}   // The UserClass::TearDownTestCase is called after all tests have run
    virtual void SetUp();               // Called before each test
    virtual void TearDown();            // Called after each test
    virtual void TestBody() = 0;        // Implemented by TEST_F and TEST_P
private:
    struct Setup_should_be_spelled_SetUp {};
    virtual Setup_should_be_spelled_SetUp* Setup() { return 0; } // Trick from GTEST to make sure users don't accidentally misspell the function
};

template<typename T>
class jc_test_value_iterator {
public:
    virtual ~jc_test_value_iterator();
    virtual const T* Get() const = 0;
    virtual void Advance() = 0;
    virtual bool Empty() const = 0;   // return false when out of values
    virtual void Rewind() = 0;
};
template<typename T> jc_test_value_iterator<T>::~jc_test_value_iterator() {} // separate line to silence warning

template<typename T>
class jc_test_array_iterator : public jc_test_value_iterator<T> {
public:
    const T *begin, *cursor, *end;
    jc_test_array_iterator(const T* _begin, const T* _end) : begin(_begin), cursor(_begin), end(_end) {}
    const T* Get() const{ return cursor; }
    void Advance()      { ++cursor; }
    bool Empty() const  { return cursor == end; }
    void Rewind()       { cursor = begin; }
};

template<typename T> jc_test_array_iterator<T>* jc_test_values_in(const T* begin, const T* end) {
    return new jc_test_array_iterator<T>(begin, end);
}
template<typename T, size_t N> jc_test_array_iterator<T>* jc_test_values_in(const T (&arr)[N] ) {
    return jc_test_values_in(arr, arr+N);
}

template<typename ParamType>
class jc_test_params_class : public jc_test_base_class {
public:
    typedef ParamType param_t;
    jc_test_params_class() {}
    static const ParamType&    GetParam()                           { return *param; }
    static void                SetParam(const ParamType* _param)    { param = _param; }
    static const ParamType* param;
};
template<typename ParamType> const ParamType* jc_test_params_class<ParamType>::param = 0;

template<typename ParamType>
struct jc_test_fixture_with_param : public jc_test_fixture
{
    void SetParam() { JC_TEST_CAST(jc_test_params_class<ParamType>*, ctx)->SetParam(param); }
    const ParamType* param;
};

template<typename ParamType>
struct jc_test_factory_interface {
    virtual ~jc_test_factory_interface() {}
    virtual jc_test_params_class<ParamType>* New() = 0;
    virtual void SetParam(const ParamType* param) = 0;
};

template<typename T>
struct jc_test_factory : public jc_test_factory_interface<typename T::param_t> {
    jc_test_params_class<typename T::param_t>* New()    { return new T(); }
    void SetParam(const typename T::param_t* param)     { T::SetParam(param); }
};

// struct jc_test_typed_factory_interface {
//     virtual ~jc_test_typed_factory_interface() {}
//     template <typename TypeParam>
//     virtual jc_test_base_class* New<typename TypeParam>() = 0;
// };

// template<template <typename> class T>
// struct jc_test_typed_factory : public jc_test_typed_factory_interface {
//     template <typename TypeParam> jc_test_base_class* New()   { return new T<TypeParam>; }
// };

#define JC_TEST_FIXTURE_TYPE_FUNCTION           0
#define JC_TEST_FIXTURE_TYPE_CLASS              1
#define JC_TEST_FIXTURE_TYPE_PARAMS_CLASS       2
#define JC_TEST_FIXTURE_TYPE_TYPED_CLASS        3

extern jc_test_fixture* jc_test_find_fixture(const char* name, unsigned int fixture_type);
extern jc_test_fixture* jc_test_alloc_fixture(const char* name, unsigned int fixture_type, void* ctx);
extern jc_test_fixture* jc_test_create_fixture(jc_test_fixture* fixture, const char* name, unsigned int fixture_type, void* ctx);
extern jc_test_entry*   jc_test_add_test_to_fixture(jc_test_fixture* fixture, const char* test_name, jc_test_func test_fn, jc_test_base_class* instance);
extern void             jc_test_memcpy(void* dst, void* src, size_t size);

extern int jc_test_register_test_fn(const char* fixture_name, const char* test_name, jc_test_func test_fn);
extern int jc_test_register_test(const char* fixture_name, const char* test_name, jc_test_func test_fn);
extern int jc_test_register_class_test(const char* fixture_name, const char* test_name,
                                                jc_test_void_staticfunc class_setup, jc_test_void_staticfunc class_teardown,
                                                jc_test_base_class* instance, unsigned int fixture_type);
template <typename ParamType>
int jc_test_register_param_class_test(const char* fixture_name, const char* test_name,
                        jc_test_void_staticfunc class_setup, jc_test_void_staticfunc class_teardown,
                        jc_test_factory_interface<ParamType>* factory) {
    jc_test_fixture* fixture = jc_test_find_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_PARAMS_CLASS);
    if (!fixture) {
        fixture = jc_test_alloc_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_PARAMS_CLASS, factory);
        fixture->cpp.fixture_setup = class_setup;
        fixture->cpp.fixture_teardown = class_teardown;
    }
    jc_test_add_test_to_fixture(fixture, test_name, 0, 0);
    return 0;
}

struct jc_test_type0 {};

template<typename T1>
struct jc_test_type1 {
    typedef T1 head;
    typedef jc_test_type0       tail;
};
template<typename T1, typename T2>
struct jc_test_type2 {
    typedef T2 head;
    typedef jc_test_type1<T1>   tail;
};
template<typename T1, typename T2, typename T3>
struct jc_test_type3 {
    typedef T3 head;
    typedef jc_test_type2<T1, T2>   tail;
};

template <typename BaseClassSelector, typename TypeList>
struct jc_test_register_typed_class_test {
    static int register_test(const char* fixture_name, const char* test_name, unsigned int index) {
        typedef typename TypeList::head TypeParam;
        typedef typename BaseClassSelector::template bind<TypeParam>::type TestClass;

        jc_test_fixture* fixture = jc_test_find_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_TYPED_CLASS);
        if (!fixture) {
            fixture = jc_test_alloc_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_CLASS, 0);
            fixture->cpp.fixture_setup = TestClass::SetUpTestCase;
            fixture->cpp.fixture_teardown = TestClass::TearDownTestCase;
        }
        fixture->index = index;
        jc_test_add_test_to_fixture(fixture, test_name, 0, new TestClass);
        return jc_test_register_typed_class_test<BaseClassSelector, typename TypeList::tail>::
                    register_test(fixture_name, test_name, index+1);
    }
};

template <typename BaseClassSelector>
struct jc_test_register_typed_class_test<BaseClassSelector,jc_test_type0> {
    static int register_test(const char*, const char*, unsigned int) {
        return 0;
    }
};

template<typename ParamType>
jc_test_fixture* jc_test_alloc_fixture_with_param(const char* name, unsigned int type, void* ctx) {
    return jc_test_create_fixture(new jc_test_fixture_with_param<ParamType>, name, type, ctx);
}

template<typename ParamType>
int jc_test_register_param_tests(const char* prototype_fixture_name, const char* fixture_name, jc_test_value_iterator<ParamType>* values)
{
    unsigned int index = 0;
    jc_test_fixture* first_fixture = 0;
    while (!values->Empty()) {
        jc_test_fixture* prototype_fixture = jc_test_find_fixture(prototype_fixture_name, JC_TEST_FIXTURE_TYPE_PARAMS_CLASS);
        JC_TEST_ASSERT_FN(prototype_fixture->type == JC_TEST_FIXTURE_TYPE_PARAMS_CLASS);

        // Allocate a new fixture, and create the test class
        jc_test_fixture_with_param<ParamType>* fixture = JC_TEST_CAST(jc_test_fixture_with_param<ParamType>*,
                                jc_test_alloc_fixture_with_param<ParamType>(fixture_name, JC_TEST_FIXTURE_TYPE_CLASS, 0) );

        fixture->first = first_fixture == 0 ? 1 : 0;
        if (!first_fixture) {
            first_fixture = fixture; // A silly trick to make the first fixture accumulate all the timings from this batch
        }
        fixture->parent = first_fixture;
        fixture->cpp = prototype_fixture->cpp;
        fixture->index = index++;

        jc_test_factory_interface<ParamType>* factory = JC_TEST_CAST(jc_test_factory_interface<ParamType>*, prototype_fixture->ctx);

        fixture->param = values->Get();
        factory->SetParam(fixture->param);
        fixture->ctx = JC_TEST_CAST(void*, factory->New());

        fixture->num_tests = prototype_fixture->num_tests;
        jc_test_memcpy(fixture->tests, prototype_fixture->tests, sizeof(fixture->tests));

        values->Advance();

        fixture->last = values->Empty() ? 1 : 0;
    }

    delete values;
    return 0;
}

#define JC_TEST_MAKE_NAME2(X,Y)                 X ## _ ## Y
#define JC_TEST_MAKE_NAME3(X,Y,Z)               X ## _ ## Y ## _ ## Z
#define JC_TEST_MAKE_CLASS_NAME(X, Y)           JC_TEST_MAKE_NAME3(X, Y, _TestCase)
#define JC_TEST_MAKE_FUNCTION_NAME(X, Y)        JC_TEST_MAKE_NAME2(X, Y)
#define JC_TEST_MAKE_UNIQUE_NAME(X, Y, LINE)    JC_TEST_MAKE_NAME3(X, Y, LINE)

// TEST(MyNameSpace, TestName)( ASSERT_EQ(4, 2*2); )
#define JC_TEST(testfixture,testfn)                                                                         \
    extern void JC_TEST_MAKE_FUNCTION_NAME(testfixture,testfn)(void* _jc_test_ctx);                         \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) =                                      \
        jc_test_register_test_fn(#testfixture, #testfn, JC_TEST_MAKE_FUNCTION_NAME(testfixture,testfn));    \
    void JC_TEST_MAKE_FUNCTION_NAME(testfixture,testfn)(void* _jc_test_ctx)


// class MyTest : public jc_test_base_class {
// public:
//     static void SetUpTestCase()      {...};
//     static void TearDownTestCase()   {...};
//     void SetUp()                     {...};
//     void TearDown()                  {...};
// };
// TEST_F(MyTest, TestName)( ASSERT_EQ(4, 2*2); )
#define JC_TEST_F(testfixture,testfn)                                                                       \
    class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture {                                \
        virtual void TestBody();                                                                            \
    };                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) = jc_test_register_class_test(         \
            #testfixture, #testfn, testfixture::SetUpTestCase, testfixture::TearDownTestCase,               \
            new JC_TEST_MAKE_CLASS_NAME(testfixture,testfn), JC_TEST_FIXTURE_TYPE_CLASS);                   \
    void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()


// class MyParamTest : public jc_test_params_class<ParamType> {
// public:
//     static void SetUpTestCase()      {...};
//     static void TearDownTestCase()   {...};
//     void SetUp()                     {...};
//     void TearDown()                  {...};
// };
// TEST_P(MyParamTest, TestName)( ParamType value = GetParam(); ASSERT_EQ(0, value&1; )
#define JC_TEST_P(testfixture,testfn)                                                                       \
    class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture {                                \
        virtual void TestBody();                                                                            \
    };                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) = jc_test_register_param_class_test(   \
            #testfixture, #testfn, testfixture::SetUpTestCase, testfixture::TearDownTestCase,               \
            new jc_test_factory<JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)>());                            \
    void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()

// Creates a fixture placeholder, that creates instances of the appropriate TEST_P classes, and gives them the params
// INSTANTIATE_TEST_CASE_P(EvenValues, MyParamTest, jc_test_values(2,4,6,8,10));
#define JC_TEST_INSTANTIATE_TEST_CASE_P(prefix,testfixture,testvalues)                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(prefix,testfixture,__LINE__) =                                      \
        jc_test_register_param_tests<testfixture::param_t>(#testfixture, #prefix "/" #testfixture, testvalues)


template<typename T> struct jc_test_typed_list {
    typedef T type;
};

template<template <typename T> class BaseClass> struct jc_test_template_sel {
    template <typename TypeParam> struct bind {
        typedef BaseClass<TypeParam> type;
    };
};

#define JC_TEST_TYPED_TEST_SUITE(testfixture,testtypes)                                                     \
    typedef jc_test_typed_list<testtypes>::type JC_TEST_MAKE_NAME2(testfixture,Types)

#define JC_TEST_TYPED_TEST(testfixture,testfn)                                                              \
    template<typename T> class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture<T> {        \
        virtual void TestBody();                                                                            \
        typedef testfixture<T> TestFixture;                                                                 \
        typedef T TypeParam;                                                                                \
    };                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) =                                      \
            jc_test_register_typed_class_test<                                                              \
                jc_test_template_sel<JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)>,                          \
                JC_TEST_MAKE_NAME2(testfixture,Types)>::register_test(#testfixture, #testfn, 0);            \
    template<typename T> void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)<T>::TestBody()

#endif // __cplusplus


#ifndef JC_UNDEF_SHORT_NAMES
    #define RUN                             JC_TEST_RUN
    #define RUN_ALL                         JC_TEST_RUN_ALL
    #define TEST                            JC_TEST
    #define TEST_F                          JC_TEST_F
    #define TEST_P                          JC_TEST_P
    #define INSTANTIATE_TEST_CASE_P         JC_TEST_INSTANTIATE_TEST_CASE_P
    #define TYPED_TEST                      JC_TEST_TYPED_TEST
    #define TYPED_TEST_SUITE                JC_TEST_TYPED_TEST_SUITE
    #define ASSERT_TRUE                     JC_TEST_ASSERT_TRUE
    #define ASSERT_FALSE                    JC_TEST_ASSERT_FALSE
    #define ASSERT_EQ                       JC_TEST_ASSERT_EQ
    #define ASSERT_NE                       JC_TEST_ASSERT_NE
    #define ASSERT_GT                       JC_TEST_ASSERT_GT
    #define ASSERT_LT                       JC_TEST_ASSERT_LT
    #define ASSERT_GE                       JC_TEST_ASSERT_GE
    #define ASSERT_LE                       JC_TEST_ASSERT_LE
    #define ASSERT_NEAR                     JC_TEST_ASSERT_NEAR
    #define ASSERT_STREQ                    JC_TEST_ASSERT_STREQ
    #define ASSERT_STRNE                    JC_TEST_ASSERT_STRNE
    #define EXPECT_FALSE                    JC_TEST_ASSERT_FALSE
    #define EXPECT_EQ                       JC_TEST_ASSERT_EQ
    #define SCOPED_TRACE                    JC_TEST_SCOPED_TRACE
#endif

#endif // JC_TEST_H

#ifdef JC_TEST_IMPLEMENTATION
#undef JC_TEST_IMPLEMENTATION

#ifdef JC_TEST_NO_COLORS
    #define JC_TEST_CLR_DEFAULT ""
    #define JC_TEST_CLR_RED     ""
    #define JC_TEST_CLR_GREEN   ""
    #define JC_TEST_CLR_YELLOW  ""
    #define JC_TEST_CLR_BLUE    ""
    #define JC_TEST_CLR_MAGENTA ""
    #define JC_TEST_CLR_CYAN    ""
    #define JC_TEST_CLR_WHITE   ""
#else
    #define JC_TEST_CLR_DEFAULT "\x1B[0m"
    #define JC_TEST_CLR_RED     "\x1B[31m"
    #define JC_TEST_CLR_GREEN   "\x1B[32m"
    #define JC_TEST_CLR_YELLOW  "\x1B[33m"
    #define JC_TEST_CLR_BLUE    "\x1B[34m"
    #define JC_TEST_CLR_MAGENTA "\x1B[35m"
    #define JC_TEST_CLR_CYAN    "\x1B[36m"
    #define JC_TEST_CLR_WHITE   "\x1B[37m"
#endif

// wip
// static const int JC_TEST_CLR_DEFAULT = 0;
// static const int JC_TEST_CLR_RED     = 1;
// static const int JC_TEST_CLR_GREEN   = 2;
// static const int JC_TEST_CLR_YELLOW  = 3;
// static const int JC_TEST_CLR_BLUE    = 4;
// static const int JC_TEST_CLR_MAGENTA = 5;
// static const int JC_TEST_CLR_CYAN    = 6;
// static const int JC_TEST_CLR_WHITE   = 7;
// static char* jc_test_colors[] = {"\x1B[0m","\x1B[31m","\x1B[32m","\x1B[33m","\x1B[34m","\x1B[35m","\x1B[36m","\x1B[37m"};

#if defined(__cplusplus)

static void jc_test_global_cleanup();

static inline void jc_test_memset(void* _mem, unsigned int pattern, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        JC_TEST_CAST(unsigned char*, _mem)[i] = JC_TEST_STATIC_CAST(unsigned char, pattern & 0xFF);
    }
}

void jc_test_memcpy(void* dst, void* src, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        JC_TEST_CAST(unsigned char*, dst)[i] = JC_TEST_CAST(unsigned char*, src)[i];
    }
}

int jc_test_streq(const char* a, const char* b)
{
    if (a == 0) return (b == 0) ? 1 : 0;
    if (b == 0) return 0;
    while (*a && (*a == *b)) {
        ++a; ++b;
    }
    return (*a - *b) == 0 ? 1 : 0;
}

jc_test_fixture::~jc_test_fixture() {}
void jc_test_fixture::SetParam() {}

jc_test_base_class::~jc_test_base_class() {}
void jc_test_base_class::SetUp() {}
void jc_test_base_class::TearDown() {}

jc_test_fixture* jc_test_create_fixture(jc_test_fixture* fixture, const char* name, unsigned int fixture_type, void* ctx) {
    fixture->name = name;
    fixture->ctx = ctx;
    fixture->type = fixture_type;
    fixture->parent = 0;
    fixture->fail = 0;
    fixture->index = 0xFFFFFFFF;
    fixture->num_tests = 0;
    fixture->first = fixture->last = 1;
    jc_test_memset(&fixture->c, 0, sizeof(fixture->c));
    jc_test_memset(&fixture->stats, 0, sizeof(fixture->stats));
    jc_test_memset(fixture->tests, 0, sizeof(fixture->tests));
    JC_TEST_ASSERT_FN(jc_test_global_state.num_fixtures < JC_TEST_MAX_NUM_FIXTURES);
    return jc_test_global_state.fixtures[jc_test_global_state.num_fixtures++] = fixture;
}

jc_test_entry* jc_test_add_test_to_fixture(jc_test_fixture* fixture, const char* test_name, jc_test_func test_fn, jc_test_base_class* instance) {
    if (fixture->num_tests >= JC_TEST_MAX_NUM_TESTS_PER_FIXTURE) {
        // error
        return 0;
    }
    jc_test_entry* test = &fixture->tests[fixture->num_tests++];
    test->name = test_name;
    test->test = test_fn;
    test->instance = instance;
    return test;
}

jc_test_fixture* jc_test_find_fixture(const char* name, unsigned int fixture_type) {
    for( int i = 0; i < jc_test_global_state.num_fixtures; ++i) {
        if (jc_test_global_state.fixtures[i]->type == fixture_type && jc_test_streq(jc_test_global_state.fixtures[i]->name, name))
            return jc_test_global_state.fixtures[i];
    }
    return 0;
}

jc_test_fixture* jc_test_alloc_fixture(const char* name, unsigned int fixture_type, void* ctx) {
    return jc_test_create_fixture(new jc_test_fixture, name, fixture_type, ctx);
}

static jc_test_fixture* jc_test_find_or_alloc_fixture(const char* fixture_name, unsigned int fixture_type, void* ctx) {
    jc_test_fixture* fixture = jc_test_find_fixture(fixture_name, fixture_type);
    if (!fixture) {
        fixture = jc_test_alloc_fixture(fixture_name, fixture_type, ctx);
    }
    return fixture;
}

int jc_test_register_test_fn(const char* fixture_name, const char* test_name, jc_test_func test_fn) {
    jc_test_fixture* fixture = jc_test_find_or_alloc_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_FUNCTION, 0);
    jc_test_add_test_to_fixture(fixture, test_name, test_fn, 0);
    return 0;
}

int jc_test_register_class_test(const char* fixture_name, const char* test_name,
                        jc_test_void_staticfunc class_setup, jc_test_void_staticfunc class_teardown,
                        jc_test_base_class* instance, unsigned int fixture_type) {
    jc_test_fixture* fixture = jc_test_find_fixture(fixture_name, fixture_type);
    if (!fixture) {
        fixture = jc_test_alloc_fixture(fixture_name, fixture_type, 0);
        fixture->cpp.fixture_setup = class_setup;
        fixture->cpp.fixture_teardown = class_teardown;
    }
    jc_test_add_test_to_fixture(fixture, test_name, 0, instance);
    return 0;
}

static void jc_test_global_cleanup() {
    for (int i = 0; i < jc_test_global_state.num_fixtures; ++i) {
        for (int j = 0; j < JC_TEST_MAX_NUM_TESTS_PER_FIXTURE; ++j) {
            if (!jc_test_global_state.fixtures[i]->tests[j].name) {
                break;
            }
            if (jc_test_global_state.fixtures[i]->tests[j].instance)
                delete jc_test_global_state.fixtures[i]->tests[j].instance;
        }
        delete jc_test_global_state.fixtures[i];
    }
}

#endif // __cplusplus

void jc_test_set_test_fail()
{
    jc_test_global_state.current_fixture->fail = JC_TEST_FAIL;
}

void jc_test_increment_assertions()
{
    ++jc_test_global_state.current_fixture->stats.num_assertions;
}

static void jc_test_report_time(jc_test_time_t t) // Micro seconds
{
#ifdef _MSC_VER
    #define JC_TEST_MICROSECONDS_STR "us"
#else
    #define JC_TEST_MICROSECONDS_STR "\u00b5s"
#endif

    if( t < 5000 )
        JC_TEST_PRINTF("%g %s", JC_TEST_STATIC_CAST(double, t), JC_TEST_MICROSECONDS_STR);
    else if( t < 500000 )
        JC_TEST_PRINTF("%g %s", t / 1000.0, "ms");
    else
        JC_TEST_PRINTF("%g %s", t / 1000000.0, "s");
}

#if defined(__cplusplus)
    #define JC_TEST_INVOKE_MEMBER_FN(INSTANCE, FN) \
            { \
                jc_test_base_class* inst = JC_TEST_CAST(jc_test_base_class*, (INSTANCE) ); \
                jc_test_void_memberfunc fn = JC_TEST_CAST(jc_test_void_memberfunc, (FN) ); \
                (inst->*fn)(); \
            }
            //(((jc_test_base_class*)(INSTANCE)) -> * (_FN) ) ();
#endif

static inline int jc_test_cmp_fpos_t(const fpos_t* a, const fpos_t* b) {
    #if defined(__linux__)
        return a->__pos == b->__pos;
    #else
        return *a == *b;
    #endif
}

void jc_test_run_test_fixture(jc_test_fixture* fixture)
{
    jc_test_global_state.current_fixture = fixture;
    if (fixture->type == JC_TEST_FIXTURE_TYPE_PARAMS_CLASS) {
        return;
    }

    jc_test_memset(&fixture->stats, 0, sizeof(fixture->stats));

    fixture->stats.totaltime = 0;
    jc_test_time_t timestart = JC_TEST_TIMING_FUNC();
    if (fixture->first) {
        JC_TEST_PRINTF("%s%s%s\n", JC_TEST_CLR_CYAN, fixture->name, JC_TEST_CLR_DEFAULT);
    }
    if(fixture->type == JC_TEST_FIXTURE_TYPE_FUNCTION) {
        if (fixture->first && fixture->c.fixture_setup != 0) {
            fixture->ctx = fixture->c.fixture_setup();
        }
    }
    #if defined(__cplusplus)
    else {
        if (fixture->first && fixture->cpp.fixture_setup != 0) {
            fixture->cpp.fixture_setup();
        }
        fixture->SetParam();
    }
    #endif


    for (unsigned int count = 0; count < fixture->num_tests; ++count)
    {
        jc_test_entry* test = &fixture->tests[count];
        fixture->fail = JC_TEST_PASS;

        if (fixture->index == 0xFFFFFFFF) {
            JC_TEST_PRINTF("%s%s%s: ", JC_TEST_CLR_YELLOW, test->name, JC_TEST_CLR_DEFAULT);
        } else {
            JC_TEST_PRINTF("%s%s%s/%d: ", JC_TEST_CLR_YELLOW, test->name, JC_TEST_CLR_DEFAULT, fixture->index);
        }
        fpos_t stdout_pos_start;
        fgetpos(stdout, &stdout_pos_start);

        jc_test_time_t teststart = 0;
        jc_test_time_t testend = 0;

        jc_test_func fns[3] = { fixture->c.test_setup, test->test, fixture->c.test_teardown };
        jc_test_void_memberfunc cppfns[3] = { &jc_test_base_class::SetUp, &jc_test_base_class::TestBody, &jc_test_base_class::TearDown };

        #if defined(__cplusplus)
        jc_test_base_class* instance = JC_TEST_CAST(jc_test_base_class*, fixture->ctx);
        if (test->instance) {
            instance = test->instance;
        }
        #endif

        for( int i = 0; i < 3; ++i ) {
            if( (fixture->type == JC_TEST_FIXTURE_TYPE_FUNCTION && !fns[i])) {
                continue;
            }

            if( i == 1 ) {
                teststart = JC_TEST_TIMING_FUNC();
            }

            if (fixture->type == JC_TEST_FIXTURE_TYPE_FUNCTION) {
                fns[i](fixture->ctx);
            }
#if defined(__cplusplus)
            else {
                JC_TEST_INVOKE_MEMBER_FN(instance, cppfns[i]);
            }
#endif

            if( i == 1 ) {
                testend = JC_TEST_TIMING_FUNC();
            }

            if( fixture->fail != JC_TEST_PASS ) {
                break;
            }
        }

        fpos_t stdout_pos_end;
        fgetpos(stdout, &stdout_pos_end);

        const char* return_char = jc_test_cmp_fpos_t(&stdout_pos_end, &stdout_pos_start) ? "\r" : "\n";

        if (fixture->index == 0xFFFFFFFF) {
            JC_TEST_PRINTF("%s%s%s%s %s (", return_char, JC_TEST_CLR_YELLOW, test->name, JC_TEST_CLR_DEFAULT, fixture->fail == JC_TEST_PASS ? (JC_TEST_CLR_GREEN "PASS" JC_TEST_CLR_DEFAULT) : (JC_TEST_CLR_RED "FAIL" JC_TEST_CLR_DEFAULT) );
        } else {
            JC_TEST_PRINTF("%s%s%s%s/%d %s (", return_char, JC_TEST_CLR_YELLOW, test->name, JC_TEST_CLR_DEFAULT, fixture->index, fixture->fail == JC_TEST_PASS ? (JC_TEST_CLR_GREEN "PASS" JC_TEST_CLR_DEFAULT) : (JC_TEST_CLR_RED "FAIL" JC_TEST_CLR_DEFAULT) );
        }
        jc_test_report_time(testend - teststart);
        JC_TEST_PRINTF(")\n");

        if( fixture->fail == JC_TEST_PASS )
            ++fixture->stats.num_pass;
        else
            ++fixture->stats.num_fail;
        ++fixture->stats.num_tests;
    }

    if(fixture->type == JC_TEST_FIXTURE_TYPE_FUNCTION) {
        if(fixture->last && fixture->c.fixture_teardown != 0) {
            fixture->c.fixture_teardown(fixture->ctx);
        }
    }
    #if defined(__cplusplus)
    else {
        if (fixture->last && fixture->cpp.fixture_teardown != 0) {
            fixture->cpp.fixture_teardown();
        }
    }
    #endif

    jc_test_time_t timeend = JC_TEST_TIMING_FUNC();
    fixture->stats.totaltime = timeend - timestart;
    if (fixture->parent) {
        fixture->parent->stats.totaltime += fixture->stats.totaltime;
    }

    if (fixture->last) {
        jc_test_time_t totaltime = fixture->stats.totaltime;
        if (fixture->parent) {
            totaltime = fixture->parent->stats.totaltime;
        }

        JC_TEST_PRINTF("%s%s%s took ", JC_TEST_CLR_CYAN, fixture->name, JC_TEST_CLR_DEFAULT);
        jc_test_report_time(totaltime);
        JC_TEST_PRINTF("\n");
    }
    jc_test_global_state.current_fixture = 0;
}

int jc_test_run_all_tests(jc_test_state* state)
{
    state->stats.totaltime = 0;
    for( int i = 0; i < state->num_fixtures; ++i )
    {
        if( state->fixtures[i] )
        {
            jc_test_run_test_fixture( state->fixtures[i] );

            state->stats.num_assertions += state->fixtures[i]->stats.num_assertions;
            state->stats.num_pass += state->fixtures[i]->stats.num_pass;
            state->stats.num_fail += state->fixtures[i]->stats.num_fail;
            state->stats.num_tests += state->fixtures[i]->stats.num_tests;
            state->stats.totaltime += state->fixtures[i]->stats.totaltime;
        }
    }

    JC_TEST_PRINTF("Ran %d tests, with %d assertions in ", state->stats.num_tests, state->stats.num_assertions);
    jc_test_report_time(state->stats.totaltime);
    JC_TEST_PRINTF("\n");
    if( state->stats.num_fail)
        JC_TEST_PRINTF("%d tests passed, and %d tests %sFAILED%s\n", state->stats.num_pass, state->stats.num_fail, JC_TEST_CLR_RED, JC_TEST_CLR_DEFAULT);
    else
        JC_TEST_PRINTF("%d tests %sPASSED%s\n", state->stats.num_pass, JC_TEST_CLR_GREEN, JC_TEST_CLR_DEFAULT);

    return state->stats.num_fail;
}

#if defined(_MSC_VER)

jc_test_time_t jc_test_get_time(void)
{
    LARGE_INTEGER tickPerSecond;
    LARGE_INTEGER tick;
    QueryPerformanceFrequency(&tickPerSecond);
    QueryPerformanceCounter(&tick);
    return tick.QuadPart / (tickPerSecond.QuadPart / 1000000);
}

#else

jc_test_time_t jc_test_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return JC_TEST_STATIC_CAST(jc_test_time_t, tv.tv_sec) * 1000000U + JC_TEST_STATIC_CAST(jc_test_time_t, tv.tv_usec);
}

#endif

void jc_test_init(int* argc, char** argv)
{
    (void)argc; (void)argv;
    JC_TEST_ATEXIT(jc_test_global_cleanup);
}

#endif
