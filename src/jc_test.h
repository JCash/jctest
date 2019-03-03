/* test.h    Copyright 2018 Mathias Westerdahl
 *
 * https://github.com/JCash/jctest
 * https://jcash.github.io/jctest
 *
 * BRIEF:
 *
 *      A single header C/C++ test framework in <900 loc
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
 *     Copyright (c) 2018-2019 Mathias Westerdahl
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
 *      This software is supplied "AS IS" without any warranties and support
 *
 *      This software was designed to be a (non complete) replacement for GTEST,
 *      with the intent to replace the library in an existing code base.
 *      Although the supported features were implemented in the same spirit as the GTEST
 *      fixtures/functions, there will be discprepancies. However, those differences have
 *      been chosen from a pragmatic standpoint, in favor of making porting of the existing
 *      tests feasible with minimal changes, as well as keeping this library
 *      as light weight as possible.
 *
 *
 * USAGE:

// Use case 1:

// Smallest test case

#define JC_TEST_IMPLEMENTATION
#include <jc_test.h>

TEST(FixtureName, TestName) {
    ASSERT_EQ(4, 2*2);
}

int main(int argc, char** argv) {
    jc_test_init(int* argc, char** argv); // may modify the command line
    return JC_TEST_RUN_ALL();
}

// Use case 2:

// Test fixtures are good if you wish to call code before or after the test or fixture itself.

class MyTest : public jc_test_base_class {
public:
 static void SetUpTestCase()      {...};
 static void TearDownTestCase()   {...};
 virtual void SetUp()             {...};
 virtual void TearDown()          {...};
};

TEST_F(MyTest, TestName) {
 ASSERT_EQ(4, 2*2);
}

// Use case 3:

// Parameterized tests are good if you wish to call a test case with different parameters

class MyParamTest : public jc_test_params_class<ParamType> {
public:
    static void SetUpTestCase()      {...};
    static void TearDownTestCase()   {...};
    virtual void SetUp()             {...};
    virtual void TearDown()          {...};
    static const ParamType&          GetParam(); // New param for each test iteration
};

TEST_P(MyParamTest, IsEven) {
    ParamType value = GetParam();
    ASSERT_EQ(0, value & 1);
}

// Creates a new fixture for each test param
INSTANTIATE_TEST_CASE_P(EvenValues, MyParamTest, jc_test_values(2,4,6,8,10));


WIP!
This new GTEST-like api needs a lot of testing
and also making sure the C-API is still intact.
TODOS:
 * SCOPED_TRACE
 * Handle input parameters (e.g. --jctest_filter=FooBar*)
 * Support ASSERT_DEATH_IF_SUPPORTED (or similar)
 * Support ASSERT_NO_FATAL_FAILURE (or similar)

*/

#ifndef JC_TEST_H
#define JC_TEST_H

#ifndef JC_TEST_ATEXIT
    #include <stdlib.h>
    #define JC_TEST_ATEXIT atexit
#endif

#ifndef JC_TEST_LOGF
    #include <stdarg.h>
    #include <stdio.h>
    #define JC_TEST_LOGF jc_test_logf
    #define JC_TEST_PRINTF printf
#endif

#ifndef JC_TEST_ASSERT_FN
    #include <assert.h>
    #define JC_TEST_ASSERT_FN assert
#endif

#ifndef JC_TEST_SNPRINTF
    #include <stdio.h>
    #if defined(_MSC_VER)
        #define JC_TEST_SNPRINTF _snprintf
    #else
        #define JC_TEST_SNPRINTF snprintf
    #endif
#endif

#if defined(JC_TEST_NO_COLORS)
    #define JC_TEST_ISATTY(_X) 0
#else
    #ifndef JC_TEST_FILENO
        #include <stdio.h> // fileno
        #if defined(_MSC_VER)
            #define JC_TEST_FILENO _fileno
        #else
            #define JC_TEST_FILENO fileno
        #endif
    #endif
    #ifndef JC_TEST_ISATTY
        #if defined(_MSC_VER)
            #include <io.h>
            #define JC_TEST_ISATTY(_X) _isatty(_X)
        #else
            #include <unistd.h>
            #define JC_TEST_ISATTY(_X) isatty(_X)
        #endif
    #endif
#endif

#ifndef JC_TEST_NO_DEATH_TEST
    #include <signal.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define JC_TEST_UNUSED __attribute__ ((unused))
#else
    #define JC_TEST_UNUSED
#endif

#if !defined(JC_TEST_UINT64)
    #include <stdint.h>
    #define JC_TEST_UINT64 uint64_t
    #define JC_TEST_UINT32 uint32_t
#endif

#define JC_TEST_PASS    0
#define JC_TEST_FAIL    1

