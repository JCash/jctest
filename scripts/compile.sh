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
    CXXFLAGS="$CXXFLAGS -Wno-zero-as-null-pointer-constant -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-suggest-override"
fi

LDFLAGS="$LDFLAGS $ASAN_LDFLAGS"



# CXX=/usr/local/opt/llvm/bin/clang++

# CXX=/Users/mathiaswesterdahl/external/llvm-build/bin/clang++
# CXXFLAGS="-ftime-trace -DJC_TEST_ASSERT_FN(_X) -DJC_TEST_NO_COLORS $CXXFLAGS"

# SYSROOT="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk"
if [ "Darwin" == "$(uname)" ]; then
    if [ -e "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk" ]; then
        SYSROOT="-isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"
    fi
    if [ "" == "${SYSROOT}" ]; then
        SYSROOT="-isysroot $(xcode-select --print-path)/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk"
    fi
fi

# Use pedantic flags when compiling jctest tests
echo "COMPILING WITH JCTEST"
PREFIX=jctest

EXAMPLE_SOURCE_DIR=./hugo/static/code

function compile_doc_example {
    local file=example_${1}.cpp
    echo "Compiling ${file}"
    $CXX -o ./build/example_${1} $OPT $ARCH $SYSROOT $CXXFLAGS ${EXAMPLE_SOURCE_DIR}/${file}
}

function compile_test {
    local name=$1
    shift
    echo "Compiling test $name"
    $CXX -o ./build/${PREFIX}_test_${name}.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS $* -c test/test_${name}.cpp
    $CXX -o ./build/${PREFIX}_main.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS -c test/main.cpp
    $CXX -o ./build/${PREFIX}_${name} $OPT $ARCH ./build/${PREFIX}_main.o ./build/${PREFIX}_test_${name}.o $LDFLAGS
}

function compile_test_with_main {
    local name=$1
    shift
    $CXX -o ./build/${PREFIX}_test_${name}.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS $* -c test/test_${name}.cpp
    $CXX -o ./build/${PREFIX}_${name} $OPT $ARCH ./build/${PREFIX}_test_${name}.o $LDFLAGS
}

time compile_test params
time compile_test typed_test
time compile_test expect -Wno-sign-compare
time compile_test death
time compile_test empty
time compile_test array
time compile_test_with_main doctest
time compile_test_with_main color_on
time compile_test_with_main color_off

time compile_doc_example minimal
time compile_doc_example custom_print
