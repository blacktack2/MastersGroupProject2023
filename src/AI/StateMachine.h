/**
 * @file   StateMachine.h
 * @brief  Utility to create state machines. Base state machine
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */

#pragma once
#include<map>
#include<vector>

namespace paintHell::core {
	class State;
	class StateTransition;

	typedef std::multimap<State*, StateTransition*> TransitionContainer;
	typedef TransitionContainer::iterator TransitionIterator;

	class StateMachine {
	public:
		StateMachine();
		virtual ~StateMachine(); //made it virtual!

		void AddState(State* s);
		void AddTransition(StateTransition* t);

		virtual void Update(float dt); //made it virtual!

	protected:
		State* mActiveState;

		std::vector<State*> mAllStates;
		TransitionContainer mAllTransitions;
	};
}
