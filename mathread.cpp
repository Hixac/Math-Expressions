#include <mathread.h>

#include <cassert>
#include <iterator>
#include <algorithm>
#include <math.h>

Yard::Yard(std::string expression)
{
	WhiteSpaceRemoval(expression);
		
	unsigned int i = 0;
	while (i < expression.size())
	{
		const char c = expression[i++];
		if (isdigit(c))
		{
			std::string snum = std::string{c};
			bool dot = true;
			while (isdigit(expression[i]) || (expression[i] == '.' && dot && (dot = false) == false))
				snum += expression[i++];
			tokens.push_back(snum);
		}
		else if (isalpha(c))
		{
			std::string sword = std::string{c};
			while (isalpha(expression[i]))
				sword += expression[i++];
			tokens.push_back(sword);
		}
		else tokens.push_back(std::string{c});
	}
}

float Yard::result()
{
	auto rit = std::find(tokens.rbegin(), tokens.rend(), "(");
	while (rit != tokens.rend()) {
		Yard yard;
			
		unsigned int first = std::distance(begin(tokens), rit.base()) - 1;
		auto it = std::find(tokens.begin() + first, tokens.end(), ")");
		if (it == tokens.end()) assert(false && "NO DAMN SHIT MAN FUUUUCK NAAAAH YOU FORGOT LEFT PARENTHESIS");		    
		yard.SetTokens(std::vector<std::string>(tokens.begin() + first + 1, it));
		tokens.insert(tokens.begin() + first, std::to_string(yard.result()));
		tokens.erase(tokens.begin() + first + 1, it + 2);

		rit = std::find(tokens.rbegin(), tokens.rend(), "(");
	}
		
	unsigned int index;
	float num;

	while (tokens.size() != 1)
	{
		Presedence p = FindHighestPresedence();
		num = GetVal(p);
		tokens.insert(tokens.begin() + (p.index - 1), std::to_string(num));
		tokens.erase(tokens.begin() + (p.index), tokens.begin() + (p.index + 3));
	}
		
	return num;
}

float Yard::GetVal(Presedence p)
{
	if (p.op == "^")
		return pow(p.operand1, p.operand2);
	if (p.op == "/")
		return p.operand1 / p.operand2;
	if (p.op == "*")
		return p.operand1 * p.operand2;
	if (p.op == "-")
		return p.operand1 - p.operand2;
	if (p.op == "+")
		return p.operand1 + p.operand2;

	assert(false && "Didn't found an operator!");
}

Yard::Presedence Yard::FindHighestPresedence()
{
	for (auto& op : opers)
	{
		auto it = std::find(tokens.begin(), tokens.end(), op);
		if (it != tokens.end())
		{
			unsigned int index = it - tokens.begin();
			return {index, *it, std::stof(tokens[index - 1]), std::stof(tokens[index + 1]) };
		}
	}
	assert(false && "Didn't get operation!");
}

void Yard::WhiteSpaceRemoval(std::string& s)
{	
	s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
}

bool Yard::IsNumber(const std::string& s)
{
	bool dot = true;
	return std::all_of(s.begin(), s.end(), [&](char c) {
		return (isdigit(c) || (c == '.' && dot && (dot = false) == false)); });
}

bool Yard::IsWord(const std::string& s)
{
	return std::all_of(s.begin(), s.end(), ::isalpha);
}
