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
    if [ "$CXX" != "g++" ]; then
        ASAN="-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -fsanitize=undefined"
        ASAN_LDFLAGS="-fsanitize=address "
    fi
fi

if [ "$STDVERSION" == "" ]; then
    STDVERSION=c++98
fi
echo Using -std=$STDVERSION


if [ "$CXX" == "" ]; then
    CXX=clang++
fi
echo Using CXX=$CXX
$CXX --version


if [ "$ARCH" == "" ]; then
    ARCH=-m64
fi
echo Using ARCH=$ARCH

CXXFLAGS="$CXXFLAGS -std=$STDVERSION -Wall -pedantic -fno-exceptions -Werror=format -Isrc -I. $ASAN $PREPROCESS"

if [ "$CXX" == "clang++" ]; then
    CXXFLAGS="$CXXFLAGS -Weverything -Wno-global-constructors"
fi

if [ "$CXX" != "c++98" ]; then
    CXXFLAGS="$CXXFLAGS -Wno-zero-as-null-pointer-constant -Wno-c++98-compat"
fi

LDFLAGS="$LDFLAGS $ASAN_LDFLAGS"



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
    $CXX -o ./build/${PREFIX}_${name} $OPT $ARCH ./build/${PREFIX}_main.o ./build/${PREFIX}_test_${name}.o $LDFLAGS
}

time compile_test params
time compile_test typed_test
time compile_test expect
time compile_test death
time compile_test empty

if [ "$TRAVIS_COMPILER" == "" ]; then
    echo ""
    echo "COMPILING WITH GTEST"

    UNAME=`uname | tr '[:upper:]' '[:lower:]'`
    PLATFORM=x86_64-$UNAME
    GTESTDIR=./test/external/gtest
    PREFIX=gtest
    #PREPROCESS="-E"
    CXXFLAGS="-Wall -std=c++11 -Wno-deprecated-declarations -Isrc -I. -DUSE_GTEST -I${GTESTDIR}/include $PREPROCESS"
    LDFLAGS="-L${GTESTDIR}/lib/$PLATFORM -lgtest"

    if [ "$UNAME" == 'linux' ]; then
        LDFLAGS="$LDFLAGS -lpthread"
    fi

    time compile_test params
    time compile_test typed_test
    time compile_test expect
    time compile_test death
fi

$CXX -o ./build/test_example $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS docs/examples/test_example.cpp
