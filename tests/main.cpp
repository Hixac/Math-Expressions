#define MATH_READER_IMPLEMENTATION
#include <mathread.hpp>
#include <gtest/gtest.h>

using namespace math_reader;

TEST(interpret, basic_calculation) {
    double x;
    interpret("1 + 1", x);

    ASSERT_EQ(x, 2);
}
