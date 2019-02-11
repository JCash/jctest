#!/usr/bin/env bash

set -e

EXTERNAL=~/work/external
BUILD=./build

if [ ! -e "${BUILD}" ]; then
    mkdir -p ${BUILD}
fi

echo "Compiling 'greatest'"
time clang -std=c89 -o ${BUILD}/test_greatest -I${EXTERNAL}/greatest test_greatest.c

echo "Compiling 'utest'"
time clang -std=c89 -o ${BUILD}/test_utest -I${EXTERNAL}/utest test_utest.c

echo "Compiling 'doctest'"
time clang++ -std=c++11 -o ${BUILD}/test_doctest -I${EXTERNAL}/doctest/doctest test_doctest.cpp

echo "Compiling 'gtest'"
time clang++ -std=c++11 -o ${BUILD}/test_gtest -I${EXTERNAL}/googletest/googletest/include -L${EXTERNAL}/googletest/googletest -lgtest test_gtest.cpp

echo "Compiling 'jctest'"
time clang++ -std=c++98 -o ${BUILD}/test_jctest -I../../src test_jctest.cpp


find ${BUILD} -perm +111 -iname "test_*" | sort | xargs ls -la