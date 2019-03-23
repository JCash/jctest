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

struct MyTest : public jc_test_base_class {
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

struct MyParamTest : public jc_test_params_class<ParamType> {
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
*/


/*
WIP!
This new GTEST-like api needs a lot of testing
TODOS:
 * SCOPED_TRACE
 * Handle input parameters (e.g. --jctest_filter=FooBar*)
 * Support ASSERT_NO_FATAL_FAILURE (or similar)

*/

#ifndef JC_TEST_H
#define JC_TEST_H

#ifndef JC_TEST_LOGF
    #include <stdarg.h> //va_list
    #include <stdio.h>  //snprintf
    // Can be overridden to log in a different way
    #define JC_TEST_LOGF jc_test_logf

    #if defined(_MSC_VER)
        #define JC_TEST_SNPRINTF _snprintf
    #else
        #define JC_TEST_SNPRINTF snprintf
    #endif
#endif

#ifndef JC_TEST_ASSERT_FN
    #include <assert.h>
    #define JC_TEST_ASSERT_FN assert
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
    #include <setjmp.h> // longjmp
    #define JC_TEST_SETJMP _setjmp
#endif

// C++0x and above
#if !defined(_MSC_VER)
#pragma GCC diagnostic push
#if !defined(__GNUC__)
    #if __cplusplus >= 199711L
        #pragma GCC diagnostic ignored "-Wc++98-compat"
    #endif
#endif
#if __cplusplus >= 201103L
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define JC_TEST_UNUSED __attribute__ ((unused))
#else
    #define JC_TEST_UNUSED
#endif

#if !defined(JC_TEST_UINT64)
    #include <stdint.h>
    #define JC_TEST_UINT64 uint64_t
#endif

#define JC_TEST_EVENT_FIXTURE_SETUP     0
#define JC_TEST_EVENT_FIXTURE_TEARDOWN  1
#define JC_TEST_EVENT_TEST_SETUP        2
#define JC_TEST_EVENT_TEST_TEARDOWN     3
#define JC_TEST_EVENT_ASSERT_FAILED     4
#define JC_TEST_EVENT_SUMMARY           5
#define JC_TEST_EVENT_GENERIC           6

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

#if __x86_64__ || __ppc64__ || _WIN64
    #define JC_TEST_PAD_IF_64BIT(X)   char _pad[X]
#else
    #define JC_TEST_PAD_IF_64BIT(X)
#endif

// Returns the user defined context for the fixture
typedef void* (*jc_test_fixture_setup_func)();

struct jc_test_base_class;
struct jc_test_factory_base_interface;
typedef void (*jc_test_void_staticfunc)();
typedef void (jc_test_base_class::*jc_test_void_memberfunc)();

typedef struct jc_test_entry {
    const char*                     name;
    jc_test_base_class*             instance;
    jc_test_factory_base_interface* factory;    // Factory for parameterized tests
    unsigned int                    fail:1;
    unsigned int                    skipped:1;
    unsigned int                    :30;
    JC_TEST_PAD_IF_64BIT(4);
} jc_test_entry;

typedef struct jc_test_stats {
    int num_pass;
    int num_fail:16;
    int num_skipped:16;
    int num_assertions;
    int num_tests;
    jc_test_time_t totaltime;
} jc_test_stats;

typedef struct jc_test_fixture {
    virtual ~jc_test_fixture();
    virtual void SetParam();
    const char*                 name;       // The name of the fixture
    const char*                 filename;   // The filename of the current ASSERT/EXPECT
    struct jc_test_fixture*     parent;     // In case of parameterized tests, this points to the first test
    jc_test_void_staticfunc     fixture_setup;
    jc_test_void_staticfunc     fixture_teardown;
    jc_test_stats               stats;
    unsigned int                fail:27;
    unsigned int                type:2;     // 0: function, 1: class, 2: params class, 3: params instance
    unsigned int                first:1;    // If it's the first in a range of fixtures
    unsigned int                last:1;     // If it's the last in a range of fixtures
    unsigned int                fatal:1;    // If set, it aborts the test
    unsigned int                index;      // the index of the param in the original params array
    int                         signum:8;   // If we're checking for a signal
    int                         line:16;    // The line of the current ASSERT/EXPECT
    int                         _pad:8;
    int                         num_tests;
    jc_test_entry               tests[JC_TEST_MAX_NUM_TESTS_PER_FIXTURE];
} jc_test_fixture;

typedef struct jc_test_state {
    jc_test_entry*      current_test;
    jc_test_fixture*    current_fixture;
    jc_test_fixture*    fixtures[JC_TEST_MAX_NUM_FIXTURES];
    jc_test_stats       stats;
    jmp_buf             jumpenv;    // Set before trying to catch exceptions
    int                 num_fixtures:31;
    int                 is_a_tty:1;
    #if !defined(__APPLE__)
    int                 _pad;
    #endif
} jc_test_state;

// ***************************************************************************************
// PUBLIC functions

// May modify the argument list, to remove the test specific arguments
extern void jc_test_init(int* argc, char** argv);

extern int jc_test_run_all_tests(jc_test_state* state);

// ***************************************************************************************
// Private functions

extern jc_test_state* jc_test_get_state();
extern void jc_test_set_test_fail(int fatal);
extern void jc_test_increment_assertions();
extern int jc_test_streq(const char* a, const char* b);
extern void jc_test_logf(const jc_test_fixture* fixture, const jc_test_entry* test, const jc_test_stats* stats, int event, const char* format, ...);
extern int jc_test_cmp_float_eq(double, double);
extern int jc_test_cmp_STREQ(const char* a, const char* b, const char* exprA, const char* exprB);
extern int jc_test_cmp_STRNE(const char* a, const char* b, const char* exprA, const char* exprB);
extern int jc_test_cmp_NEAR(double a, double b, double epsilon, const char* exprA, const char* exprB, const char* exprC);

#define JC_TEST_CAST(_TYPE_, _EXPR_)            reinterpret_cast< _TYPE_ >( _EXPR_ )
#define JC_TEST_STATIC_CAST(_TYPE_, _EXPR_)     static_cast< _TYPE_ >( _EXPR_ )

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
    JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_ASSERT_FAILED, "\nExpected: (%s) %s (%s), actual: %s vs %s\n", exprA, op, exprB, bufferA, bufferB);
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

template <typename T> int jc_test_cmp_EQ(double a, T b, const char* exprA, const char* exprB) {
    if (jc_test_cmp_float_eq(JC_TEST_STATIC_CAST(double, a), JC_TEST_STATIC_CAST(double, b))) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "==");
    return 0;
}
template <typename T> int jc_test_cmp_EQ(float a, T b, const char* exprA, const char* exprB) {
    if (jc_test_cmp_float_eq(JC_TEST_STATIC_CAST(double, a), JC_TEST_STATIC_CAST(double, b))) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "==");
    return 0;
}

