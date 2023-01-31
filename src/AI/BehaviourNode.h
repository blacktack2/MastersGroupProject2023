/**
 * @file   BehaviourNode.h
 * @brief  Utility to create behavior trees. Base BehaviourNode
 * 
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */
#pragma once
#include <string>

namespace paintHell::core {
	enum BehaviourState {
		Initialise,
		Failure,
		Success,
		Ongoing
	};

	class BehaviourNode {
	public:
		BehaviourNode(const std::string& nodeName) {
			mCurrentState = Initialise;
			mName = nodeName;
		}
		virtual ~BehaviourNode() {}
		virtual BehaviourState Execute(float dt) = 0;

		//BehaviourState	GetState() const {return currentState; }
		virtual void	Reset() { mCurrentState = Initialise; }
	protected:
		BehaviourState	mCurrentState;
		std::string		mName;
	};
}
