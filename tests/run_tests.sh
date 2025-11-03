#!/bin/sh

readonly DIR_MATH_EXPR_HEADEARS=../include
readonly DIR_GTEST_HEADERS=./gtest/include

echo "Compiling tests..."

g++ \
    --std=c++23 \
    -L./gtest/ -lgtest_main -lgtest \
    -I$DIR_MATH_EXPR_HEADEARS -I$DIR_GTEST_HEADERS \
    main.cpp -o tests.out

./tests.out
if [ $? -ne 0 ]; then
    exit 1
fi
