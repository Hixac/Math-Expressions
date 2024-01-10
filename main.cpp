#include <mathread.h>

#include <iostream>

int main(void)
{
	std::string input = "(3*(60+32)+3*(48+32)+3*(30.72+32)+3*(24.576+32)+10*(19.6608+32))/22";
	Yard yard(input);

	std::cout << yard.result() << std::endl;
	
	return 0;
}