template <typename T> int jc_test_cmp_NE(double a, T b, const char* exprA, const char* exprB) {
    if (!jc_test_cmp_float_eq(JC_TEST_STATIC_CAST(double, a), JC_TEST_STATIC_CAST(double, b))) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "!=");
    return 0;
}
template <typename T> int jc_test_cmp_NE(float a, T b, const char* exprA, const char* exprB) {
    if (!jc_test_cmp_float_eq(JC_TEST_STATIC_CAST(double, a), JC_TEST_STATIC_CAST(double, b))) return 1;
    jc_test_log_failure(a, b, exprA, exprB, "!=");
    return 0;
}

// The only way to match this function, is with a null literal (since the type is hidden)
char _jc_test_is_null_literal(struct _jc_test_null_literal* p);
char (&_jc_test_is_null_literal(...))[2];
#define JC_TEST_IS_NULL_LITERAL(VALUE)          (sizeof(_jc_test_is_null_literal(VALUE)) == 1)

template <typename T> struct jc_test_is_pointer {
    static const bool value = false;
};
template <typename T> struct jc_test_is_pointer<T*> {
    static const bool value = true;
};
template <bool> struct jc_test_enable_argument;
template <> struct jc_test_enable_argument<true> { typedef void type; };


template<bool is_null_literal>
struct jc_test_cmp_eq_helper {
    template<typename T1, typename T2>
    static int compare(const T1& a, const T2& b, const char* exprA, const char* exprB) {
        return jc_test_cmp_EQ(a, b, exprA, exprB);
    }
};

template<>
struct jc_test_cmp_eq_helper<true> {
    template<typename T1, typename T2>
    static int compare(const T1& a, const T2& b, const char* exprA, const char* exprB,
                        typename jc_test_enable_argument<!jc_test_is_pointer<T2>::value>::type* = 0) {
        return jc_test_cmp_EQ(a, b, exprA, exprB);
    }
    template<typename T>
    static int compare(_jc_test_null_literal*, T* b, const char* exprA, const char* exprB) {
        return jc_test_cmp_EQ(JC_TEST_STATIC_CAST(T*, 0), b, exprA, exprB);
    }
};

#define JC_TEST_ASSERT_SETUP                                                    \
    jc_test_get_fixture()->line = __LINE__;                                     \
    jc_test_get_fixture()->filename = __FILE__;                                 \
    jc_test_increment_assertions()

#define JC_TEST_FATAL_FAILURE       jc_test_set_test_fail(1); return
#define JC_TEST_NON_FATAL_FAILURE   jc_test_set_test_fail(0);

