#pragma once
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

struct Transition
{
	std::string transition;
	std::string destinationState;
};

class Automaton
{
	public:
		Automaton() = default;
		~Automaton() = default;
	//constructor that reads the automaton from a file
	Automaton(const std::string& filename);

	//setters
	void SetStartState(const std::string& startState) { m_startState = startState; }
	void SetFinalStates(const std::list<std::string>& finalStates) { m_finalStates = finalStates; }
	void SetStates(const std::list<std::string>& states) { m_states = states; }
	void SetAlphabet(const std::vector<std::string>& alphabet) { m_alphabet = alphabet; }
	void SetTransitions(const std::unordered_map<std::string, std::list<Transition> >&	transitions) { m_transitions = transitions; }

	//getters
	std::string GetStartState() const { return m_startState; }
	std::list<std::string> GetFinalStates() const { return m_finalStates; }
	std::list<std::string> GetStates() const { return m_states; }
	std::vector<std::string> GetAlphabet() const { return m_alphabet; }
	std::unordered_map<std::string, std::list<Transition> > GetTransitions() const { return m_transitions; }

	//modifiers
	void AddTransition(const std::string& sourceState, const Transition newTransition);
	void RemoveTransition(const std::string& sourceState, const Transition);
	void ConcatenateTransitions();
	void RemoveState(const std::string& state);
	void MergeFinalStates();
	void RemoveFinalState(const std::string& state);
	void CheckStartState();


	//method to print the automaton
	void Print() const;

private:
	std::string m_startState;
	std::list<std::string> m_finalStates;
	std::list<std::string> m_states;
	std::vector<std::string> m_alphabet;
	//<startState, Transitions>
	std::unordered_map<std::string, std::list<Transition> > m_transitions;
};

