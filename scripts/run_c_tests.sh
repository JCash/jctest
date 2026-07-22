#!/usr/bin/env bash

set -e

if [ ! -e build ]; then
    echo "Missing build directory; run scripts/compile.sh first"
    exit 1
fi

EXECUTABLE_PATTERN="-perm +111"
if [ "$(uname)" != "Darwin" ]; then
    EXECUTABLE_PATTERN="-executable"
fi

TESTS=$(find build -maxdepth 1 -type f \
    \( -name "jctest_c" -o -name "jctest_c_*" \) $EXECUTABLE_PATTERN)
if [ -z "$TESTS" ]; then
    echo "No C test executables found; run scripts/compile.sh first"
    exit 1
fi

for name in $TESTS
do
    echo "$name"
    time "$name"
done