#define JC_ASSERT_TEST_BOOLEAN(OP, VALUE, FAIL_FUNC)                            \
    do {                                                                        \
        JC_TEST_ASSERT_SETUP;                                                   \
        if ( jc_test_cmp_##OP (VALUE, #VALUE) == 0 ) {                          \
            FAIL_FUNC;                                                          \
        }                                                                       \
    } while(0)
#define JC_ASSERT_TEST_EQ(A, B, FAIL_FUNC)                                      \
    do {                                                                        \
        JC_TEST_ASSERT_SETUP;                                                   \
        if ( jc_test_cmp_eq_helper<JC_TEST_IS_NULL_LITERAL(A)>::compare(A, B, #A, #B) == 0 ) { \
            FAIL_FUNC;                                                          \
        }                                                                       \
    } while(0)
#define JC_ASSERT_TEST_OP(OP, A, B, FAIL_FUNC)                                  \
    do {                                                                        \
        JC_TEST_ASSERT_SETUP;                                                   \
        if ( jc_test_cmp_##OP (A, B, #A, #B) == 0 ) {                           \
            FAIL_FUNC;                                                          \
        }                                                                       \
    } while(0)
#define JC_ASSERT_TEST_3OP(OP, A, B, C, FAIL_FUNC)                              \
    do {                                                                        \
        JC_TEST_ASSERT_SETUP;                                                   \
        if ( jc_test_cmp_##OP (A, B, C, #A, #B, #C) == 0 ) {                    \
            FAIL_FUNC;                                                          \
        }                                                                       \
    } while(0)
#define JC_ASSERT_TEST_DEATH_OP(STATEMENT, RE, FAIL_FUNC)                       \
    do {                                                                        \
        JC_TEST_ASSERT_SETUP;                                                   \
        if (JC_TEST_SETJMP(jc_test_get_state()->jumpenv) == 0) {                \
            JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_GENERIC, "\njc_test: Death test begin ->\n"); \
            STATEMENT;                                                          \
            JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_ASSERT_FAILED, "\nExpected this to fail: %s", #STATEMENT ); \
            FAIL_FUNC;                                                          \
        }                                                                       \
        JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_GENERIC, "jc_test: <- Death test end\n"); \
    } while(0)

// TEST API Begin -->

#define JC_TEST_RUN_ALL()               jc_test_run_all_tests(jc_test_get_state())

#define SKIP()                          { jc_test_set_test_skipped(); return; }

#define ASSERT_TRUE( VALUE )            JC_ASSERT_TEST_BOOLEAN( TRUE, VALUE, JC_TEST_FATAL_FAILURE )
#define ASSERT_FALSE( VALUE )           JC_ASSERT_TEST_BOOLEAN( FALSE, VALUE, JC_TEST_FATAL_FAILURE )
#define ASSERT_EQ( A, B )               JC_ASSERT_TEST_EQ( A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_NE( A, B )               JC_ASSERT_TEST_OP( NE, A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_LT( A, B )               JC_ASSERT_TEST_OP( LT, A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_GT( A, B )               JC_ASSERT_TEST_OP( GT, A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_LE( A, B )               JC_ASSERT_TEST_OP( LE, A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_GE( A, B )               JC_ASSERT_TEST_OP( GE, A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_STREQ( A, B )            JC_ASSERT_TEST_OP( STREQ, A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_STRNE( A, B )            JC_ASSERT_TEST_OP( STRNE, A, B, JC_TEST_FATAL_FAILURE )
#define ASSERT_NEAR( A, B, EPS )        JC_ASSERT_TEST_3OP( NEAR, A, B, EPS, JC_TEST_FATAL_FAILURE )
#define ASSERT_DEATH_IF_SUPPORTED(S, RE) JC_ASSERT_TEST_DEATH_OP(S, RE, JC_TEST_FATAL_FAILURE )

#define EXPECT_TRUE( VALUE )            JC_ASSERT_TEST_BOOLEAN( TRUE, VALUE, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_FALSE( VALUE )           JC_ASSERT_TEST_BOOLEAN( FALSE, VALUE, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_EQ( A, B )               JC_ASSERT_TEST_EQ( A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_NE( A, B )               JC_ASSERT_TEST_OP( NE, A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_LT( A, B )               JC_ASSERT_TEST_OP( LT, A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_GT( A, B )               JC_ASSERT_TEST_OP( GT, A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_LE( A, B )               JC_ASSERT_TEST_OP( LE, A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_GE( A, B )               JC_ASSERT_TEST_OP( GE, A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_STREQ( A, B )            JC_ASSERT_TEST_OP( STREQ, A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_STRNE( A, B )            JC_ASSERT_TEST_OP( STRNE, A, B, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_NEAR( A, B, EPS )        JC_ASSERT_TEST_3OP( NEAR, A, B, EPS, JC_TEST_NON_FATAL_FAILURE )
#define EXPECT_DEATH_IF_SUPPORTED(S, RE) JC_ASSERT_TEST_DEATH_OP(S, RE, JC_TEST_NON_FATAL_FAILURE )