#define JC_TEST_EVENT_FIXTURE_SETUP     0
#define JC_TEST_EVENT_FIXTURE_TEARDOWN  1
#define JC_TEST_EVENT_TEST_SETUP        2
#define JC_TEST_EVENT_TEST_TEARDOWN     3
#define JC_TEST_EVENT_ASSERT_FAILED     4
#define JC_TEST_EVENT_SUMMARY           5

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
    struct jc_test_factory_base_interface;
    typedef void (*jc_test_void_staticfunc)();
    typedef void (jc_test_base_class::*jc_test_void_memberfunc)();
#else
    typedef int jc_test_base_class;
    typedef int jc_test_factory_base_interface;
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
#if defined(__cplusplus)
    virtual ~jc_test_fixture();
    virtual void SetParam();
#endif
    const char*                     name;       // The name of the fixture
    const char*                     filename;   // The filename of the current ASSERT/EXPECT
    void*                           ctx;        // The test case context
    struct jc_test_fixture*         parent;     // In case of parameterized tests, this points to the first test
    jc_test_base_class*             instance;   // The (parameterized) test case instance
    jc_test_factory_base_interface* factory; // The factory creating parameterized test case instances
    union {
        jc_test_funcs_c     c;
        jc_test_funcs_cpp   cpp;
    };
    jc_test_stats           stats;
    unsigned int            fail:27;
    unsigned int            type:2;     // 0: function, 1: class, 2: params class, 3: params instance
    unsigned int            first:1;    // If it's the first in a range of fixtures
    unsigned int            last:1;     // If it's the last in a range of fixtures
    unsigned int            fatal:1;    // If set, it aborts the test
    unsigned int            index;      // the index of the param in the original params array
    int                     signum:8;   // If we're checking for a signal
    int                     line:16;    // The line of the current ASSERT/EXPECT
    int                     _pad:8;
    int                     num_tests;
    jc_test_entry           tests[JC_TEST_MAX_NUM_TESTS_PER_FIXTURE];
} jc_test_fixture;

typedef struct jc_test_state
{
    jc_test_stats       stats;
    int num_fixtures;
    int is_a_tty:1;
    int _padding1:31;
    int _padding2[2];
    jc_test_entry*      current_test;
    jc_test_fixture*    current_fixture;
    jc_test_fixture*    fixtures[JC_TEST_MAX_NUM_FIXTURES];
    fpos_t              stdout_pos; // set at the start of each test
} jc_test_state;


// May modify the argument list, to remove the test specific arguments
void jc_test_init(int* argc, char** argv);

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

// functions

extern jc_test_state* jc_test_get_state();
extern void jc_test_set_test_fail(int fatal);
extern void jc_test_increment_assertions();
extern int jc_test_streq(const char* a, const char* b);
extern void jc_test_logf(const jc_test_fixture* fixture, const jc_test_entry* test, const jc_test_stats* stats, int event, const char* format, ...);


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

#define JC_TEST_RUN(_NAME_)         jc_test_run_fixture( & __jc_test_fixture_##_NAME_ )
#define JC_TEST_RUN_ALL()           jc_test_run_all_tests(jc_test_get_state())

#define JC_TEST_FILE_AND_LINE       { const int jc_test_line = __LINE__; const char* jc_test_filename = __FILE__;


#if defined(__cplusplus)
static inline jc_test_fixture* jc_test_get_fixture() {
    return jc_test_get_state()->current_fixture;
}
static inline jc_test_entry* jc_test_get_test() {
    return jc_test_get_state()->current_test;
}
template <typename T> char* jc_test_print_value(char* buffer, size_t, const T) {
    buffer[0] = '?'; buffer[1] = 0;
    return buffer+2;
}
template <> char* jc_test_print_value(char* buffer, size_t buffer_len, const double value);
template <> char* jc_test_print_value(char* buffer, size_t buffer_len, const float value);
template <> char* jc_test_print_value(char* buffer, size_t buffer_len, const int value);
template <> char* jc_test_print_value(char* buffer, size_t buffer_len, const unsigned int value);
template <> char* jc_test_print_value(char* buffer, size_t buffer_len, const char* value);

template <typename T1, typename T2>
static inline void jc_test_log_failure(T1 a, T2 b, const char* exprA, const char* exprB, const char* op) {
    char bufferA[64]; jc_test_print_value(bufferA, sizeof(bufferA), a);
    char bufferB[64]; jc_test_print_value(bufferB, sizeof(bufferB), b);
    JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_ASSERT_FAILED, "\nExpected: (%s) %s (%s), actual: %s vs %s", exprA, op, exprB, bufferA, bufferB);
}

template <typename T>
static inline void jc_test_log_failure_boolean(T v, const char* expr) {
    JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_ASSERT_FAILED, "\nValue of: %s\nExpected: %s\n  Actual: %s\n", expr, (!v)?"true":"false", v?"true":"false");
}

template <typename T>
static inline void jc_test_log_failure_str(const T* a, const T* b, const char* exprA, const char* exprB, const char* op) {
    JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_ASSERT_FAILED, "\nValue of: %s %s %s\nExpected: %s\n  Actual: %s\n", exprA, op, exprB, a, b);
}

