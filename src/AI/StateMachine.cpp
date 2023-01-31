/**
 * @file   StateMachine.cpp
 * @brief  see StateMachine.h
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"

using namespace paintHell::core;

StateMachine::StateMachine()	{
	mActiveState = nullptr;
}

StateMachine::~StateMachine()	{
	for (auto& i : mAllStates) {
		delete i;
	}
	for (auto& i : mAllTransitions) {
		delete i.second;
	}
}

void StateMachine::AddState(State* s) {
	mAllStates.emplace_back(s);
	if (mActiveState == nullptr) {
		mActiveState = s;
	}
}

void StateMachine::AddTransition(StateTransition* t) {
	mAllTransitions.insert(std::make_pair(t->GetSourceState(), t));
}

void StateMachine::Update(float dt) {
	if (mActiveState) {
		mActiveState->Update(dt);
	
		//Get the transition set starting from this state node;
		std::pair<TransitionIterator, TransitionIterator> range = mAllTransitions.equal_range(mActiveState);

		for (auto& i = range.first; i != range.second; ++i) {
			if (i->second->CanTransition()) {
				State* newState = i->second->GetDestinationState();
				mActiveState = newState;
			}
		}
	}
}