#define SCOPED_TRACE(_MSG)

struct jc_test_base_class {
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
struct jc_test_value_iterator {
    virtual ~jc_test_value_iterator();
    virtual const T* Get() const = 0;
    virtual void Advance() = 0;
    virtual bool Empty() const = 0;   // return false when out of values
    virtual void Rewind() = 0;
};
template<typename T> jc_test_value_iterator<T>::~jc_test_value_iterator() {} // separate line to silence warning

template<typename T>
struct jc_test_array_iterator : public jc_test_value_iterator<T> {
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
struct jc_test_params_class : public jc_test_base_class {
    typedef ParamType param_t;
    jc_test_params_class() {}
    static const ParamType&    GetParam()                           { return *param; }
    static void                SetParam(const ParamType* _param)    { param = _param; }
    static const ParamType* param;
};
template<typename ParamType> const ParamType* jc_test_params_class<ParamType>::param = 0;

template<typename ParamType>
struct jc_test_fixture_with_param : public jc_test_fixture {
    void SetParam() { JC_TEST_CAST(jc_test_params_class<ParamType>*, jc_test_get_state()->current_test)->SetParam(param); }
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

#define JC_TEST_FIXTURE_TYPE_CLASS              0
#define JC_TEST_FIXTURE_TYPE_PARAMS_CLASS       1
#define JC_TEST_FIXTURE_TYPE_TYPED_CLASS        2

extern jc_test_fixture* jc_test_find_fixture(const char* name, unsigned int fixture_type);
extern jc_test_fixture* jc_test_alloc_fixture(const char* name, unsigned int fixture_type);
extern jc_test_fixture* jc_test_create_fixture(jc_test_fixture* fixture, const char* name, unsigned int fixture_type);
extern jc_test_entry*   jc_test_add_test_to_fixture(jc_test_fixture* fixture, const char* test_name, jc_test_base_class* instance, jc_test_factory_base_interface* factory);
extern void             jc_test_memcpy(void* dst, void* src, size_t size);

extern int jc_test_register_class_test(const char* fixture_name, const char* test_name,
                                                jc_test_void_staticfunc class_setup, jc_test_void_staticfunc class_teardown,
                                                jc_test_base_class* instance, unsigned int fixture_type);
template <typename ParamType>
int jc_test_register_param_class_test(const char* fixture_name, const char* test_name,
                        jc_test_void_staticfunc class_setup, jc_test_void_staticfunc class_teardown,
                        jc_test_factory_interface<ParamType>* factory) {
    jc_test_fixture* fixture = jc_test_find_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_PARAMS_CLASS);
    if (!fixture) {
        fixture = jc_test_alloc_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_PARAMS_CLASS);
        fixture->fixture_setup = class_setup;
        fixture->fixture_teardown = class_teardown;
    }
    jc_test_add_test_to_fixture(fixture, test_name, 0, factory);
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
            fixture = jc_test_alloc_fixture(fixture_name, JC_TEST_FIXTURE_TYPE_CLASS);
            fixture->fixture_setup = TestClass::SetUpTestCase;
            fixture->fixture_teardown = TestClass::TearDownTestCase;
        }
        fixture->index = index;
        jc_test_add_test_to_fixture(fixture, test_name, new TestClass, 0);
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
jc_test_fixture* jc_test_alloc_fixture_with_param(const char* name, unsigned int type) {
    return jc_test_create_fixture(new jc_test_fixture_with_param<ParamType>, name, type);
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
                                jc_test_alloc_fixture_with_param<ParamType>(fixture_name, JC_TEST_FIXTURE_TYPE_CLASS) );

        fixture->first = first_fixture == 0 ? 1 : 0;
        if (!first_fixture) {
            first_fixture = fixture; // A silly trick to make the first fixture accumulate all the timings from this batch
        }
        fixture->parent = first_fixture;
        fixture->index = index++;
        fixture->param = values->Get();
        fixture->fixture_setup = prototype_fixture->fixture_setup;
        fixture->fixture_teardown = prototype_fixture->fixture_teardown;

