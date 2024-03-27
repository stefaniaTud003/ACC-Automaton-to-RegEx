#include "Automaton.h"
#include <fstream>
#include <iostream>
#include <sstream>


//on the first line of the file, there are all the states separated by " "
//on the second line of the file, there are all the alphabet characters separated by " "
//on the third line of the file, there is the start state
//on the fourth line of the file, there are all the final states separated by " "
//on the next lines of the file, there are the transitions in this format: state1 transition state2
Automaton::Automaton(const std::string& filename)
{
    // Open the input file
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Read states from the first line
    std::string line;
    std::getline(inputFile, line);
    std::istringstream issStates(line);
    std::string state;
    while (issStates >> state) {
        m_states.push_back(state);
    }

    // Read alphabet characters from the second line
    std::getline(inputFile, line);
    std::istringstream issAlphabet(line);
    char symbol;
    while (issAlphabet >> symbol) {
        m_alphabet.push_back(std::string(1, symbol));
    }

    // Read the start state from the third line
    std::getline(inputFile, m_startState);

    // Read final states from the fourth line
    std::getline(inputFile, line);
    std::istringstream issFinalStates(line);
    while (issFinalStates >> state) {
        m_finalStates.push_back(state);
    }

    // Read transitions
    while (std::getline(inputFile, line)) {
        std::istringstream issTransition(line);
        std::string sourceState, transition, destinationState;
        if (issTransition >> sourceState >> transition >> destinationState) {
            m_transitions[sourceState].push_back({ transition, destinationState });
        }
    }
}

//add a transition to the automaton
//search for the source state in the transitions map
//in the list of transitions of the source state, search if the transition already exists
//if it does, return
void Automaton::AddTransition(const std::string& sourceState, const Transition newTransition)
{
	if (m_transitions.find(sourceState) != m_transitions.end())
	{
		for(const auto &transition : m_transitions[sourceState])
		{
			if (transition.transition == newTransition.transition && transition.destinationState == newTransition.destinationState)
			{
				return;
			}
		}
	}
	//print the transition added, along with a message
	std::cout << "Added transition: " << sourceState << " " << newTransition.transition << " " << newTransition.destinationState << std::endl;
	m_transitions[sourceState].push_back(newTransition);
}

//remove a transition from the automaton
//if the transition does not exist, do nothing
//we should search in the transition list of the source state and remove the transition
void Automaton::RemoveTransition(const std::string& sourceState, const Transition transitionToRemove)
{
	// Find the source state in the transitions map
	auto it = m_transitions.find(sourceState);
	if (it == m_transitions.end())
	{
		return;
	}

	// Find the transition in the list of transitions
	auto& transitions = it->second;
	for (auto it = transitions.begin(); it != transitions.end(); ++it)
	{
		if (it->transition == transitionToRemove.transition && it->destinationState == transitionToRemove.destinationState)
		{
			//print the transition removed, along with a message
			std::cout << "Removed transition: " << sourceState << " " << transitionToRemove.transition << " " << transitionToRemove.destinationState << std::endl;
			transitions.erase(it);
			return;
		}
	}

	//Check if the source state has no more transitions
	for (auto it = transitions.begin(); it != transitions.end();++it)
	{
		if (transitions.empty())
		{
			//print the source state removed, along with a message
			std::cout << "There are no more transition starting from " << sourceState <<std::endl;
			m_transitions.erase(sourceState);
			return;
		}
	}
}


