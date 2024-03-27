#include "RegularExpr.h"

#include <iostream>
#include <regex>

RegularExpr::RegularExpr(const Automaton& automaton)
{
	m_regularExpr = "";
	m_automaton = automaton;
}

//the state transmitted is the source state and the end state of the transition
//we should find the transition that has the source state and the end state and star it
std::vector<std::string>  RegularExpr::StarTransition(const std::string& state)
{
	std::vector<std::string> result;
	auto transitions = m_automaton.GetTransitions();
	for (const auto& transition : transitions[state])
	{
		if (transition.destinationState == state)
		{
			result.push_back("(" + transition.transition + ")*");
		}
	}
	return result;
}


//as long as there are more than 2 states in the automaton (start and end states)
//we should remove states one by one
//removing a state means removing all transitions that have the state as source or end state
void RegularExpr::AutomatonToRegularExpr()
{
	m_automaton.ConcatenateTransitions();

	auto loopSize = m_automaton.GetFinalStates().size() + 1;
	//search for start state in final states list
	for (const auto& finalState : m_automaton.GetFinalStates())
	{
		if (finalState == m_automaton.GetStartState())
		{
			loopSize--;
		}
	}

	while (m_automaton.GetStates().size() > loopSize)
	{
		//remove the first state in the list of states that is not the start state or the end state
		auto states = m_automaton.GetStates();

		auto startState = m_automaton.GetStartState();
		auto finalStates = m_automaton.GetFinalStates();
		bool a = states.front() == startState;
		bool b = false;//= finalStates.front() == states.front();

		for (const auto& state : finalStates)
		{
			if (state == states.front())
			{
				b = true;
			}
		}

		while (a || b)
		{
			states.pop_front();
			a = states.front() == startState;
			b = false;
			for (const auto& state : finalStates)
			{
				if (state == states.front())
				{
					b = true;
				}
			}
		}
		std::cout << "Removing state: " << states.front() << std::endl;
		RemoveState(states.front());
		m_automaton.RemoveState(states.front());
		states.pop_front();
		m_automaton.ConcatenateTransitions();
	}

	//if there are more than one final state, we should remove all but the first one
	while (m_automaton.GetFinalStates().size() > 1)
	{
		auto states = m_automaton.GetStates();

		auto startState = m_automaton.GetStartState();
		auto finalStates = m_automaton.GetFinalStates();
		//if the state is the start state or not one of the final states, we should remove it
		while (states.front() == startState)
		{
			states.pop_front();
		}
		std::cout << "Removing state: " << states.front() << std::endl;
		
		//add lambda transitions from the state to be eliminated to the first final state
		auto x = m_automaton.GetTransitions()[states.front()];
		//check if the state has any transitions
		if (x.empty())
		{
			//add a lambda transition from the state to be eliminated to the first final state
			m_automaton.AddTransition(states.front(), { "λ", finalStates.front() });
		}
		for (const auto& transition : x)
		{
			for (const auto& finalState : finalStates)
			{
				if (transition.destinationState == finalState)
				{
					m_automaton.AddTransition(states.front(), { "λ", finalStates.front() });
				}
			}
		}
		m_automaton.ConcatenateTransitions();
		m_automaton.RemoveFinalState(states.front());
		RemoveState(states.front());
		m_automaton.RemoveState(states.front());
		states.pop_front();
		
	}
	auto startState = m_automaton.GetStartState();
	
	
		//star the transition that has the start state as source and then remove it
		auto starredStart = StarTransition(m_automaton.GetStartState());
	if(m_automaton.GetStates().size() > 1)
	{
		//remove the paranthesis and star from the transition
		auto pos = starredStart.front().find_first_of("(");
		auto pos2 = starredStart.front().find_last_of(")");
		auto transitionToRemove = starredStart.front().substr(pos + 1, pos2 - pos - 1);
		m_automaton.RemoveTransition(m_automaton.GetStartState(), { transitionToRemove, m_automaton.GetStartState() });

		//star the transition that has the end state as destination and then remove it
		auto starredEnd = StarTransition(m_automaton.GetFinalStates().front());
		pos = starredEnd.front().find_first_of("(");
		pos2 = starredEnd.front().find_last_of(")");
		transitionToRemove = starredEnd.front().substr(pos + 1, pos2 - pos - 1);
		m_automaton.RemoveTransition(m_automaton.GetFinalStates().front(), { transitionToRemove, m_automaton.GetFinalStates().front() });
		m_automaton.RemoveTransition(m_automaton.GetFinalStates().front(), { starredEnd.front(), m_automaton.GetFinalStates().front() });

		//concatenate the two starred transitions with each other transition from the start state to the end state
		auto transitions = m_automaton.GetTransitions();
		
		auto finalStates = m_automaton.GetFinalStates();
		for (const auto& transition : transitions[startState])
		{
			if (transition.destinationState == finalStates.front())
			{
				m_automaton.AddTransition(startState, { starredStart.front() + transition.transition + starredEnd.front(), finalStates.front() });
				//remove the transition that has the start state as source and the end state as destination
				m_automaton.RemoveTransition(startState, { transition.transition, finalStates.front() });
			}
		}

		//if there is a transition from the end state to the start state, we should concatenate it with the first transition in the transitions list and star the entire transition
		//add the transition from the end state to the start state
		//search the transition map for transitions containing the end state


		if (!m_automaton.GetTransitions()[m_automaton.GetFinalStates().front()].empty())
		{
			auto x = m_automaton.GetTransitions()[m_automaton.GetFinalStates().front()];
			for (const auto& transition : x)
			{
				if (transition.destinationState == m_automaton.GetStartState())
				{
					//x represents the transition from the end state to the start state
					auto X = m_automaton.GetTransitions()[m_automaton.GetStartState()].front().transition;
					auto Y = transition.transition;
					m_automaton.AddTransition(m_automaton.GetStartState(), { "(" + X + Y + X +
						 + ")*" ,
						 m_automaton.GetFinalStates().front() });
					//RemoveState(m_automaton.GetFinalStates().front());
					m_automaton.ConcatenateTransitions();
				}
			}

		}
	}
	else
	{
		//add starred start if it exists
		if (!starredStart.empty())
		{
			m_automaton.AddTransition(m_automaton.GetStartState(), { starredStart.front(), m_automaton.GetStartState() });
			//remove the paranthesis and star from the transition
			auto pos = starredStart.front().find_first_of("(");
			auto pos2 = starredStart.front().find_last_of(")");
			auto transitionToRemove = starredStart.front().substr(pos + 1, pos2 - pos - 1);
			m_automaton.RemoveTransition(m_automaton.GetStartState(), { transitionToRemove, m_automaton.GetStartState() });
		}
	}
	m_automaton.Print();
	m_regularExpr = m_automaton.GetTransitions()[startState].front().transition;
	//RemoveParenthesis();
	std::cout << "Regular Expression: " << m_regularExpr << std::endl;

}

