#!/usr/bin/env bash

set -e

if [ ! -e build ]; then
    exit 1
fi

function run_tests {
    local pattern=$1
    for name in `find build -iname "${pattern}" -perm +111`
    do
        echo $name
        time $name
    done
}

#run_tests gtest_*
run_tests jctest_*