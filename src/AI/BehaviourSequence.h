/**
 * @file   BehaviourSequence.h
 * @brief  Utility to create behavior trees. Executes child nodes in sequence. Fails if all its children fail. Succeeds when all its children succeed.
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */

#pragma once
#include "BehaviourNodeWithChildren.h"
namespace paintHell::core {
	class BehaviourSequence : public BehaviourNodeWithChildren {
	public:
		BehaviourSequence(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
		~BehaviourSequence() {}
		BehaviourState Execute(float dt) override {
			//std::cout << "Executing sequence " << mName << "\n";
			for (auto& i : childNodes) {
				BehaviourState nodeState = i->Execute(dt);
				switch (nodeState) {
				case Success: continue;
				case Failure:
				case Ongoing:
				{
					mCurrentState = nodeState;
					return nodeState;
				}
				}
			}
			return Success;
		}
	};

}