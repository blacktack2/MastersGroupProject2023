/**
 * @file   StateTransition.h
 * @brief  Utility to create state machines. Base state transition
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */
#pragma once
#include<functional>

namespace paintHell::core {
	class State;
	class StateTransition;
	typedef std::function<bool(const StateTransition&)> StateTransitionFunction;
	class StateTransition {

	public:
		StateTransition(State* source, State* dest, StateTransitionFunction f) {
			sourceState = source;
			destinationState = dest;
			function = f;
		}

		bool CanTransition() const {
			return function(*this);
		}

		State* GetDestinationState()  const {
			return destinationState;
		}

		State* GetSourceState() const {
			return sourceState;
		}

	protected:
		State* sourceState;
		State* destinationState;
		StateTransitionFunction function;
	};
}


