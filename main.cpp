#include <iostream>

#define MATH_EXPR_IMPLEMENTATION
#include "include/mathread.h"

int main(void)
{
	std::string input = "1+1";
	Yard yard(input);
	
    float answer;
	yard.result(answer);
	
	std::cout << answer << std::endl;
	
	return 0;
}
