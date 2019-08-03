#!/usr/bin/env bash

set -e

EXTERNAL=~/work/external
BUILD=./build

if [ ! -e "${BUILD}" ]; then
    mkdir -p ${BUILD}
fi

echo "Compiling 'snow'"
time clang -O2 -std=c99 -o ${BUILD}/test_snow -I${EXTERNAL}/snow -DSNOW_ENABLED -Wall test_snow.c

echo "Compiling 'greatest'"
time clang -O2 -std=c89 -o ${BUILD}/test_greatest -I${EXTERNAL}/greatest test_greatest.c

echo "Compiling 'utest'"
time clang -O2 -std=c89 -o ${BUILD}/test_utest -I${EXTERNAL}/utest test_utest.c

echo "Compiling 'catch2'"
time clang++ -O2 -std=c++11 -o ${BUILD}/test_catch2 -I${EXTERNAL}/Catch2/single_include test_catch2.cpp

echo "Compiling 'doctest'"
time clang++ -O2 -std=c++11 -o ${BUILD}/test_doctest -I${EXTERNAL}/doctest/doctest test_doctest.cpp

echo "Compiling 'gtest'"
time clang++ -O2 -std=c++11 -o ${BUILD}/test_gtest -I${EXTERNAL}/googletest/googletest/include -L${EXTERNAL}/googletest/googletest -lgtest test_gtest.cpp

echo "Compiling 'jctest'"
time clang++ -O2 -std=c++98 -o ${BUILD}/test_jctest -I../../src test_jctest.cpp


find ${BUILD} -perm +111 -iname "test_*" | sort | xargs ls -la
