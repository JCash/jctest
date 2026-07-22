#!/usr/bin/env bash

set -e

if [ ! -e build ]; then
    mkdir -p build
fi

if [ "$CC" == "" ]; then
    CC=clang
fi
if [ "$CSTDVERSION" == "" ]; then
    CSTDVERSION=c11
fi
if [ "$OPT" == "" ]; then
    OPT=-O2
fi
if [ "$ARCH" == "" ]; then
    ARCH=-m64
fi

echo "COMPILING C TESTS"
echo "Using CC=$CC"
echo "Using ARCH=$ARCH"
$CC --version

CSTD_FLAG=$CSTDVERSION
if [ "$CSTDVERSION" == "c23" ] && \
   ! $CC -std=c23 -E -x c /dev/null -o /dev/null 2>/dev/null; then
    CSTD_FLAG=c2x
fi
echo "Using -std=$CSTD_FLAG"

C_TEST_FLAGS="$CFLAGS -std=$CSTD_FLAG -g -Wall -Wextra -pedantic -Werror=format -Isrc -I. $SANITIZER_CFLAGS"

$CC -o ./build/jctest_c $OPT $ARCH $SYSROOT $C_TEST_FLAGS \
    test/c/main.c test/c/test_assertions.c test/c/test_fixture.c $LDFLAGS
$CC -o ./build/jctest_c_reporting $OPT $ARCH $SYSROOT $C_TEST_FLAGS \
    test/c/test_reporting.c $LDFLAGS
$CC -o ./build/jctest_c_failure_allocation $OPT $ARCH $SYSROOT $C_TEST_FLAGS \
    test/c/test_failure_allocation.c $LDFLAGS
$CC -o ./build/jctest_c_default_main $OPT $ARCH $SYSROOT $C_TEST_FLAGS \
    test/c/test_default_main.c $LDFLAGS
$CC -o ./build/jctest_c_filtering $OPT $ARCH $SYSROOT $C_TEST_FLAGS \
    test/c/test_filtering.c $LDFLAGS
$CC -o ./build/jctest_c_color $OPT $ARCH $SYSROOT $C_TEST_FLAGS \
    test/c/test_color.c $LDFLAGS

if $CC -c $OPT $ARCH $SYSROOT $C_TEST_FLAGS test/c/compile_fail_ordered_pointer.c -o ./build/jctest_c_compile_fail.o 2>/dev/null; then
    echo "Expected ordered pointer assertion compilation to fail"
    exit 1
fi
if $CC -c $OPT $ARCH $SYSROOT $C_TEST_FLAGS test/c/compile_fail_struct.c -o ./build/jctest_c_compile_fail.o 2>/dev/null; then
    echo "Expected struct assertion compilation to fail"
    exit 1
fi
