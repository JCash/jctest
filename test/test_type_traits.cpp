#define JC_TEST_USE_COLORS 1
#define JC_TEST_USE_DEFAULT_MAIN
#include <jc_test.h>

#if __cplusplus > 199711L
#include <type_traits>
#endif

typedef enum {
    A,
    B,
    C
} enum_t;

typedef void *function_t(int, double, char *);

TEST(TypesTraits, Integral_t)
{
#if __cplusplus > 199711L
    ASSERT_EQ(true, jc::is_integral<long long>::value);
    ASSERT_EQ(std::is_integral<long long>::value, jc::is_integral<long long>::value);

    ASSERT_EQ(true, jc::is_integral<unsigned long long>::value);
    ASSERT_EQ(std::is_integral<unsigned long long>::value, jc::is_integral<unsigned long long>::value);
#endif
}

TEST(TypesTraits, Ptr_t)
{
    ASSERT_EQ(false, jc::is_pointer<function_t>::value);
#if __cplusplus > 199711L
    ASSERT_EQ(std::is_pointer<function_t>::value, jc::is_pointer<function_t>::value);
#endif

    ASSERT_EQ(true, jc::is_pointer<void *>::value);
#if __cplusplus > 199711L
    ASSERT_EQ(std::is_pointer<void *>::value, jc::is_pointer<void *>::value);
#endif
}

TEST(TypesTraits, Function_t)
{
    ASSERT_EQ(true, jc::is_function<function_t>::value);
#if __cplusplus > 199711L
    ASSERT_EQ(std::is_function<function_t>::value, jc::is_function<function_t>::value);
#endif
}

TEST(TypesTraits, Enum_t)
{
    ASSERT_EQ(true, jc::is_enum<enum_t>::value);
#if __cplusplus > 199711L
    ASSERT_EQ(std::is_enum<enum_t>::value, jc::is_enum<enum_t>::value);
    ASSERT_EQ(std::is_enum<std::nullptr_t>::value, jc::is_enum<std::nullptr_t>::value);
#endif
}

//////////////////////////////////////////////
