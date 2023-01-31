/**
 * @file   State.h
 * @brief  Utility to create state machines. Base state
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */
#pragma once
#include<functional>


namespace paintHell::core {
	typedef std::function<void(float)> StateUpdateFunction;

	class  State {
	public:
		State() {}
		State(StateUpdateFunction someFunc) {
			func = someFunc;
		}
		void Update(float dt) {
			if (func != nullptr) {
				func(dt);
			}
		}

		int GetStateNum() const { return stateNum; }
		void SetStateNum(int val) { stateNum = val; }

	protected:
		StateUpdateFunction func;
		int stateNum = 0;
	};
}

