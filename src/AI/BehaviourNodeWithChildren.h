/**
 * @file   BehaviourNodewithChildren.h
 * @brief  Utility to create behavior trees. Base BehaviourNode with children
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */
#pragma once
#include "BehaviourNode.h"
#include<string>
#include<vector>

namespace paintHell::core {
	class BehaviourNodeWithChildren : public BehaviourNode {
	public:
		BehaviourNodeWithChildren(const std::string& nodeName) : BehaviourNode(nodeName) {};
		~BehaviourNodeWithChildren() {
			for (auto& i : childNodes) {
				delete i;
			}
		}
		void AddChild(BehaviourNode* n) {
			childNodes.emplace_back(n);
		}

		void Reset() override {
			mCurrentState = Initialise;
			for (auto& i : childNodes) {
				i->Reset();
			}
		}
	protected:
		std::vector<BehaviourNode*> childNodes;
	};
}