template <typename T>
int jc_test_cmp_TRUE(T v, const char* expr) {
    if (v) return 1;
    jc_test_log_failure_boolean(v, expr);
    return 0;
}
template <typename T>
int jc_test_cmp_FALSE(T v, const char* expr) {
    if (!v) return 1;
    jc_test_log_failure_boolean(v, expr);
    return 0;
}
template <typename T>
int jc_test_cmp_STREQ(T* a, T* b, const char* exprA, const char* exprB) {
    if (jc_test_streq(a, b)) return 1;
    jc_test_log_failure_str(a, b, exprA, exprB, "==");
    return 0;
}
template <typename T>
int jc_test_cmp_STRNE(T* a, T* b, const char* exprA, const char* exprB) {
    if (!jc_test_streq(a, b)) return 1;
    jc_test_log_failure_str(a, b, exprA, exprB, "!=");
    return 0;
}

#define JC_TEST_COMPARE_FUNC(OP_NAME, OP)                                           \
    template <typename T1, typename T2>                                             \
    int jc_test_cmp_ ## OP_NAME(T1 a, T2 b, const char* exprA, const char* exprB) { \
        if (a OP b) return 1;                                                       \
        jc_test_log_failure(a, b, exprA, exprB, #OP);                               \
        return 0;                                                                   \
    }

JC_TEST_COMPARE_FUNC(EQ, ==)
JC_TEST_COMPARE_FUNC(NE, !=)
JC_TEST_COMPARE_FUNC(LE, <=)
JC_TEST_COMPARE_FUNC(LT, <)
JC_TEST_COMPARE_FUNC(GE, >=)
JC_TEST_COMPARE_FUNC(GT, >)

template <typename INT, typename T>
static inline bool jc_test_cmp_float(T a, T b) {
    union {
        T f; INT i;
    } ua, ub; ua.f = a; ub.f = b;
    return ua.i == ub.i;
}

template <typename T> int jc_test_cmp_EQ(double a, T b, const char* exprA, const char* exprB) {
    if (jc_test_cmp_float<JC_TEST_UINT64>(a, b)) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "==");
    return 0;
}
template <typename T> int jc_test_cmp_EQ(float a, T b, const char* exprA, const char* exprB) {
    if (jc_test_cmp_float<JC_TEST_UINT32>(a, b)) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "==");
    return 0;
}

template <typename T> int jc_test_cmp_NE(double a, T b, const char* exprA, const char* exprB) {
    if (!jc_test_cmp_float<JC_TEST_UINT64>(a, b)) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "!=");
    return 0;
}
template <typename T> int jc_test_cmp_NE(float a, T b, const char* exprA, const char* exprB) {
    if (!jc_test_cmp_float<JC_TEST_UINT32>(a, b)) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "!=");
    return 0;
}

#define JC_TEST_ABS(A) ((A) < 0 ? -(A) : (A))

template <typename T> int jc_test_cmp_NEAR(T a, T b, T epsilon, const char* exprA, const char* exprB, const char* exprC) {
    T diff = JC_TEST_ABS(a - b);
    if (diff <= epsilon) return 1;
    char bA[64]; jc_test_print_value(bA, sizeof(bA), a);
    char bB[64]; jc_test_print_value(bB, sizeof(bB), b);
    char bEpsilon[64]; jc_test_print_value(bEpsilon, sizeof(bEpsilon), epsilon);
    char bDiff[64]; jc_test_print_value(bDiff, sizeof(bDiff), diff);
    JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_ASSERT_FAILED, "\nValue of: abs(%s - %s) <= %s\nExpected: abs(%s - %s) <= %s\n  Actual: abs(%s - %s) == %s\n", exprA, exprB, exprC, bA, bB, bEpsilon, bA, bB, bDiff);
    return 0;
}


#endif

#define JC_TEST_ASSERT_SETUP                                                \
    jc_test_get_fixture()->line = __LINE__;                                 \
    jc_test_get_fixture()->filename = __FILE__;                             \
    jc_test_increment_assertions()

#define JC_TEST_FAILm(_MSG)                                                 \
    {                                                                       \
        JC_TEST_FILE_AND_LINE;                                              \
        JC_TEST_LOGF(   jc_test_get_fixture(),                              \
                        jc_test_get_test(),                                 \
                        &jc_test_get_fixture()->stats,                      \
                        JC_TEST_EVENT_ASSERT_FAILED, "\n\t%s( %d ): %s\n",  \
                        jc_test_filename, jc_test_line, (_MSG));            \
        jc_test_set_test_fail();                                            \
        return JC_TEST_FAIL;                                                \
    }

#if !defined(__cplusplus)

#define JC_TEST_ASSERT_TRUEm(_VALUE, _MSG)                                  \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if( !(_VALUE) ) { JC_TEST_FAILm(_MSG) }                             \
    } while(0)

