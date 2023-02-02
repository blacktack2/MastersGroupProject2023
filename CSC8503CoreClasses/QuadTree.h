#pragma once

namespace NCL {
	namespace Maths {
		class Ray;
	}
	using namespace Maths;
	namespace CSC8503 {

		class GameObject;
		class QuadTree;
		class NodeStack;
		
		struct QuadTreeEntry {
			Vector2 pos;
			Vector2 size;
			GameObject* object;

			QuadTreeEntry(GameObject* obj, Vector2 pos, Vector2 size) {
				object = obj;
				this->pos = pos;
				this->size = size;
			}
		};

		class QuadTreeNode {
		public:
			typedef std::function<void(std::list<QuadTreeEntry>&, const Vector2& nodePos, const Vector2& nodeSize)> QuadTreeFunc;
			friend class QuadTree;

			QuadTreeNode(NodeStack& nodeStack);
			~QuadTreeNode();

			void Init(Vector2 pos, Vector2 size);
			void Insert(GameObject* object, const Vector2& objectPos, const Vector2& objectSize, int depthLeft, int maxSize);

			void OperateOnContents(QuadTreeFunc& func);
			void OperateOnContents(QuadTreeFunc& func, const Vector2& subsetPos, const Vector2& subsetSize);
			void OperateOnContents(QuadTreeFunc& func, const Ray& ray);

			void Split();
			void Clear();
			void PushToStack();

			void DebugDraw();
		protected:
			NodeStack& nodeStack;

			std::list<QuadTreeEntry> contents{};

			Vector2 position;
			Vector2 size;

			QuadTreeNode* children[4]{ nullptr, nullptr, nullptr, nullptr };
			bool isSplit;
		};

		class NodeStack {
		public:
			explicit NodeStack(int maxNodes);
			~NodeStack() {}

			void Push(QuadTreeNode* node);
			QuadTreeNode* Pop();
		private:
			std::vector<QuadTreeNode> nodes;
			std::stack<QuadTreeNode*> stack;
		};
		
		class QuadTree {
		public:
			QuadTree(Vector2 size, int maxDepth = 6, int maxSize = 5);
			~QuadTree() = default;

			void Insert(GameObject* object, const Vector2& pos, const Vector2& size);
			void OperateOnContents(typename QuadTreeNode::QuadTreeFunc func);
			void OperateOnContents(typename QuadTreeNode::QuadTreeFunc func, const Vector2& subsetPos, const Vector2& subsetSize);
			void OperateOnContents(typename QuadTreeNode::QuadTreeFunc func, const Ray& ray);

			void Clear();

			void DebugDraw();
		protected:
			QuadTreeNode root;
			NodeStack nodeStack;
			int maxDepth;
			int maxSize;
		};
	}
}