//remove a state from the automaton
//if i have the transition (state1, transition, state2) and i want to remove state2
//and state2 is a start state in another transition (state2, transition2, state3)
//i should add a new transition (state1, transitiontransition2, state3)
//and remove the other two transitions
//if state2 has more than one destination state, we should add a transition for each destination state
void RegularExpr::RemoveState(const std::string& state)
{
	//all transitions that have the state as source
	std::vector<Transition> transitions = FindTransitions(state);
	std::cout << "Transitions that have " << state << " as source: " <<std::endl;
	for (const auto& transition : transitions)
	{
		std::cout << "(" << state << ", " << transition.transition << ", " << transition.destinationState << ") " << std::endl;
	}

	//all transitions that have the state as destination
	std::vector<std::pair<std::string, std::string>> transitionsWithDestination = FindTransitionsWithDestination(state);
	std::cout << "Transitions that have " << state << " as destination: " << std::endl;
	for (const auto& transition : transitionsWithDestination)
	{
		std::cout << "(" << transition.first << ", " << transition.second << ", " << state << ") " << std::endl;
	}

	//if starredTransition is not empty, when concatenating the transitions, we should star also add the starred transition
	std::vector<std::string> starredTransition = StarTransition(state);
	std::string starredTransitionStr = "";
	if (!starredTransition.empty())
	{
		//concatenate teh starred transition with a "|" between them
		for (const auto& transition : starredTransition)
		{
			//remove the paranthesis and star from the transition
			std::string::size_type pos = transition.find_first_of("(");
			std::string::size_type pos2 = transition.find_last_of(")");
			std::string transitionToRemove = transition.substr(pos + 1, pos2 - pos - 1);
			m_automaton.RemoveTransition(state, { transitionToRemove, state });
			starredTransitionStr += transition + "|";
		}
		//remove the last "|"
		std::string::size_type pos = starredTransitionStr.find_last_of("|");
		starredTransitionStr = starredTransitionStr.substr(0, pos);
		std::cout << "Starred transition: " << starredTransitionStr << std::endl;

		//remove all transitions that have the state as source and as destination
		
	}

	//add new transitions that concatenate all transitions that have the state as destination with the starred transition and all transitions that have the state as source
	for (const auto& transition : transitionsWithDestination)
	{
		for (const auto& t : transitions)
		{
			m_automaton.AddTransition(transition.first, { transition.second  + starredTransitionStr + t.transition, t.destinationState });
		}
	}

	//remove all transitions that have the state as source and as destination
	for (const auto& transition : transitions)
	{
		m_automaton.RemoveTransition(state, transition);
	}
	for (const auto& transition : transitionsWithDestination)
	{
		m_automaton.RemoveTransition(transition.first, { transition.second, state });
	}
	//print a separator
	m_automaton.Print();
	std::cout << "--------------------------------" << std::endl;
}


//find all transitions that have a specific source state
std::vector<Transition> RegularExpr::FindTransitions(const std::string& sourceState)
{
	std::vector<Transition> result;
	auto transitions = m_automaton.GetTransitions();
	for (const auto& transition : transitions[sourceState])
	{
		if(transition.destinationState != sourceState)
		result.push_back(transition);
	}
	return result;
}

//find all transitions that have a specific destination state
std::vector<std::pair<std::string, std::string>> RegularExpr::FindTransitionsWithDestination(
	const std::string& destinationState)
{
	std::vector<std::pair<std::string, std::string>> result;
	auto transitions = m_automaton.GetTransitions();
	for (const auto& transition : transitions)
	{
		for (const auto& t : transition.second)
		{
			if (t.destinationState == destinationState && destinationState != transition.first)
			{
				result.push_back({ transition.first, t.transition });
			}
		}
	}
	return result;
}

//remove unnecessary parenthesis from the regular expression
//paranthesis are unnecessary if inside them there is only one character
//the paranthesis are only necessary if after them there is a star or they contain a "|"
void RegularExpr::RemoveParenthesis()
{
	std::string newRegularExpr = "";
	for (int i = 0; i < m_regularExpr.size(); i++)
	{
		if (m_regularExpr[i] == '(' && m_regularExpr[i + 2] == ')' && m_regularExpr[i + 1] != '(' && m_regularExpr[i + 1] != ')')
		{
			newRegularExpr += m_regularExpr[i + 1];
			i += 2;
		}
		else
		{
			newRegularExpr += m_regularExpr[i];
		}
	}
	m_regularExpr = newRegularExpr;
	
}