#define JC_TEST_ASSERT_FALSEm( _VALUE, _MSG)                                \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if( (_VALUE) ) { JC_TEST_FAILm(_MSG) }                              \
    } while(0)

#define JC_TEST_ASSERT_EQm( _EXPECTED, _VALUE, _MSG)                        \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if( !((_EXPECTED) == (_VALUE)) ) { JC_TEST_FAILm(_MSG) }            \
    } while(0)

#define JC_TEST_ASSERT_NEm( _EXPECTED, _VALUE, _MSG)                        \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if( ((_EXPECTED) == (_VALUE)) ) { JC_TEST_FAILm(_MSG) }             \
    } while(0)

#define JC_TEST_ASSERT_GTm( _EXPECTED, _VALUE, _MSG )                       \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if ((_EXPECTED) <= (_VALUE)) { JC_TEST_FAILm(_MSG) }                \
    } while(0)

#define JC_TEST_ASSERT_LTm( _EXPECTED, _VALUE, _MSG )                       \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if ((_EXPECTED) >= (_VALUE)) { JC_TEST_FAILm(_MSG) }                \
    } while(0)

#define JC_TEST_ASSERT_GEm( _EXPECTED, _VALUE, _MSG )                       \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if ((_EXPECTED) < (_VALUE)) { JC_TEST_FAILm(_MSG) }                 \
    } while(0)

#define JC_TEST_ASSERT_LEm( _EXPECTED, _VALUE, _MSG )                       \
    do { JC_TEST_ASSERT_SETUP;                                              \
        if ((_EXPECTED) > (_VALUE)) { JC_TEST_FAILm(_MSG) }                 \
    } while(0)

#define JC_TEST_ASSERT_NEARm( _EXPECTED, _VALUE, _EPSILON, _MSG)                            \
    do { JC_TEST_ASSERT_SETUP;                                                              \
        double _expected = (_EXPECTED);                                                     \
        double _value = (_VALUE);                                                           \
        double _epsilon = (_EPSILON);                                                       \
        if( JC_TEST_ABS(_expected - _value) > _epsilon ) { JC_TEST_FAILm(_MSG) }            \
     } while(0)