        fixture->num_tests = prototype_fixture->num_tests;
        for (int i = 0; i < fixture->num_tests; ++i) {
            fixture->tests[i].name = prototype_fixture->tests[i].name;
            fixture->tests[i].factory = 0;

            jc_test_factory_interface<ParamType>* factory = JC_TEST_CAST(jc_test_factory_interface<ParamType>*, prototype_fixture->tests[i].factory);
            factory->SetParam(fixture->param);
            fixture->tests[i].instance = factory->New();
        }

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

#define TEST(testfixture,testfn)                                                                                         \
class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public jc_test_base_class {                                             \
    virtual void TestBody();                                                                                                \
};                                                                                                                          \
static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) JC_TEST_UNUSED = jc_test_register_class_test(              \
        #testfixture, #testfn, jc_test_base_class::SetUpTestCase, jc_test_base_class::TearDownTestCase,                     \
        new JC_TEST_MAKE_CLASS_NAME(testfixture,testfn), JC_TEST_FIXTURE_TYPE_CLASS);                                       \
void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()

#define TEST_F(testfixture,testfn)                                                                                       \
    class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture {                                                \
        virtual void TestBody();                                                                                            \
    };                                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) JC_TEST_UNUSED = jc_test_register_class_test(          \
            #testfixture, #testfn, testfixture::SetUpTestCase, testfixture::TearDownTestCase,                               \
            new JC_TEST_MAKE_CLASS_NAME(testfixture,testfn), JC_TEST_FIXTURE_TYPE_CLASS);                                   \
    void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()

#define TEST_P(testfixture,testfn)                                                                                       \
    class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture {                                                \
        virtual void TestBody();                                                                                            \
    };                                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) JC_TEST_UNUSED = jc_test_register_param_class_test(    \
            #testfixture, #testfn, testfixture::SetUpTestCase, testfixture::TearDownTestCase,                               \
            new jc_test_factory<JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)>());                                            \
    void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)::TestBody()

#define INSTANTIATE_TEST_CASE_P(prefix,testfixture,testvalues)                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(prefix,testfixture,__LINE__) JC_TEST_UNUSED =                                       \
        jc_test_register_param_tests<testfixture::param_t>(#testfixture, #prefix "/" #testfixture, testvalues)


template<typename T> struct jc_test_typed_list {
    typedef T type;
};

template<template <typename T> class BaseClass> struct jc_test_template_sel {
    template <typename TypeParam> struct bind {
        typedef BaseClass<TypeParam> type;
    };
};

#define TYPED_TEST_SUITE(testfixture,testtypes)                                                     \
    typedef jc_test_typed_list<testtypes>::type JC_TEST_MAKE_NAME2(testfixture,Types)

#define TYPED_TEST(testfixture,testfn)                                                              \
    template<typename T> class JC_TEST_MAKE_CLASS_NAME(testfixture,testfn) : public testfixture<T> {        \
        virtual void TestBody();                                                                            \
        typedef testfixture<T> TestFixture;                                                                 \
        typedef T TypeParam;                                                                                \
    };                                                                                                      \
    static int JC_TEST_MAKE_UNIQUE_NAME(testfixture,testfn,__LINE__) JC_TEST_UNUSED =                       \
            jc_test_register_typed_class_test<                                                              \
                jc_test_template_sel<JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)>,                          \
                JC_TEST_MAKE_NAME2(testfixture,Types)>::register_test(#testfixture, #testfn, 0);            \
    template<typename T> void JC_TEST_MAKE_CLASS_NAME(testfixture,testfn)<T>::TestBody()


#if !defined(_MSC_VER)
#pragma GCC diagnostic pop
#endif

#endif // JC_TEST_H

#ifdef JC_TEST_IMPLEMENTATION
#undef JC_TEST_IMPLEMENTATION

#if !defined(_MSC_VER)
#pragma GCC diagnostic push
#if !defined(__GNUC__)
#endif
#if __cplusplus >= 201103L
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#endif

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

static size_t jc_test_snprint_time(char* buffer, size_t buffer_len, jc_test_time_t t);

#if defined(__GNUC__) || defined(__clang__)
__attribute__ ((format (printf, 5, 6)))
#endif
void jc_test_logf(const jc_test_fixture* fixture, const jc_test_entry* test, const jc_test_stats* stats, int event, const char* format, ...) {
    char buffer[1024];
    char* cursor = buffer;
    const char* end = buffer + sizeof(buffer);

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
        const char* pass = jc_test_get_state()->is_a_tty ? JC_TEST_CLR_GREEN "PASS" JC_TEST_CLR_DEFAULT : "PASS";
        const char* fail = jc_test_get_state()->is_a_tty ? JC_TEST_CLR_RED "FAIL" JC_TEST_CLR_DEFAULT : "FAIL";
        const char* skipped = jc_test_get_state()->is_a_tty ? JC_TEST_CLR_MAGENTA "SKIPPED" JC_TEST_CLR_DEFAULT : "SKIPPED";

        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n%s%s%s", JC_TEST_COL(YELLOW), test->name, JC_TEST_COL(DEFAULT));
        if (fixture->index != 0xFFFFFFFF) {
            cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "/%d ", fixture->index);
        }
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), " %s (", test->fail ? fail : (test->skipped ? skipped : pass));
        cursor += jc_test_snprint_time(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), stats->totaltime);
        JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), ")\n");
    } else if (event == JC_TEST_EVENT_ASSERT_FAILED) {
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n%s%s%s:%d:", JC_TEST_COL(MAGENTA), fixture->filename, JC_TEST_COL(DEFAULT), fixture->line);
        if (format) {
            va_list ap;
            va_start(ap, format);
            vsnprintf(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), format, ap);
            va_end(ap);
        }
    } else if (event == JC_TEST_EVENT_SUMMARY) {
        // print failed tests
        for( int i = 0; stats->num_fail && i < jc_test_get_state()->num_fixtures; ++i )
        {
            fixture = jc_test_get_state()->fixtures[i];
            if (!fixture || fixture->fail)
                continue;
            for (int count = 0; count < fixture->num_tests; ++count){
                test = &fixture->tests[count];
                if (test->fail) {
                    cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "%s%s%s.%s%s %sfailed%s\n", JC_TEST_COL(MAGENTA), fixture->name, JC_TEST_COL(DEFAULT), JC_TEST_COL(YELLOW), test->name, JC_TEST_COL(RED), JC_TEST_COL(DEFAULT));
                }
            }
        }
        cursor += JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "Ran %d tests, with %d assertions in ", stats->num_tests, stats->num_assertions);
        cursor += jc_test_snprint_time(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), stats->totaltime);
        if( stats->num_fail)
            JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n%d tests passed, %d skipped and %d tests %sFAILED%s\n", stats->num_pass, stats->num_skipped, stats->num_fail, JC_TEST_COL(RED), JC_TEST_COL(DEFAULT));
        else
            JC_TEST_SNPRINTF(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), "\n%d tests %sPASSED%s and %d skipped\n", stats->num_pass, JC_TEST_COL(GREEN), JC_TEST_COL(DEFAULT), stats->num_skipped);
    } else if (event == JC_TEST_EVENT_GENERIC) {
        if (format) {
            va_list ap;
            va_start(ap, format);
            vsnprintf(cursor, JC_TEST_STATIC_CAST(size_t,end-cursor), format, ap);
            va_end(ap);
        }
    }
    buffer[sizeof(buffer)-1] = 0;
    printf("%s", buffer);
}

