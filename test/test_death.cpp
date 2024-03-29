

#include <string.h>
#include <stdio.h>
#include "testutil.h"

#include <assert.h>
#include <signal.h>

#if !defined(_MSC_VER)
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
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

TEST(DeathTest, Test1)
{
    EXPECT_DEATH(raise(SIGSEGV),"");
    EXPECT_DEATH(raise(SIGFPE),"");
    EXPECT_DEATH(raise(SIGABRT),"");
    EXPECT_DEATH(raise(SIGILL),"");
    EXPECT_DEATH(call_abort(false),"");
    EXPECT_TRUE(true);
}
