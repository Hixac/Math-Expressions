#pragma once

#include <cfloat>
#include <cassert>

#include <algorithm>

#include <climits>
#include <vector>
#include <array>
#include <string>

class Yard
{
	struct Presedence { unsigned int index; std::string op; float operand1; float operand2; };

	using ERROR_RESULT = int;
	
public:
	Yard() = default;
	Yard(std::string expression, int x = INT_MAX);
	
    ERROR_RESULT result(float& answer);
	float GetVal(Presedence p);

    Presedence ClearFunction(unsigned int index);
    Presedence FindHighestPresedence();

	void WhiteSpaceRemoval(std::string& s);

	bool IsNumber(const std::string& s);	
	bool IsWord(const std::string& s);
	bool IsOper(const std::string& s);
	bool IsFunction(const std::string& s);
	
	void SetTokens(std::vector<std::string> tokens) { this->tokens = tokens; }
	
private:
	std::array<std::string, 18> opers {"abs", "cos", "sin", "tg", "arccos", "arcsin", "arctg", "ch", "sh", "th", "arch", "arsh", "arth", "^", "/", "*", "-", "+"};
	std::vector<std::string> tokens;
};

#if defined(MATH_EXPR_IMPLEMENTATION)

Yard::Yard(std::string expression, int x)
{
	auto check = [](char c) { return (static_cast<unsigned char>(c) > 127); };
	for (char c : expression)
		if (check(c)) return;

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
		else if (c == 'x')
		{
			tokens.push_back(std::to_string(x));
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

int Yard::result(float& answer)
{
	if (tokens.size() == 0 || tokens.size() == 2) return -2;
	for (int i = 1; i < tokens.size()-1; i++)
	{
		auto& tok = tokens[i];
		auto& ltok = tokens[i - 1];
		auto& rtok = tokens[i + 1];
		if (IsOper(tok))
			if ((!IsNumber(ltok) || !IsNumber(rtok)) && (!IsFunction(rtok) && ltok != ")")) return -10;
		if (IsFunction(rtok) && i == tokens.size()-2)
			return -12;
	}
	int t1 = 0, t2 = 0;
	for (auto& tok : tokens)
	{
	    if (!IsFunction(tok) && IsWord(tok)) return -11;
		if (tok == "(")
			t1++;
		if (tok == ")")
			t2++;
	}
	if (t1 != t2) return -7;
	
	auto rit = std::find(tokens.rbegin(), tokens.rend(), "(");
	while (rit != tokens.rend())
	{
		Yard yard;
			
		unsigned int first = std::distance(begin(tokens), rit.base()) - 1;
		auto it = std::find(tokens.begin() + first, tokens.end(), ")");
		unsigned int second = it - tokens.begin();
		if (it == tokens.end()) return -1;
		if (first >= 1 && IsFunction(tokens[first - 1]))
		{
			yard.SetTokens(std::vector<std::string>(tokens.begin() + first + 1, it));
			int err; if ((err = yard.result(answer)) < 0) return err;
			tokens.insert(tokens.begin() + first + 1, std::to_string(answer));
			tokens.erase(tokens.begin() + first + 2, tokens.begin() + second + 1);
			
			Presedence p = ClearFunction(first - 1);
			if (p.op == "0") return -8;
			
			answer = GetVal(p);
			tokens.insert(tokens.begin() + p.index, std::to_string(answer));
			tokens.erase(tokens.begin() + p.index + 1, tokens.begin() + (p.index + 5));
		}
		else
		{
			yard.SetTokens(std::vector<std::string>(tokens.begin() + first + 1, it));
			int err; if ((err = yard.result(answer)) < 0) return err;
			tokens.insert(tokens.begin() + first, std::to_string(answer));
			tokens.erase(tokens.begin() + first + 1, tokens.begin() + second + 2);			
		}

		rit = std::find(tokens.rbegin(), tokens.rend(), "(");
	}
	
	if (tokens.size() == 1 and IsNumber(tokens[0]))
	{
		answer = std::stof(tokens[0]);
		return 0;
	}
	
	while (tokens.size() != 1)
	{
		Presedence p = FindHighestPresedence();
		if (p.op == "0") return -6;
		
	    answer = GetVal(p);
		tokens.insert(tokens.begin() + p.index - 1, std::to_string(answer));
		tokens.erase(tokens.begin() + p.index, tokens.begin() + (p.index + 3));
	}

	return 0;
}

float Yard::GetVal(Presedence p)
{
	if (p.op == "abs")
		return abs(p.operand1);
	if (p.op == "sin")
		return sinf(p.operand1);
	if (p.op == "cos")
		return cosf(p.operand1);
	if (p.op == "tg")
		return tanf(p.operand1);
	if (p.op == "arccos")
		return acosf(p.operand1);
	if (p.op == "arcsin")
		return asinf(p.operand1);
	if (p.op == "arctg")
		return atanf(p.operand1);
	if (p.op == "ch")
		return coshf(p.operand1);
	if (p.op == "sh")
		return sinhf(p.operand1);
	if (p.op == "th")
		return tanhf(p.operand1);
	if (p.op == "arsh")
		return asinhf(p.operand1);
	if (p.op == "arch")
		return acoshf(p.operand1);
	if (p.op == "arth")
		return atanhf(p.operand1);
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

Yard::Presedence Yard::ClearFunction(unsigned int index)
{
	// Поиск функций (ПИЗДЕЦ Я ЗАЕБАЛСЯ)
	for (int i = 0; i < opers.size() - 5; i++)
	{
		auto it = std::find(tokens.begin() + index, tokens.end(), opers[i]);
		if (it != tokens.end())
		{ // из-за злоебучего "стоф" мне приходится пригибаться и впендюривать трай-кэч, эта хуйня жрёт неоправданно много ресов, но я говокодер)))) нэ пэрэжэвайти
			try {
				unsigned int index = it - tokens.begin();
				return {index, *it, std::stof(tokens[index + 2])};
			}
			catch (...) {
				// Error check
			}
		}
	}
	return {0, "0", 0, FLT_MAX};
}

// Поиск токенам по приоритету и получение их операнд
Yard::Presedence Yard::FindHighestPresedence()
{
	for (int i = 2; i < opers.size(); i++)
	{
		auto it = std::find(tokens.begin(), tokens.end(), opers[i]);
		if (it != tokens.end())
		{
			try {
				unsigned int index = it - tokens.begin();
				return {index, *it, std::stof(tokens[index - 1]), std::stof(tokens[index + 1]) };				
			}
			catch (...) {
				// Error check
			}
		}
	}
    return {0, "0", 0, FLT_MAX};
}

void Yard::WhiteSpaceRemoval(std::string& s)
{
	s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
}

bool Yard::IsNumber(const std::string& s)
{
	bool dot = true;
	std::string temp = s;
	if (s[0] == '-') temp.erase(temp.begin());
	return std::all_of(temp.begin(), temp.end(), [&](char c) {
		return (isdigit(c) || (c == '.' && dot && (dot = false) == false)); });
}

bool Yard::IsWord(const std::string& s)
{
	return std::all_of(s.begin(), s.end(), ::isalpha);
}

bool Yard::IsOper(const std::string& s)
{
	for (int i = opers.size() - 5; i < opers.size(); i++)
	{
		if (s == opers[i])
			return true;
	}
	return false;
}

bool Yard::IsFunction(const std::string& s)
{
	for (int i = 0; i < opers.size() - 5; i++)
	{
		if (s == opers[i])
			return true;
	}
	return false;
}

#endif
