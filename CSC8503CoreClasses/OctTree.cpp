#include "OctTree.h"

#include "CollisionDetection.h"
#include "Debug.h"
#include "Ray.h"
#include "Vector2.h"
#include "Vector3.h"
using namespace NCL;
using namespace CSC8503;
using namespace Maths;

OctTreeNode::OctTreeNode(NodeStack& nodeStack) : nodeStack(nodeStack), isSplit(false) {
}

OctTreeNode::~OctTreeNode() {
	//PushToStack();
}

void OctTreeNode::Init(Vector3 pos, Vector3 size) {
	isSplit = false;
	this->position = pos;
	this->size = size;
	contents.clear();
}

void OctTreeNode::Insert(GameObject* object, const Vector3& objectPos, const Vector3& objectSize, int depthLeft, int maxSize) {
	if (!CollisionDetection::AABBTest(position, size, objectPos, objectSize)) {
		return;
	}

	if (isSplit) {
		for (int i = 0; i < 8; i++) {
			children[i]->Insert(object, objectPos, objectSize, depthLeft - 1, maxSize);
		}
	}
	else {
		contents.emplace_back(object, objectPos, objectSize);
		if ((int)contents.size() > maxSize && depthLeft > 0) {
			Split();
			for (auto& entry : contents) {
				for (int c = 0; c < 8; c++) {
					children[c]->Insert(entry.object, entry.pos, entry.size, depthLeft - 1, maxSize);
				}
			}
			contents.clear();
		}
	}
}

void OctTreeNode::OperateOnContents(OctTreeFunc& func) {
	if (isSplit) {
		for (int i = 0; i < 8; i++) {
			children[i]->OperateOnContents(func);
		}
	}
	else if (!contents.empty()) {
		func(contents, position, size);
	}
}

void OctTreeNode::OperateOnContents(OctTreeFunc& func, const Vector3& subsetPos, const Vector3& subsetSize) {
	if (!CollisionDetection::AABBTest(subsetPos, position, subsetSize, size)) {
		return;
	}
	if (isSplit) {
		for (int i = 0; i < 8; i++) {
			children[i]->OperateOnContents(func, subsetPos, subsetSize);
		}
	} else if (!contents.empty()) {
		func(contents, position, size);
	}
}

void OctTreeNode::OperateOnContents(OctTreeFunc& func, const Ray& ray) {
    if (!CollisionDetection::AABBRayTest(position, size, ray)) {
        return;
    }

    if (isSplit) {
        for (int i = 0; i < 8; i++) {
            children[i]->OperateOnContents(func, ray);
        }
    } else if (!contents.empty()) {
        func(contents, position, size);
    }
}

void OctTreeNode::Split() {
	Vector3 halfSize = size * 0.5f;
	static const Vector3 offsets[] = {
		Vector3(-1, 1, 1), Vector3(1, 1, 1), Vector3(-1, -1, 1), Vector3(1, -1, 1),
		Vector3(-1, 1, -1), Vector3(1, 1, -1), Vector3(-1, -1, -1), Vector3(1, -1, -1)
	};
	for (int i = 0; i < 8; i++) {
		children[i] = nodeStack.Pop();
		children[i]->Init(position + offsets[i] * halfSize, halfSize);
	}
	isSplit = true;
}

void OctTreeNode::Clear() {
	if (isSplit) {
		for (int i = 0; i < 8; i++) {
			children[i]->Clear();
			children[i]->PushToStack();
			children[i] = nullptr;
		}
		isSplit = false;
	}
	else {
		contents.clear();
	}
}

void OctTreeNode::PushToStack() {
	if (isSplit) {
		for (int i = 0; i < 8; i++) {
			if (children[i]) {
				children[i]->PushToStack();
			}
		}
	}
	nodeStack.Push(this);
}


void OctTreeNode::DebugDraw() {
	if (isSplit) {
		for (int i = 0; i < 8; i++) {
			children[i]->DebugDraw();
		}
	}
	else {
		Debug::DrawLine(Vector3(position.x, position.y - 1000, position.z), Vector3(position.x, position.y + 1000, position.z), Vector4());
		Debug::DrawLine(Vector3(position.x - 1000, position.y, position.z), Vector3(position.x + 1000, position.y, position.z), Vector4());
		Debug::DrawLine(Vector3(position.x, position.y, position.z - 1000), Vector3(position.x, position.y, position.z + 1000), Vector4());
	}
}

NodeStack::NodeStack(int maxNodes) : nodes(maxNodes, *this) {
	for (auto& node : nodes) {
		Push(&node);
	}
}

void NodeStack::Push(OctTreeNode* node) {
	stack.push(node);
}

OctTreeNode* NodeStack::Pop() {
	if (stack.empty()) {
		return nullptr;
	} else {
		OctTreeNode* top = stack.top();
		stack.pop();
		return top;
	}
}

OctTree::OctTree(Vector3 size, int maxDepth, int maxSize) : nodeStack(std::pow(8, maxDepth)), root(nodeStack) {
	root.Init(Vector3(), size);
	this->maxDepth = maxDepth;
	this->maxSize = maxSize;
}

void OctTree::Insert(GameObject* object, const Vector3& pos, const Vector3& size) {
	root.Insert(object, pos, size, maxDepth, maxSize);
}

void OctTree::OperateOnContents(typename OctTreeNode::OctTreeFunc func) {
	root.OperateOnContents(func);
}

void OctTree::OperateOnContents(typename OctTreeNode::OctTreeFunc func, const Vector3& subsetPos, const Vector3& subsetSize) {
	root.OperateOnContents(func, subsetPos, subsetSize);
}

void OctTree::OperateOnContents(typename OctTreeNode::OctTreeFunc func, const Ray& ray) {
	root.OperateOnContents(func, ray);
}

void OctTree::Clear() {
	root.Clear();
}

void OctTree::DebugDraw() {
	root.DebugDraw();
}