#undef JC_TEST_COL

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

int jc_test_cmp_NEAR(double a, double b, double epsilon, const char* exprA, const char* exprB, const char* exprC) {
    double diff = a > b ? a - b : b - a;
    if (diff <= epsilon) return 1;
    char bA[64]; jc_test_print_value(bA, sizeof(bA), a);
    char bB[64]; jc_test_print_value(bB, sizeof(bB), b);
    char bEpsilon[64]; jc_test_print_value(bEpsilon, sizeof(bEpsilon), epsilon);
    char bDiff[64]; jc_test_print_value(bDiff, sizeof(bDiff), diff);
    JC_TEST_LOGF(jc_test_get_fixture(), jc_test_get_test(), 0, JC_TEST_EVENT_ASSERT_FAILED, "\nValue of: abs(%s - %s) <= %s\nExpected: abs(%s - %s) <= %s\n  Actual: abs(%s - %s) == %s\n", exprA, exprB, exprC, bA, bB, bEpsilon, bA, bB, bDiff);
    return 0;
}

int jc_test_cmp_STREQ(const char* a, const char* b, const char* exprA, const char* exprB) {
    if (jc_test_streq(a, b)) return 1;
    jc_test_log_failure_str(a, b, exprA, exprB, "==");
    return 0;
}

int jc_test_cmp_STRNE(const char* a, const char* b, const char* exprA, const char* exprB) {
    if (!jc_test_streq(a, b)) return 1;
    jc_test_log_failure_str(a, b, exprA, exprB, "!=");
    return 0;
}

// http://en.wikipedia.org/wiki/Signed_number_representations
static JC_TEST_UINT64 jc_test_float_to_biased(JC_TEST_UINT64 bits) {
    const JC_TEST_UINT64 sign_bit = JC_TEST_STATIC_CAST(JC_TEST_UINT64, 1) << (8*sizeof(double) - 1);
    return (sign_bit & bits) ? ~bits + 1 : bits | sign_bit;
}

// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition
static int jc_test_cmp_float_almost_equal(double a, double b) {
    static const int max_ulp = 4;
    union {
        double f; JC_TEST_UINT64 i;
    } ua, ub; ua.f = a; ub.f = b;
    JC_TEST_UINT64 biased_a = jc_test_float_to_biased(ua.i);
    JC_TEST_UINT64 biased_b = jc_test_float_to_biased(ub.i);
    JC_TEST_UINT64 dist_ulp = (biased_a > biased_b) ? (biased_a - biased_b) : (biased_b - biased_a);
    return dist_ulp <= max_ulp;
}

