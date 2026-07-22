#!/usr/bin/env bash

set -e

if [ ! -e build ]; then
    exit 1
fi

function run_tests {
    local pattern=$1
    EXECUTABLE_PATTERN="-perm +111"
    if [ `uname` != "Darwin" ]; then
        EXECUTABLE_PATTERN="-executable"
    fi
    for name in `find build -maxdepth 1 -type f -iname "${pattern}" $EXECUTABLE_PATTERN`
    do
        echo $name
        time $name
    done
}

run_tests jctest_cpp_*
