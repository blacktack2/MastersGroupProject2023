#include "QuadTree.h"

#include "CollisionDetection.h"
#include "Debug.h"
#include "Ray.h"
#include "Vector2.h"
#include "Vector3.h"
using namespace NCL;
using namespace CSC8503;
using namespace Maths;

QuadTreeNode::QuadTreeNode(NodeStack& nodeStack) : nodeStack(nodeStack), isSplit(false) {
}

QuadTreeNode::~QuadTreeNode() {
	//PushToStack();
}

void QuadTreeNode::Init(Vector2 pos, Vector2 size) {
	isSplit = false;
	this->position = pos;
	this->size = size;
	contents.clear();
}

void QuadTreeNode::Insert(GameObject* object, const Vector2& objectPos, const Vector2& objectSize, int depthLeft, int maxSize) {
	if (!CollisionDetection::AASquareTest(objectPos, position, objectSize, size)) {
		return;
	}
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->Insert(object, objectPos, objectSize, depthLeft - 1, maxSize);
		}
	} else {
		contents.emplace_back(object, objectPos, objectSize);
		if ((int)contents.size() > 4 && depthLeft > 0) {
			Split();
			for (auto& entry : contents) {
				for (int c = 0; c < 4; c++) {
					children[c]->Insert(entry.object, entry.pos, entry.size, depthLeft - 1, maxSize);
				}
			}
			contents.clear();
		}
	}
}

void QuadTreeNode::OperateOnContents(QuadTreeFunc& func) {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->OperateOnContents(func);
		}
	} else if (!contents.empty()) {
		func(contents, position, size);
	}
}

void QuadTreeNode::OperateOnContents(QuadTreeFunc& func, const Vector2& subsetPos, const Vector2& subsetSize) {
	if (!CollisionDetection::AASquareTest(subsetPos, position, subsetSize, size)) {
		return;
	}
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->OperateOnContents(func, subsetPos, subsetSize);
		}
	} else if (!contents.empty()) {
		func(contents, position, size);
	}
}

void QuadTreeNode::OperateOnContents(QuadTreeFunc& func, const Ray& ray) {
	if (!CollisionDetection::AASquareRayTest(position, size, Vector2(ray.GetPosition().x, ray.GetPosition().z), Vector2(ray.GetDirection().x, ray.GetDirection().z))) {
		return;
	}
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->OperateOnContents(func, ray);
		}
	} else if (!contents.empty()) {
		func(contents, position, size);
	}
}

void QuadTreeNode::Split() {
	Vector2 halfSize = size * 0.5f;
	static const Vector2 offsets[] = { Vector2(-1, 1), Vector2(1, 1), Vector2(-1, -1), Vector2(1, -1) };
	for (int i = 0; i < 4; i++) {
		children[i] = nodeStack.Pop();
		children[i]->Init(position + offsets[i] * halfSize, halfSize);
	}
	isSplit = true;
}

void QuadTreeNode::Clear() {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->PushToStack();
		}
		isSplit = false;
	} else {
		contents.clear();
	}
}

void QuadTreeNode::PushToStack() {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			if (children[i])
			{
				children[i]->PushToStack();
			}
		}
	}
	nodeStack.Push(this);
}

void QuadTreeNode::DebugDraw() {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->DebugDraw();
		}
	} else {
		Debug::DrawLine(Vector3(position.x, -1000, position.y), Vector3(position.x, 1000, position.y), Vector4());
	}
}

NodeStack::NodeStack(int maxNodes) : nodes(maxNodes, *this) {
	for (auto& node : nodes) {
		Push(&node);
	}
}

void NodeStack::Push(QuadTreeNode* node) {
	stack.push(node);
}

QuadTreeNode* NodeStack::Pop() {
	if (stack.empty()) {
		return nullptr;
	} else {
		QuadTreeNode* top = stack.top();
		stack.pop();
		return top;
	}
}

QuadTree::QuadTree(Vector2 size, int maxDepth, int maxSize) : nodeStack(std::pow(4, maxDepth)), root(nodeStack) {
	root.Init(Vector2(), size);
	this->maxDepth = maxDepth;
	this->maxSize = maxSize;
}

void QuadTree::Insert(GameObject* object, const Vector2& pos, const Vector2& size) {
	root.Insert(object, pos, size, maxDepth, maxSize);
}

void QuadTree::OperateOnContents(typename QuadTreeNode::QuadTreeFunc func) {
	root.OperateOnContents(func);
}

void QuadTree::OperateOnContents(typename QuadTreeNode::QuadTreeFunc func, const Vector2& subsetPos, const Vector2& subsetSize) {
	root.OperateOnContents(func, subsetPos, subsetSize);
}

void QuadTree::OperateOnContents(typename QuadTreeNode::QuadTreeFunc func, const Ray& ray) {
	root.OperateOnContents(func, ray);
}

void QuadTree::Clear() {
	root.Clear();
}

void QuadTree::DebugDraw() {
	root.DebugDraw();
}