int jc_test_cmp_float_eq(double a, double b) {
    return jc_test_cmp_float_almost_equal(a, b);
}


jc_test_factory_base_interface::~jc_test_factory_base_interface() {}

jc_test_fixture::~jc_test_fixture() {}
void jc_test_fixture::SetParam() {}

jc_test_base_class::~jc_test_base_class() {}
void jc_test_base_class::SetUp() {}
void jc_test_base_class::TearDown() {}

jc_test_fixture* jc_test_create_fixture(jc_test_fixture* fixture, const char* name, unsigned int fixture_type) {
    fixture->name = name;
    fixture->type = fixture_type;
    fixture->parent = 0;
    fixture->fail = 0;
    fixture->fatal = 0;
    fixture->index = 0xFFFFFFFF;
    fixture->num_tests = 0;
    fixture->first = fixture->last = 1;
    fixture->signum = 0;
    fixture->fixture_setup = 0;
    fixture->fixture_teardown = 0;
    jc_test_memset(&fixture->stats, 0, sizeof(fixture->stats));
    jc_test_memset(fixture->tests, 0, sizeof(fixture->tests));
    JC_TEST_ASSERT_FN(jc_test_get_state()->num_fixtures < JC_TEST_MAX_NUM_FIXTURES);
    return jc_test_get_state()->fixtures[jc_test_get_state()->num_fixtures++] = fixture;
}

jc_test_entry* jc_test_add_test_to_fixture(jc_test_fixture* fixture, const char* test_name, jc_test_base_class* instance, jc_test_factory_base_interface* factory) {
    if (fixture->num_tests >= JC_TEST_MAX_NUM_TESTS_PER_FIXTURE) {
        return 0; // error
    }
    jc_test_entry* test = &fixture->tests[fixture->num_tests++];
    test->name = test_name;
    test->instance = instance;
    test->factory = factory;
    test->fail = 0;
    test->skipped = 0;
    return test;
}

jc_test_fixture* jc_test_find_fixture(const char* name, unsigned int fixture_type) {
    for( int i = 0; i < jc_test_get_state()->num_fixtures; ++i) {
        if (jc_test_get_state()->fixtures[i]->type == fixture_type && jc_test_streq(jc_test_get_state()->fixtures[i]->name, name))
            return jc_test_get_state()->fixtures[i];
    }
    return 0;
}

jc_test_fixture* jc_test_alloc_fixture(const char* name, unsigned int fixture_type) {
    return jc_test_create_fixture(new jc_test_fixture, name, fixture_type);
}

int jc_test_register_class_test(const char* fixture_name, const char* test_name,
                        jc_test_void_staticfunc class_setup, jc_test_void_staticfunc class_teardown,
                        jc_test_base_class* instance, unsigned int fixture_type) {
    jc_test_fixture* fixture = jc_test_find_fixture(fixture_name, fixture_type);
    if (!fixture) {
        fixture = jc_test_alloc_fixture(fixture_name, fixture_type);
        fixture->fixture_setup = class_setup;
        fixture->fixture_teardown = class_teardown;
    }
    jc_test_add_test_to_fixture(fixture, test_name, instance, 0);
    return 0;
}

static void jc_test_global_cleanup() {
    jc_test_state* state = jc_test_get_state();
    for (int i = 0; i < state->num_fixtures; ++i) {
        for (int j = 0; j < state->fixtures[i]->num_tests; ++j) {
            delete state->fixtures[i]->tests[j].instance;
            delete state->fixtures[i]->tests[j].factory;
        }
        delete state->fixtures[i];
    }
}

void jc_test_set_test_fail(int fatal) {
    jc_test_get_test()->fail = 1;
    jc_test_get_fixture()->fail = 1;
    jc_test_get_fixture()->fatal |= fatal;
}

