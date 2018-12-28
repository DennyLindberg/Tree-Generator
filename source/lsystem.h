#pragma once
#include <string>
#include <map>

class LSystemString
{
public:
	std::string axiom = "";
	std::map<char, std::string> productionRules; // any symbol assigned here becomes a variable

	LSystemString() = default;
	~LSystemString() = default;

	std::string RunProduction(int iterations = 1);
};
