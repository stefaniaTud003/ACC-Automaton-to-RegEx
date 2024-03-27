#pragma once
#include "Automaton.h"

class RegularExpr
{
public:
	RegularExpr() = default;
	~RegularExpr() = default;

	//constructor that recieves an automaton
	RegularExpr(const Automaton& automaton);

	//method to star a transition
	std::vector<std::string> StarTransition(const std::string& state);

	//method to turn the automaton into a regular expression
	void AutomatonToRegularExpr();

	//method to remove a state from the automaton
	void RemoveState(const std::string& state);

	//method to find all transitions (transition, destinationState) that have a specific source state
	std::vector<Transition> FindTransitions(const std::string& sourceState);

	//method to find all transitions (startState, transition) that have a specific destination state
	std::vector<std::pair<std::string, std::string>> FindTransitionsWithDestination(const std::string& destinationState);

	std::string RemoveExtraClosingParentheses(const std::string& string);
	//method to remove unnecessary parenthesis from the regular expression
	void RemoveParenthesis();

private:
	Automaton m_automaton;
	std::string m_regularExpr;
};

