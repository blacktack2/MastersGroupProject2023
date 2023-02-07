/**
 * @file   BehaviourParallel.h
 * @brief  Utility to implement behaviour trees. Nodes which execute all its children in parallel. Succeeds when all its children succeed. Fails if any one of its children fail.
 * 
 * @author Daniel Thomas Abraham
 * @date   February 2023
 */
#pragma once
#include "BehaviourNodeWithChildren.h"
#include <string>


class BehaviourParallel : public BehaviourNodeWithChildren {
public:
	BehaviourParallel(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
	~BehaviourParallel() {}
	BehaviourState Execute(float dt) override {
		//std::cout << "Executing parallel " << mName << "\n";
		unsigned int count = 0;
		for (auto& i : childNodes) {
			BehaviourState nodeState = i->Execute(dt);
			switch (nodeState) {
			case Success:count++;
			case Ongoing:continue;
			case Failure: 
			{
				currentState = nodeState;
				return currentState;
			}
			}
		}
		if (count == childNodes.size()) return Success;
		return Ongoing;
	}
};
