#pragma once

#include <vector>
#include <array>
#include <string>

class Yard
{
	struct Presedence { unsigned int index; std::string op; float operand1; float operand2; };
public:
	Yard() = default;
	Yard(std::string expression);
	
    float result();
	float GetVal(Presedence p);

    Presedence FindHighestPresedence();

	void WhiteSpaceRemoval(std::string& s);
	bool IsNumber(const std::string& s);	
	bool IsWord(const std::string& s);
	
	void SetTokens(std::vector<std::string> tokens) { this->tokens = tokens; }
	
private:
	std::array<std::string, 5> opers { "^", "/", "*", "-", "+"};
	std::vector<std::string> tokens;
};
