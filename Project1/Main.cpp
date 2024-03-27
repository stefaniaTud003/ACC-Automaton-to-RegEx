#include <iostream>

#include "Automaton.h"
#include "RegularExpr.h"

int main()
{
	
	Automaton automaton("input.txt");
	std::cout << "Initial Automaton: " << std::endl;
	automaton.Print();
	std::cout << "--------------------------------" << std::endl;
	std::cout << "--------------------------------" << std::endl;

	RegularExpr regularExpr(automaton);
	regularExpr.AutomatonToRegularExpr();
	return 0;
}