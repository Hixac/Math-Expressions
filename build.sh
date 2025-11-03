#!/bin/sh

testing() {
    local RUN="run_tests.sh"
    local DIRECTORY="tests"

    (cd ./$DIRECTORY && ./$RUN)
}

echo "Testing before going to compile..."

testing
if [ $? -ne 0 ]; then
    echo "TESTING FAILED"
    exit 1
fi
echo "SUCCESSFULLY TESTED"
echo "Compiling program..."

g++ --std=c++23 -g -o example.out example.cpp

echo "Compiled"
