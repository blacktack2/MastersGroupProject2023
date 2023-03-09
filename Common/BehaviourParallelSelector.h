#pragma once
#include "BehaviourNodeWithChildren.h"
#include <string>

class BehaviourParallelSelector : public BehaviourNodeWithChildren {
public:
	BehaviourParallelSelector(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
	~BehaviourParallelSelector() {}
	BehaviourState Execute(float dt) override {
		//std::cout << "Executing sequence " << name << "\n";
		BehaviourState state = Failure;
		for (auto& i : childNodes) {
			BehaviourState nodeState = i->Execute(dt);
			switch (nodeState) {
			case Failure: continue;
			case Ongoing:
			{
				state = Ongoing;
				continue;
			}
			case Success:
			{
				state = (state == Ongoing) ? state : Success;
				continue;
			}
			}
		}
		return state;
	}
};