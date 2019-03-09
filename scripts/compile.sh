#!/usr/bin/env bash

set -e
if [ ! -e build ]; then
    mkdir -p build
fi

OPT="-g -O2"
#DISASSEMBLY="-S -masm=intel"
#PREPROCESS="-E"
#OPT="-g -O0"

if [ "$USE_ASAN" != "" ]; then
    ASAN="-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -fsanitize=undefined"
    ASAN_LDFLAGS="-fsanitize=address "
fi

if [ "$STDVERSION" == "" ]; then
    STDVERSION=c++98
fi
echo Using -std=$STDVERSION


if [ "$CXX" == "" ]; then
    CXX=clang++
fi
echo Using CXX=$CXX

CXXFLAGS="$CXXFLAGS -std=$STDVERSION -Wall -Weverything -pedantic -Wno-global-constructors -fno-exceptions -Isrc -I. $ASAN $PREPROCESS"
LDFLAGS="$ASAN_LDFLAGS"
ARCH=-m64


# CXX=/usr/local/opt/llvm/bin/clang++

# CXX=/Users/mathiaswesterdahl/external/llvm-build/bin/clang++
# CXXFLAGS="-ftime-trace -DJC_TEST_ASSERT_FN(_X) -DJC_TEST_NO_COLORS $CXXFLAGS"

# SYSROOT="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk"

# Use pedantic flags when compiling jctest tests
echo "COMPILING WITH JCTEST"
PREFIX=jctest

function compile_test {
    local name=$1
    $CXX -o ./build/${PREFIX}_test_${name}.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS -c test/test_${name}.cpp
    $CXX -o ./build/${PREFIX}_main.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS -c test/main.cpp
    $CXX -o ./build/${PREFIX}_${name} $OPT $ARCH $LDFLAGS ./build/${PREFIX}_main.o ./build/${PREFIX}_test_${name}.o
}

time compile_test params
time compile_test typed_test
time compile_test expect
time compile_test death

if [ "Darwin" == `uname` ]; then
    echo ""
    echo "COMPILING WITH GTEST"

    PLATFORM=x86_64-darwin
    GTESTDIR=./test/external/gtest
    PREFIX=gtest
    CXXFLAGS="-Wall -std=c++11 -Wno-deprecated-declarations -Isrc -I. -DUSE_GTEST -I${GTESTDIR}/include"
    LDFLAGS="-L${GTESTDIR}/lib/$PLATFORM -lgtest"

    time compile_test params
    time compile_test typed_test
    time compile_test expect
    time compile_test death
fi

$CXX -o ./build/test_example $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS docs/examples/test_example.cpp