#define JC_TEST_ASSERT_STRm( OP, _EXPECTED, _VALUE, _MSG )                      \
    do { JC_TEST_ASSERT_SETUP;                                                  \
        if ( !jc_test_str_##OP(_EXPECTED, _VALUE) ) { JC_TEST_FAILm(_MSG) }     \
    } while(0)

#else

#define JC_ASSERT_TEST_BOOLEAN(OP, VALUE, FATAL)                                \
    {                                                                           \
        JC_TEST_ASSERT_SETUP;                                                   \
        if ( jc_test_cmp_##OP (VALUE, #VALUE) == 0 ) {                          \
            jc_test_set_test_fail(FATAL);                                       \
            if (FATAL) { return; }                                              \
        }                                                                       \
    }
#define JC_ASSERT_TEST_OP(OP, A, B, FATAL)                                      \
    {                                                                           \
        JC_TEST_ASSERT_SETUP;                                                   \
        if ( jc_test_cmp_##OP (A, B, #A, #B) == 0 ) {                           \
            jc_test_set_test_fail(FATAL);                                       \
            if (FATAL) { return; }                                              \
        }                                                                       \
    }
#define JC_ASSERT_TEST_3OP(OP, A, B, C, FATAL)                                  \
    {                                                                           \
        JC_TEST_ASSERT_SETUP;                                                   \
        if ( jc_test_cmp_##OP (A, B, C, #A, #B, #C) == 0 ) {                    \
            jc_test_set_test_fail(FATAL);                                       \
            if (FATAL) { return; }                                              \
        }                                                                       \
    }

#define JC_TEST_ASSERT_TRUE( VALUE )            JC_ASSERT_TEST_BOOLEAN( TRUE, VALUE, 1 )
#define JC_TEST_ASSERT_FALSE( VALUE )           JC_ASSERT_TEST_BOOLEAN( FALSE, VALUE, 1 )
#define JC_TEST_ASSERT_EQ( A, B )               JC_ASSERT_TEST_OP( EQ, A, B, 1 )
#define JC_TEST_ASSERT_NE( A, B )               JC_ASSERT_TEST_OP( NE, A, B, 1 )
#define JC_TEST_ASSERT_LT( A, B )               JC_ASSERT_TEST_OP( LT, A, B, 1 )
#define JC_TEST_ASSERT_GT( A, B )               JC_ASSERT_TEST_OP( GT, A, B, 1 )
#define JC_TEST_ASSERT_LE( A, B )               JC_ASSERT_TEST_OP( LE, A, B, 1 )
#define JC_TEST_ASSERT_GE( A, B )               JC_ASSERT_TEST_OP( GE, A, B, 1 )
#define JC_TEST_ASSERT_STREQ( A, B )            JC_ASSERT_TEST_OP( STREQ, A, B, 1 )
#define JC_TEST_ASSERT_STRNE( A, B )            JC_ASSERT_TEST_OP( STRNE, A, B, 1 )
#define JC_TEST_ASSERT_NEAR( A, B, EPS )        JC_ASSERT_TEST_3OP( NEAR, A, B, EPS, 1 )

#define JC_TEST_EXPECT_TRUE( VALUE )            JC_ASSERT_TEST_BOOLEAN( TRUE, VALUE, 0 )
#define JC_TEST_EXPECT_FALSE( VALUE )           JC_ASSERT_TEST_BOOLEAN( FALSE, VALUE, 0 )
#define JC_TEST_EXPECT_EQ( A, B )               JC_ASSERT_TEST_OP( EQ, A, B, 0 )
#define JC_TEST_EXPECT_NE( A, B )               JC_ASSERT_TEST_OP( NE, A, B, 0 )
#define JC_TEST_EXPECT_LT( A, B )               JC_ASSERT_TEST_OP( LT, A, B, 0 )
#define JC_TEST_EXPECT_GT( A, B )               JC_ASSERT_TEST_OP( GT, A, B, 0 )
#define JC_TEST_EXPECT_LE( A, B )               JC_ASSERT_TEST_OP( LE, A, B, 0 )
#define JC_TEST_EXPECT_GE( A, B )               JC_ASSERT_TEST_OP( GE, A, B, 0 )
#define JC_TEST_EXPECT_STREQ( A, B )            JC_ASSERT_TEST_OP( STREQ, A, B, 0 )
#define JC_TEST_EXPECT_STRNE( A, B )            JC_ASSERT_TEST_OP( STRNE, A, B, 0 )
#define JC_TEST_EXPECT_NEAR( A, B, EPS )        JC_ASSERT_TEST_3OP( NEAR, A, B, EPS, 0 )

#endif

#define JC_TEST_SCOPED_TRACE(_MSG)


// <-- End C TEST API

extern void jc_test_run_fixture(jc_test_fixture* fixture);
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

struct jc_test_factory_base_interface {
    virtual ~jc_test_factory_base_interface();
};

template<typename ParamType>
struct jc_test_factory_interface : public jc_test_factory_base_interface {
    virtual jc_test_params_class<ParamType>* New() = 0;
    virtual void SetParam(const ParamType* param) = 0;
};

template<typename T>
struct jc_test_factory : public jc_test_factory_interface<typename T::param_t> {
    jc_test_params_class<typename T::param_t>* New()    { return new T(); }
    void SetParam(const typename T::param_t* param)     { T::SetParam(param); }
};

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
        fixture = jc_test_alloc_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_PARAMS_CLASS, 0);
        fixture->factory = factory;
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

        jc_test_factory_interface<ParamType>* factory = JC_TEST_CAST(jc_test_factory_interface<ParamType>*, prototype_fixture->factory);

        fixture->param = values->Get();
        factory->SetParam(fixture->param);
        fixture->instance = factory->New();

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

#if !defined(__cplusplus)
#define JC_TEST(testfixture,testfn)                                                                         \
    extern void JC_TEST_MAKE_FUNCTION_NAME(testfixture,testfn)(void* _jc_test_ctx);                         \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) =                                      \
        jc_test_register_test_fn(#testfixture, #testfn, JC_TEST_MAKE_FUNCTION_NAME(testfixture,testfn));    \
    void JC_TEST_MAKE_FUNCTION_NAME(testfixture,testfn)(void* JC_TEST_UNUSED _jc_test_ctx)
#else
    #define JC_TEST(testfixture,testfn)                                                                     \
    class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public jc_test_base_class {                         \
        virtual void TestBody();                                                                            \
    };                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) = jc_test_register_class_test(         \
            #testfixture, #testfn, jc_test_base_class::SetUpTestCase, jc_test_base_class::TearDownTestCase,               \
            new JC_TEST_MAKE_CLASS_NAME(testfixture,testfn), JC_TEST_FIXTURE_TYPE_CLASS);                   \
    void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()
#endif

#define JC_TEST_F(testfixture,testfn)                                                                       \
    class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture {                                \
        virtual void TestBody();                                                                            \
    };                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) = jc_test_register_class_test(         \
            #testfixture, #testfn, testfixture::SetUpTestCase, testfixture::TearDownTestCase,               \
            new JC_TEST_MAKE_CLASS_NAME(testfixture,testfn), JC_TEST_FIXTURE_TYPE_CLASS);                   \
    void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()