//if two states are conected by multiple transitions, we should concatenate them
//if we have the transitions q0 0 -> q1 and q0 1 -> q1, we should concatenate them to q0 0|1 -> q1
void Automaton::ConcatenateTransitions()
{
	for (auto& transition : m_transitions)
	{
		std::unordered_map<std::string, std::string> concatenatedTransitions;
		for (auto& transition : transition.second)
		{
			if (concatenatedTransitions.find(transition.destinationState) == concatenatedTransitions.end())
			{

				concatenatedTransitions[transition.destinationState] = transition.transition;
			}
			else
			{
				concatenatedTransitions[transition.destinationState] += "|" + transition.transition;
				auto aux = concatenatedTransitions[transition.destinationState];
				////auto aux = concatenatedTransitions[transition.destinationState];
				//
				concatenatedTransitions[transition.destinationState].empty();
				concatenatedTransitions[transition.destinationState]= "(" + aux + ")";
			}
		}
		transition.second.clear();
		for ( auto& concatenatedTransition : concatenatedTransitions)
		{
			////if the added transition contains a "|", we should add paranthesis
			//auto pos = concatenatedTransition.second.find("|");
			//std::string aux = concatenatedTransition.second;
			//if (pos != std::string::npos)
			//{
			//	concatenatedTransition.second.empty();
			//	concatenatedTransition.second = "(" + aux + ")";
			//}
			transition.second.push_back({ concatenatedTransition.second, concatenatedTransition.first });
		}
	}
}

//remove a state from the states list
void Automaton::RemoveState(const std::string& state)
{
	//remove the state from the states list
	m_states.remove(state);
}


//if we have two or more final states, we should merge them into one
//while we have more than one final state, we remove the second one and add all its transitions to the first one
//so if i q2 and q3 as final states and the transitions q1 0 q2 and q1 1 q3, we should remove q3 and add the transition q1 1 q2
void Automaton::MergeFinalStates()
{
	//if we have only one final state, we should return
	if (m_finalStates.size() == 1)
	{
				return;
	}
	//remove the first final state from the list
	while (m_finalStates.size() > 1)
	{
		auto finalState = m_finalStates.back();
		m_finalStates.pop_back();
		//add all the transitions of the removed final state to the first final state
		for (const auto& transition : m_transitions[finalState])
		{
			AddTransition(m_finalStates.front(), transition);
			RemoveTransition(finalState, transition);
		}
	}
	
}

void Automaton::RemoveFinalState(const std::string& state)
{
	//remove the final state from the list of final states
	m_finalStates.remove(state);
}

//check if the start state is in the list of final states
//if it is, we should remove it from the list of final states and add a new state to the list of final states
//all transitions that have the start state as destination should be added to the new final state
void Automaton::CheckStartState()
{
	//if the start state is not a final state, we should return
	if (std::find(m_finalStates.begin(), m_finalStates.end(), m_startState) == m_finalStates.end())
	{
		return;
	}
	//remove the start state from the list of final states
	m_finalStates.remove(m_startState);
	//add a new state to the list of final states
	std::string newFinalState = "q" + std::to_string(m_states.size() + 1);
	m_finalStates.push_back(newFinalState);
	//add the transitions to the new final state
	//
	for (const auto& transition : m_transitions[m_startState])
	{
				AddTransition(newFinalState, transition);
		RemoveTransition(m_startState, transition);
	}
	//set the start state to the new final state
	m_startState = newFinalState;
}

//print all states between {}
//print the alphabet between {}
//on the next line we print the start state
//on the next line we print the final states between {}
//on the next lines we print the transitions in this format: state1,transition -> state2;
//if there are multiple transitions from the same state, we print them on diffrent lines
void Automaton::Print() const
{
	std::cout << "Automaton States: " << "{";
	for (const auto& state : m_states)
	{
				std::cout << state << ",";
	}
	std::cout << "}" << std::endl;

	std::cout << "Alphabet: " << "{";
	for (const auto& letter : m_alphabet)
	{
				std::cout << letter << ",";
	}
	std::cout << "}" << std::endl;

	std::cout << "Start State: " << m_startState << std::endl;

	std::cout << "Final States: " << "{";
	for (const auto& finalState : m_finalStates)
	{
				std::cout << finalState << ",";
	}
	std::cout << "}" << std::endl;

    std::cout << "Transitions: " << std::endl;
	for (const auto& transition : m_transitions)
	{
		for (const auto& state : transition.second)
		{
			std::cout << transition.first << "," << state.transition << " -> " << state.destinationState << ";" << std::endl;
		}
	}
}
