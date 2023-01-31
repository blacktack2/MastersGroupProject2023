/**
 * @file   BehaviourAction.h
 * @brief  Utility to create behavior trees. Action node with function to execute
 * 
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */

#pragma once
#include "BehaviourNode.h"
#include <functional>
namespace paintHell::core {

	typedef std::function<BehaviourState(float, BehaviourState)> BehaviourActionFunc;

	class BehaviourAction : public BehaviourNode {
	public:
		BehaviourAction(const std::string& nodeName, BehaviourActionFunc f) : BehaviourNode(nodeName) {
			function = f;
		}
		BehaviourState Execute(float dt) override {
			mCurrentState = function(dt, mCurrentState);
			return mCurrentState;
		}
	protected:
		BehaviourActionFunc function;
	};
}
