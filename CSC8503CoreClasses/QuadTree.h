#pragma once

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		template<class T>
		class QuadTree;

		template<class T>
		class NodeStack;

		template<class T>
		struct QuadTreeEntry {
			Vector2 pos;
			Vector2 size;
			T object;

			QuadTreeEntry(T obj, Vector2 pos, Vector2 size) {
				object = obj;
				this->pos = pos;
				this->size = size;
			}
		};

		template<class T>
		class QuadTreeNode {
		public:
			typedef std::function<void(std::list<QuadTreeEntry<T>>&, const Vector2& nodePos, const Vector2& nodeSize)> QuadTreeFunc;
			friend class QuadTree<T>;

			QuadTreeNode(NodeStack<T>& nodeStack);
			~QuadTreeNode();

			void Init(Vector2 pos, Vector2 size);
			void Insert(T& object, const Vector2& objectPos, const Vector2& objectSize, int depthLeft, int maxSize);

			void OperateOnContents(QuadTreeFunc& func);
			void OperateOnContents(QuadTreeFunc& func, const Vector2& subsetPos, const Vector2& subsetSize);

			void Split();
			void Clear();
			void PushToStack();

			void DebugDraw();
		protected:
			NodeStack<T>& nodeStack;

			std::list<QuadTreeEntry<T>> contents{};

			Vector2 position;
			Vector2 size;

			QuadTreeNode<T>* children[4]{ nullptr, nullptr, nullptr, nullptr };
			bool isSplit;
		};

		template<class T>
		class NodeStack {
		public:
			explicit NodeStack(int maxNodes);

			void Push(QuadTreeNode<T>* node);
			QuadTreeNode<T>* Pop();
		private:
			std::vector<QuadTreeNode<T>> nodes;
			std::stack<QuadTreeNode<T>*> stack;
		};

		template<class T>
		class QuadTree {
		public:
			QuadTree(Vector2 size, int maxDepth = 6, int maxSize = 5);
			~QuadTree() = default;

			void Insert(T object, const Vector2& pos, const Vector2& size);
			void OperateOnContents(typename QuadTreeNode<T>::QuadTreeFunc func);
			void OperateOnContents(typename QuadTreeNode<T>::QuadTreeFunc func, const Vector2& subsetPos, const Vector2& subsetSize);

			void Clear();

			void DebugDraw();
		protected:
			QuadTreeNode<T> root;
			NodeStack<T> nodeStack;
			int maxDepth;
			int maxSize;
		};
	}
}

#include "QuadTree.cpp"
