/**
 * @file   BehaviourSelector.h
 * @brief  Utility to create behavior trees. Executes children in sequence. Fails only if all its children fail. Succeeds even if one of its children succeed.
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */

#pragma once
#include "BehaviourNodeWithChildren.h"

namespace paintHell::core {
	class BehaviourSelector : public BehaviourNodeWithChildren {
	public:
		BehaviourSelector(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
		~BehaviourSelector() {}
		BehaviourState Execute(float dt) override {
			//std::cout << "Executing selector " << mName << "\n";
			for (auto& i : childNodes) {
				BehaviourState nodeState = i->Execute(dt);
				switch (nodeState) {
				case Failure: continue;
				case Success:
				case Ongoing:
				{
					mCurrentState = nodeState;
					return mCurrentState;
				}
				}
			}
			return Failure;
		}
	};
}
