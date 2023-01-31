/**
 * @file   BehaviourInvertor.h
 * @brief  Utility to implement behaviour trees. Node which has one child. Succeeds when its child fails. Fails if its child succeeds.
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */
#pragma once
#include "BehaviourNodeWithChildren.h"
#include <string>

namespace paintHell::core {

	class BehaviourInvertor : public BehaviourNodeWithChildren {
	public:
		BehaviourInvertor(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
		~BehaviourInvertor() {}
		BehaviourState Execute(float dt) override {
			//std::cout << "Executing invertor " << mName << "\n";
			int count = 0;
			for (auto& i : childNodes) {
				BehaviourState nodeState = i->Execute(dt);
				switch (nodeState) {
				case Success: {
					mCurrentState = Failure;
					break; 
				}
					
				case Ongoing: {
					mCurrentState = nodeState;
					break;
				}
				case Failure:
				{
					mCurrentState = Success;
					break;
				}
				}
			}
			return mCurrentState;
		}
		void AddChild(BehaviourNode* n){
			if(childNodes.size()<1)
			childNodes.emplace_back(n);
		}
	};
}