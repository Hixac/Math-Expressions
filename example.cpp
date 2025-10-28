#define MATH_READER_IMPLEMENTATION
#include "include/mathread.hpp"

int main() {
    double res;
    math_reader::interpret("52.23 / (23333.1 * 0.001)", res);

    std::println("{}", res);
}
