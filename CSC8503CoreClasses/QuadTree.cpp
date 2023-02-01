#include "QuadTree.h"

#include "CollisionDetection.h"
#include "Debug.h"
#include "Vector2.h"
#include "Vector3.h"
using namespace NCL;
using namespace CSC8503;

template<class T>
QuadTreeNode<T>::QuadTreeNode(NodeStack<T>& nodeStack) : nodeStack(nodeStack), isSplit(false) {
}

template<class T>
QuadTreeNode<T>::~QuadTreeNode() {
	PushToStack();
}

template<class T>
void QuadTreeNode<T>::Init(Vector2 pos, Vector2 size) {
	isSplit = false;
	this->position = pos;
	this->size = size;
	contents.clear();
}

template<class T>
void QuadTreeNode<T>::Insert(T& object, const Vector2& objectPos, const Vector2& objectSize, int depthLeft, int maxSize) {
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

template<class T>
void QuadTreeNode<T>::OperateOnContents(QuadTreeFunc& func) {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->OperateOnContents(func);
		}
	} else if (!contents.empty()) {
		func(contents, position, size);
	}
}

template<class T>
void QuadTreeNode<T>::OperateOnContents(QuadTreeFunc& func, const Vector2& subsetPos, const Vector2& subsetSize) {
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

template<class T>
void QuadTreeNode<T>::Split() {
	Vector2 halfSize = size * 0.5f;
	static const Vector2 offsets[] = { Vector2(-1, 1), Vector2(1, 1), Vector2(-1, -1), Vector2(1, -1) };
	for (int i = 0; i < 4; i++) {
		children[i] = nodeStack.Pop();
		children[i]->Init(position + offsets[i] * halfSize, halfSize);
	}
	isSplit = true;
}

template<class T>
void QuadTreeNode<T>::Clear() {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->PushToStack();
		}
		isSplit = false;
	} else {
		contents.clear();
	}
}

template<class T>
void QuadTreeNode<T>::PushToStack() {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->PushToStack();
		}
	}
	nodeStack.Push(this);
}

template<class T>
void QuadTreeNode<T>::DebugDraw() {
	if (isSplit) {
		for (int i = 0; i < 4; i++) {
			children[i]->DebugDraw();
		}
	} else {
		Debug::DrawLine(Vector3(position.x, -1000, position.y), Vector3(position.x, 1000, position.y), Vector4());
	}
}

template<class T>
NodeStack<T>::NodeStack(int maxNodes) : nodes(maxNodes, *this) {
	for (auto& node : nodes) {
		Push(&node);
	}
}

template<class T>
void NodeStack<T>::Push(QuadTreeNode<T>* node) {
	stack.push(node);
}

template<class T>
QuadTreeNode<T>* NodeStack<T>::Pop() {
	if (stack.empty()) {
		return nullptr;
	} else {
		QuadTreeNode<T>* top = stack.top();
		stack.pop();
		return top;
	}
}

template<class T>
QuadTree<T>::QuadTree(Vector2 size, int maxDepth, int maxSize) : nodeStack(std::pow(4, maxDepth)), root(nodeStack) {
	root.Init(Vector2(), size);
	this->maxDepth = maxDepth;
	this->maxSize  = maxSize;
}

template<class T>
void QuadTree<T>::Insert(T object, const Vector2& pos, const Vector2& size) {
	root.Insert(object, pos, size, maxDepth, maxSize);
}

template<class T>
void QuadTree<T>::OperateOnContents(typename QuadTreeNode<T>::QuadTreeFunc func) {
	root.OperateOnContents(func);
}

template<class T>
void QuadTree<T>::OperateOnContents(typename QuadTreeNode<T>::QuadTreeFunc func, const Vector2& subsetPos, const Vector2& subsetSize) {
	root.OperateOnContents(func, subsetPos, subsetSize);
}

template<class T>
void QuadTree<T>::Clear() {
	root.Clear();
}

template<class T>
void QuadTree<T>::DebugDraw() {
	root.DebugDraw();
}
