#include "lsystem.h"

std::string LSystemString::RunProduction(int iterations)
{
	std::string production = axiom;

	while (--iterations >= 0)
	{
		std::string newString;
		for (char c : production)
		{
			if (productionRules.find(c) != productionRules.end())
			{
				newString.append(productionRules[c]);
			}
			else
			{
				newString.append(1, c);
			}
		}
		production = newString;
	}

	return production;
}