void jc_test_set_test_skipped() {
    jc_test_get_test()->skipped = 1;
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

#define JC_TEST_INVOKE_MEMBER_FN(INSTANCE, FN) \
    (JC_TEST_CAST(jc_test_base_class*,INSTANCE) ->* JC_TEST_CAST(jc_test_void_memberfunc,FN)) ()

static void jc_test_run_fixture(jc_test_fixture* fixture) {
    jc_test_get_state()->current_fixture = fixture;

    if (fixture->type == JC_TEST_FIXTURE_TYPE_PARAMS_CLASS) {
        return;
    }

    jc_test_memset(&fixture->stats, 0, sizeof(fixture->stats));

    fixture->stats.totaltime = 0;
    jc_test_time_t timestart = JC_TEST_TIMING_FUNC();
    if (fixture->first) {
        JC_TEST_LOGF(fixture, 0, 0, JC_TEST_EVENT_FIXTURE_SETUP, 0);
    }

    if (fixture->first && fixture->fixture_setup != 0) {
        fixture->fixture_setup();
    }

    for (int count = 0; count < fixture->num_tests; ++count) {
        jc_test_entry* test = &fixture->tests[count];
        fixture->fail = 0;
        test->fail = 0;
        jc_test_get_state()->current_test = test;
        fixture->SetParam();

        JC_TEST_LOGF(fixture, test, 0, JC_TEST_EVENT_TEST_SETUP, 0);

        jc_test_time_t teststart = 0;
        jc_test_time_t testend = 0;

        jc_test_void_memberfunc cppfns[3] = { &jc_test_base_class::SetUp, &jc_test_base_class::TestBody, &jc_test_base_class::TearDown };

        for( int i = 0; i < 3; ++i ) {
            if( i == 1 ) {
                teststart = JC_TEST_TIMING_FUNC();
            }

            JC_TEST_INVOKE_MEMBER_FN(test->instance, cppfns[i]);

            if( i == 1 ) {
                testend = JC_TEST_TIMING_FUNC();
            }

            if( fixture->fatal ) {
                break;
            }
        }

        jc_test_stats test_stats = {0, 0, 0, 0, 0, testend-teststart};
        JC_TEST_LOGF(fixture, test, &test_stats, JC_TEST_EVENT_TEST_TEARDOWN, 0);

        if( test->fail )
            ++fixture->stats.num_fail;
        else if ( test->skipped )
            ++fixture->stats.num_skipped;
        else
            ++fixture->stats.num_pass;
        ++fixture->stats.num_tests;
    }
    jc_test_get_state()->current_test = 0;

    if (fixture->last && fixture->fixture_teardown != 0) {
        fixture->fixture_teardown();
    }

    jc_test_time_t timeend = JC_TEST_TIMING_FUNC();
    fixture->stats.totaltime = timeend - timestart;
    if (fixture->parent) {
        fixture->parent->stats.totaltime += fixture->stats.totaltime;
    }

    if (fixture->last) {
        JC_TEST_LOGF(fixture, 0, &fixture->stats, JC_TEST_EVENT_FIXTURE_TEARDOWN, 0);
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
            state->stats.num_skipped += state->fixtures[i]->stats.num_skipped;
            state->stats.num_tests += state->fixtures[i]->stats.num_tests;
            state->stats.totaltime += state->fixtures[i]->stats.totaltime;
        }
    }

    JC_TEST_LOGF(0, 0, &state->stats, JC_TEST_EVENT_SUMMARY, 0);

    int num_fail = state->stats.num_fail;
    jc_test_global_cleanup();

    return num_fail;
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
    gettimeofday(&tv, 0);
    return JC_TEST_STATIC_CAST(jc_test_time_t, tv.tv_sec) * 1000000U + JC_TEST_STATIC_CAST(jc_test_time_t, tv.tv_usec);
}

#endif

#if !defined(JC_TEST_NO_DEATH_TEST)
#if defined(__clang__) || defined(__GNUC__)
__attribute__ ((noreturn))
#endif
static void jc_test_signal_handler(int) {
    longjmp(jc_test_get_state()->jumpenv, 1);
}
#endif

static jc_test_state jc_test_global_state;

jc_test_state* jc_test_get_state() {
    return &jc_test_global_state;
}

void jc_test_init(int* argc, char** argv) {
    (void)argc; (void)argv;

#ifndef JC_TEST_NO_DEATH_TEST
    #if defined(_MSC_VER)
    signal(SIGILL, jc_test_signal_handler);
    signal(SIGABRT, jc_test_signal_handler);
    signal(SIGFPE, jc_test_signal_handler);
    signal(SIGSEGV, jc_test_signal_handler);
    #else
    #if !defined(_MSC_VER) && defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdisabled-macro-expansion"
    #endif
    struct sigaction handler;
    jc_test_memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = jc_test_signal_handler;
    sigaction(SIGILL, &handler, 0);
    sigaction(SIGABRT, &handler, 0);
    sigaction(SIGBUS, &handler, 0);
    sigaction(SIGFPE, &handler, 0);
    sigaction(SIGSEGV, &handler, 0);
    sigaction(SIGPIPE, &handler, 0);
    #if !defined(_MSC_VER) && defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
    #endif
#endif

    #if !defined(JC_TEST_NO_COLORS)
    FILE* o = stdout;
    jc_test_global_state.is_a_tty = JC_TEST_ISATTY(JC_TEST_FILENO(o));
    #endif
}

#if !defined(_MSC_VER)
#pragma GCC diagnostic pop
#endif
#endif
