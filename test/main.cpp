#if defined(USE_GTEST)
    #include <gtest/gtest.h>
#else
    #define JC_TEST_IMPLEMENTATION
    #include <jc_test.h>
#endif

extern void GlobalTestSetup();
extern int GlobalTestTeardown();

int main(int argc, char **argv)
{
#if defined(USE_GTEST)
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
    GlobalTestSetup();
    jc_test_init(&argc, argv);
    int ret = jc_test_run_all();

    ret = GlobalTestTeardown();
    if (ret != 0) {
        printf("Test verification failed\n");
    } else {
        printf("Test verification OK\n");
    }
    return ret;
#endif
}
