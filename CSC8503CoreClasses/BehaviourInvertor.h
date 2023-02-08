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
			BehaviourState nodeState = childNodes[0]->Execute(dt);
			switch (nodeState) {
				case Success: {
					currentState = Failure;
					return currentState;
				}
					
				case Ongoing: {
					currentState = nodeState;
					return currentState;
				}
				case Failure:
				{
					currentState = Success;
					return currentState;
				}
			}
			
		}
		void AddChild(BehaviourNode* n){
			if(childNodes.size()<1)
			childNodes.emplace_back(n);
			else{
				childNodes.clear();
				childNodes.emplace_back(n);
			}
		}
	};
}