#define JC_TEST_P(testfixture,testfn)                                                                       \
    class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture {                                \
        virtual void TestBody();                                                                            \
    };                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) = jc_test_register_param_class_test(   \
            #testfixture, #testfn, testfixture::SetUpTestCase, testfixture::TearDownTestCase,               \
            new jc_test_factory<JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)>());                            \
    void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()

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
    #define EXPECT_TRUE                     JC_TEST_EXPECT_TRUE
    #define EXPECT_FALSE                    JC_TEST_EXPECT_FALSE
    #define EXPECT_EQ                       JC_TEST_EXPECT_EQ
    #define EXPECT_NE                       JC_TEST_EXPECT_NE
    #define EXPECT_GT                       JC_TEST_EXPECT_GT
    #define EXPECT_LT                       JC_TEST_EXPECT_LT
    #define EXPECT_GE                       JC_TEST_EXPECT_GE
    #define EXPECT_LE                       JC_TEST_EXPECT_LE
    #define EXPECT_NEAR                     JC_TEST_EXPECT_NEAR
    #define EXPECT_STREQ                    JC_TEST_EXPECT_STREQ
    #define EXPECT_STRNE                    JC_TEST_EXPECT_STRNE

    #define SCOPED_TRACE                    JC_TEST_SCOPED_TRACE
#endif

#endif // JC_TEST_H

#ifdef JC_TEST_IMPLEMENTATION
#undef JC_TEST_IMPLEMENTATION

#define JC_TEST_PRINT_TYPE_FN(TYPE, FORMAT) \
    template <> char* jc_test_print_value(char* buffer, size_t buffer_len, const TYPE value) { \
        return buffer + JC_TEST_SNPRINTF(buffer, buffer_len, FORMAT, value); \
    }

JC_TEST_PRINT_TYPE_FN(double, "%f")
JC_TEST_PRINT_TYPE_FN(int, "%d")
JC_TEST_PRINT_TYPE_FN(unsigned int, "%u")
JC_TEST_PRINT_TYPE_FN(char*, "%s")

template <> char* jc_test_print_value(char* buffer, size_t buffer_len, const float value) {
    return buffer + JC_TEST_SNPRINTF(buffer, buffer_len, "%f", JC_TEST_STATIC_CAST(double, value));
}

#define JC_TEST_CLR_DEFAULT "\x1B[0m"
#define JC_TEST_CLR_RED     "\x1B[31m"
#define JC_TEST_CLR_GREEN   "\x1B[32m"
#define JC_TEST_CLR_YELLOW  "\x1B[33m"
#define JC_TEST_CLR_BLUE    "\x1B[34m"
#define JC_TEST_CLR_MAGENTA "\x1B[35m"
#define JC_TEST_CLR_CYAN    "\x1B[36m"
#define JC_TEST_CLR_WHITE   "\x1B[37m"

#define JC_TEST_COL(CLR) (jc_test_get_state()->is_a_tty ? JC_TEST_CLR_ ## CLR : "")

static inline int jc_test_cmp_fpos_t(const fpos_t* a, const fpos_t* b) {
    #if defined(__linux__)
        return a->__pos == b->__pos;
    #else
        return *a == *b;
    #endif
}

static size_t jc_test_snprint_time(char* buffer, size_t buffer_len, jc_test_time_t t);

#if defined(__GNUC__) || defined(__clang__)
__attribute__ ((format (printf, 5, 6)))
#endif
void jc_test_logf(const jc_test_fixture* fixture, const jc_test_entry* test, const jc_test_stats* stats, int event, const char* format, ...) {
    char buffer[1024];
    char* cursor = buffer;
    const char* end = buffer + sizeof(buffer);
    const char* pass = jc_test_get_state()->is_a_tty ? JC_TEST_CLR_GREEN "PASS" JC_TEST_CLR_DEFAULT : "PASS";
    const char* fail = jc_test_get_state()->is_a_tty ? JC_TEST_CLR_RED "FAIL" JC_TEST_CLR_DEFAULT : "FAIL";

    if (event == JC_TEST_EVENT_FIXTURE_SETUP) {
        JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "%s%s%s\n", JC_TEST_COL(CYAN), fixture->name, JC_TEST_COL(DEFAULT));
    } else if (event == JC_TEST_EVENT_FIXTURE_TEARDOWN) {
        jc_test_time_t totaltime = fixture->stats.totaltime;
        if (fixture->parent) {
            totaltime = fixture->parent->stats.totaltime;
        }
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "%s%s%s took ", JC_TEST_COL(CYAN), fixture->name, JC_TEST_COL(DEFAULT));
        cursor += jc_test_snprint_time(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), totaltime);
        JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n");

    } else if (event == JC_TEST_EVENT_TEST_SETUP) {
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "%s%s%s", JC_TEST_COL(YELLOW), test->name, JC_TEST_COL(DEFAULT));
        if (fixture->index != 0xFFFFFFFF) {
            JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "/%d ", fixture->index);
        }
    } else if (event == JC_TEST_EVENT_TEST_TEARDOWN) {
        fpos_t stdout_pos;
        fgetpos(stdout, &stdout_pos);
        const char* return_char = jc_test_cmp_fpos_t(&stdout_pos, &jc_test_get_state()->stdout_pos) ? "\r" : "\n";

        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "%s%s%s%s", return_char, JC_TEST_COL(YELLOW), test->name, JC_TEST_COL(DEFAULT));
        if (fixture->index != 0xFFFFFFFF) {
            cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "/%d ", fixture->index);
        }
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), " %s (", fixture->fail == JC_TEST_PASS ? pass : fail);
        cursor += jc_test_snprint_time(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), stats->totaltime);
        JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), ")\n");
    } else if (event == JC_TEST_EVENT_ASSERT_FAILED) {
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n%s%s%s:%d:", JC_TEST_COL(MAGENTA), fixture->filename, JC_TEST_COL(DEFAULT), fixture->line);
        va_list ap;
        va_start(ap, format);
        vsnprintf(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), format, ap);
        va_end(ap);
    } else if (event == JC_TEST_EVENT_SUMMARY) {
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "Ran %d tests, with %d assertions in ", stats->num_tests, stats->num_assertions);
        cursor += jc_test_snprint_time(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), stats->totaltime);
        if( stats->num_fail)
            JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n%d tests passed, and %d tests %sFAILED%s\n", stats->num_pass, stats->num_fail, JC_TEST_COL(RED), JC_TEST_COL(DEFAULT));
        else
            JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n%d tests %sPASSED%s\n", stats->num_pass, JC_TEST_COL(GREEN), JC_TEST_COL(DEFAULT));
    }
    buffer[sizeof(buffer)-1] = 0;
    JC_TEST_PRINTF("%s", buffer);
}

