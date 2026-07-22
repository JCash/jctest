#!/usr/bin/env bash

set -e
if [ ! -e build ]; then
    mkdir -p build
fi

#DISASSEMBLY="-S -masm=intel"
#PREPROCESS="-E"

if [ "$CXX" == "" ]; then
    CXX=clang++
fi

if [ "$CC" == "" ]; then
    if [ "$CXX" == "g++" ]; then
        CC=gcc
    else
        CC=clang
    fi
fi

if [ "$USE_STATICANALYZE" != "" ]; then
    if [ "$CXX" == "clang++" ]; then
        STATIC_ANALYZER_CLANG=1
        STATIC_ANALYZER_FLAGS="-Xanalyzer -analyzer-output=text -Xanalyzer -analyzer-werror -Xanalyzer -analyzer-disable-checker -Xanalyzer deadcode.DeadStores"
        echo "Using STATIC ANALYZER (clang)"
    else
        echo "USE_STATICANALYZE requires clang++ in scripts/compile.sh"
        exit 1
    fi
fi

if [ "$USE_ASAN" != "" ]; then
    if [ "$CXX" != "g++" ]; then
        SANITIZER_CXXFLAGS="$SANITIZER_CXXFLAGS -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope"
        SANITIZER_LDFLAGS="$SANITIZER_LDFLAGS -fsanitize=address"
        echo "Using ASAN"
    fi
fi

if [ "$USE_UBSAN" != "" ]; then
    if [ "$CXX" != "g++" ]; then
        SANITIZER_CXXFLAGS="$SANITIZER_CXXFLAGS -fsanitize=undefined -fno-omit-frame-pointer"
        SANITIZER_LDFLAGS="$SANITIZER_LDFLAGS -fsanitize=undefined"
        echo "Using UBSAN"
    fi
fi

if [ "$USE_MSAN" != "" ]; then
    if [ "$CXX" != "clang++" ]; then
        echo "MSAN requires clang++"
        exit 1
    fi
    SANITIZER_CXXFLAGS="$SANITIZER_CXXFLAGS -fsanitize=memory -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer"
    SANITIZER_LDFLAGS="$SANITIZER_LDFLAGS -fsanitize=memory"
    echo "Using MSAN"
fi

if [ "$OPT" == "" ]; then
    OPT="-O2"
fi
echo "Using OPT=${OPT}"

if [ "$STDVERSION" == "" ]; then
    STDVERSION=c++11
fi
echo Using -std=$STDVERSION


echo Using CXX=$CXX
$CXX --version


if [ "$ARCH" == "" ]; then
    ARCH=-m64
fi
echo Using ARCH=$ARCH

CXXFLAGS="$CXXFLAGS -std=$STDVERSION -g -Wall -pedantic -fno-exceptions -Werror=format -Isrc -I. $SANITIZER_CXXFLAGS $PREPROCESS -Wno-old-style-cast"

if [ "$CXX" == "clang++" ]; then
    CXXFLAGS="$CXXFLAGS -Weverything -Wno-global-constructors -Wuninitialized -Wsometimes-uninitialized -Wconditional-uninitialized"
fi

if [ "$CXX" == "g++" ]; then
    CXXFLAGS="$CXXFLAGS -Wuninitialized -Wmaybe-uninitialized"
fi

CXXFLAGS="$CXXFLAGS -Wno-zero-as-null-pointer-constant -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-suggest-override"

LDFLAGS="$LDFLAGS $SANITIZER_LDFLAGS"



# CXX=/usr/local/opt/llvm/bin/clang++

# CXX=/Users/mathiaswesterdahl/external/llvm-build/bin/clang++
# CXXFLAGS="-ftime-trace -DJC_TEST_ASSERT_FN(_X) -DJC_TEST_NO_COLORS $CXXFLAGS"

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
PREFIX=jctest_cpp

EXAMPLE_SOURCE_DIR=./hugo/static/code
ANALYZE_INDEX=0
ANALYZED_MAIN=0

function analyze_source {
    local file=$1
    shift
    local outfile=./build/analyze_${ANALYZE_INDEX}.o
    ANALYZE_INDEX=$((ANALYZE_INDEX + 1))
    echo "Analyzing ${file}"
    $CXX --analyze $OPT $ARCH $SYSROOT $CXXFLAGS $STATIC_ANALYZER_FLAGS $* ${file} -o ${outfile}
}

function compile_doc_example {
    local file=example_${1}.cpp
    if [ "$STATIC_ANALYZER_CLANG" != "" ]; then
        analyze_source ${EXAMPLE_SOURCE_DIR}/${file}
    fi
    echo "Compiling ${file}"
    $CXX -o ./build/example_${1} $OPT $ARCH $SYSROOT $CXXFLAGS ${EXAMPLE_SOURCE_DIR}/${file}
}

function compile_test {
    local name=$1
    shift
    if [ "$STATIC_ANALYZER_CLANG" != "" ]; then
        analyze_source test/test_${name}.cpp $*
        if [ "$ANALYZED_MAIN" == "0" ]; then
            analyze_source test/main.cpp
            ANALYZED_MAIN=1
        fi
    fi
    echo "Compiling test $name"
    $CXX -o ./build/${PREFIX}_test_${name}.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS $* -c test/test_${name}.cpp
    $CXX -o ./build/${PREFIX}_main.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS -c test/main.cpp
    $CXX -o ./build/${PREFIX}_${name} $OPT $ARCH ./build/${PREFIX}_main.o ./build/${PREFIX}_test_${name}.o $LDFLAGS
}

function compile_test_with_main {
    local name=$1
    shift
    if [ "$STATIC_ANALYZER_CLANG" != "" ]; then
        analyze_source test/test_${name}.cpp $*
    fi
    echo "Compiling test $name"
    $CXX -o ./build/${PREFIX}_test_${name}.o $OPT $DISASSEMBLY $ARCH $SYSROOT $CXXFLAGS $* -c test/test_${name}.cpp
    $CXX -o ./build/${PREFIX}_${name} $OPT $ARCH ./build/${PREFIX}_test_${name}.o $LDFLAGS
}

time compile_test params
time compile_test typed_test
time compile_test expect -Wno-sign-compare
time compile_test death
time compile_test empty
time compile_test array
time compile_test_with_main buffered_string
time compile_test_with_main reporting -Wno-sign-compare
time compile_test_with_main color_on
time compile_test_with_main color_off
time compile_test_with_main failure_recap
time compile_test_with_main failure_allocation
time compile_test_with_main filtering

time compile_doc_example minimal
time compile_doc_example custom_print
