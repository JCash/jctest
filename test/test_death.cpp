
#include <string.h>
#include <stdio.h>
#include "testutil.h"

#include <assert.h>

#if __cplusplus >= 199711L
    #define NULLPTR nullptr
    #pragma GCC diagnostic ignored "-Wc++98-compat"
#else
    #define NULLPTR 0
#endif

// To test the testing framework /////////////
void GlobalTestSetup(); // to silence a warning
void GlobalTestSetup() {
    printf("Setting up calls\n");
}
int GlobalTestTeardown(); // to silence a warning
int GlobalTestTeardown() {
    printf("Verifying the calls\n");
    return 0;
}
//////////////////////////////////////////////

// ASSERT_NO_FATAL_FAILURE

static void call_abort(bool v) {
    assert(v && "call_assert called with false");
}

static void call_segv(int* paddr) {
    *paddr = 0;
}

TEST(DeathTest, Test1)
{
    EXPECT_DEATH_IF_SUPPORTED(call_segv(NULLPTR),"");
    EXPECT_DEATH_IF_SUPPORTED(call_abort(false),"");
    EXPECT_TRUE(true);
}