#undef JC_TEST_COL

#if defined(__cplusplus)

static void jc_test_global_cleanup();

static inline void jc_test_memset(void* _mem, unsigned int pattern, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        JC_TEST_CAST(unsigned char*, _mem)[i] = JC_TEST_STATIC_CAST(unsigned char, pattern & 0xFF);
    }
}

void jc_test_memcpy(void* dst, void* src, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        JC_TEST_CAST(unsigned char*, dst)[i] = JC_TEST_CAST(unsigned char*, src)[i];
    }
}

int jc_test_streq(const char* a, const char* b) {
    if (a == 0) return (b == 0) ? 1 : 0;
    if (b == 0) return 0;
    while (*a && (*a == *b)) {
        ++a; ++b;
    }
    return (*a - *b) == 0 ? 1 : 0;
}

jc_test_factory_base_interface::~jc_test_factory_base_interface() {}

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
    fixture->instance = 0;
    fixture->fail = 0;
    fixture->fatal = 0;
    fixture->index = 0xFFFFFFFF;
    fixture->num_tests = 0;
    fixture->first = fixture->last = 1;
    fixture->signum = 0;
    jc_test_memset(&fixture->c, 0, sizeof(fixture->c));
    jc_test_memset(&fixture->stats, 0, sizeof(fixture->stats));
    jc_test_memset(fixture->tests, 0, sizeof(fixture->tests));
    JC_TEST_ASSERT_FN(jc_test_get_state()->num_fixtures < JC_TEST_MAX_NUM_FIXTURES);
    return jc_test_get_state()->fixtures[jc_test_get_state()->num_fixtures++] = fixture;
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
    for( int i = 0; i < jc_test_get_state()->num_fixtures; ++i) {
        if (jc_test_get_state()->fixtures[i]->type == fixture_type && jc_test_streq(jc_test_get_state()->fixtures[i]->name, name))
            return jc_test_get_state()->fixtures[i];
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

static void jc_test_signal_hook_sigabrt(int signum) {
    if (jc_test_get_state()->current_fixture)
        jc_test_get_state()->current_fixture->signum = signum;
}

static void jc_test_global_cleanup() {
    jc_test_state* state = jc_test_get_state();
    for (int i = 0; i < state->num_fixtures; ++i) {
        for (int j = 0; j < state->fixtures[i]->num_tests; ++j) {
            if (state->fixtures[i]->tests[j].instance)
                delete state->fixtures[i]->tests[j].instance;
        }
        delete state->fixtures[i]->instance;
        delete state->fixtures[i];
    }
}

#endif // __cplusplus

void jc_test_set_test_fail(int fatal) {
    jc_test_get_fixture()->fail = JC_TEST_FAIL;
    jc_test_get_fixture()->fatal |= fatal;
}

void jc_test_increment_assertions() {
    jc_test_get_fixture()->stats.num_assertions++;
}

static size_t jc_test_snprint_time(char* buffer, size_t buffer_len, jc_test_time_t t) { // Micro seconds
#ifdef _MSC_VER
    #define JC_TEST_MICROSECONDS_STR "us"
#else
    #define JC_TEST_MICROSECONDS_STR "\u00b5s"
#endif
    int printed;
    if( t < 5000 )
        printed = JC_TEST_SNPRINTF(buffer, buffer_len, "%g %s", JC_TEST_STATIC_CAST(double, t), JC_TEST_MICROSECONDS_STR);
    else if( t < 500000 )
        printed = JC_TEST_SNPRINTF(buffer, buffer_len, "%g %s", t / 1000.0, "ms");
    else
        printed = JC_TEST_SNPRINTF(buffer, buffer_len, "%g %s", t / 1000000.0, "s");
    return JC_TEST_STATIC_CAST(size_t, printed);
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

void jc_test_run_fixture(jc_test_fixture* fixture) {
    jc_test_get_state()->current_fixture = fixture;

    if (fixture->type == JC_TEST_FIXTURE_TYPE_PARAMS_CLASS) {
        return;
    }

    jc_test_memset(&fixture->stats, 0, sizeof(fixture->stats));

    fixture->stats.totaltime = 0;
    jc_test_time_t timestart = JC_TEST_TIMING_FUNC();
    if (fixture->first) {
        JC_TEST_LOGF(fixture, 0, 0, JC_TEST_EVENT_FIXTURE_SETUP, "");
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

    for (int count = 0; count < fixture->num_tests; ++count)
    {
        jc_test_entry* test = &fixture->tests[count];
        fixture->fail = JC_TEST_PASS;
        jc_test_get_state()->current_test = test;

        JC_TEST_LOGF(fixture, test, 0, JC_TEST_EVENT_TEST_SETUP, "");

        fgetpos(stdout, &jc_test_get_state()->stdout_pos);

        jc_test_time_t teststart = 0;
        jc_test_time_t testend = 0;

        jc_test_func fns[3] = { fixture->c.test_setup, test->test, fixture->c.test_teardown };
        jc_test_void_memberfunc cppfns[3] = { &jc_test_base_class::SetUp, &jc_test_base_class::TestBody, &jc_test_base_class::TearDown };

        #if defined(__cplusplus)
        jc_test_base_class* instance = fixture->instance; // parameterized test
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

#if defined(__cplusplus)
            JC_TEST_INVOKE_MEMBER_FN(instance, cppfns[i]);
#else
            fns[i](fixture->ctx);
#endif

            if( i == 1 ) {
                testend = JC_TEST_TIMING_FUNC();
            }

            if( fixture->fatal ) {
                break;
            }
        }

        jc_test_stats test_stats = {0, 0, 0, 0, testend-teststart};
        JC_TEST_LOGF(fixture, test, &test_stats, JC_TEST_EVENT_TEST_TEARDOWN, "");

        if( fixture->fail == JC_TEST_PASS )
            ++fixture->stats.num_pass;
        else
            ++fixture->stats.num_fail;
        ++fixture->stats.num_tests;
    }
    jc_test_get_state()->current_test = 0;

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
        JC_TEST_LOGF(fixture, 0, &fixture->stats, JC_TEST_EVENT_FIXTURE_TEARDOWN, "");
    }
    jc_test_get_state()->current_fixture = 0;
}

int jc_test_run_all_tests(jc_test_state* state) {
    state->stats.totaltime = 0;
    for( int i = 0; i < state->num_fixtures; ++i )
    {
        if( state->fixtures[i] )
        {
            jc_test_run_fixture( state->fixtures[i] );

            state->stats.num_assertions += state->fixtures[i]->stats.num_assertions;
            state->stats.num_pass += state->fixtures[i]->stats.num_pass;
            state->stats.num_fail += state->fixtures[i]->stats.num_fail;
            state->stats.num_tests += state->fixtures[i]->stats.num_tests;
            state->stats.totaltime += state->fixtures[i]->stats.totaltime;
        }
    }

    JC_TEST_LOGF(0, 0, &state->stats, JC_TEST_EVENT_SUMMARY, "");
    return state->stats.num_fail;
}

#if defined(_MSC_VER)

jc_test_time_t jc_test_get_time(void) {
    LARGE_INTEGER tickPerSecond;
    LARGE_INTEGER tick;
    QueryPerformanceFrequency(&tickPerSecond);
    QueryPerformanceCounter(&tick);
    return JC_TEST_STATIC_CAST(jc_test_time_t, tick.QuadPart / (tickPerSecond.QuadPart / 1000000));
}

#else

jc_test_time_t jc_test_get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return JC_TEST_STATIC_CAST(jc_test_time_t, tv.tv_sec) * 1000000U + JC_TEST_STATIC_CAST(jc_test_time_t, tv.tv_usec);
}

#endif


#if !defined(__cplusplus)
static jc_test_state jc_test_global_state = { {0, 0, 0, 0, 0}, 0, 0, 0, {0, 0}, 0, 0, {0}, 0 };
#else
static jc_test_state jc_test_global_state;
#endif

jc_test_state* jc_test_get_state() {
    return &jc_test_global_state;
}

void jc_test_init(int* argc, char** argv) {
    (void)argc; (void)argv;
    JC_TEST_ATEXIT(jc_test_global_cleanup);

#ifndef JC_TEST_NO_DEATH_TEST
    signal(SIGABRT, jc_test_signal_hook_sigabrt);
#endif
    FILE* o = stdout;
    jc_test_global_state.is_a_tty = JC_TEST_ISATTY(JC_TEST_FILENO(o));
}

#endif
