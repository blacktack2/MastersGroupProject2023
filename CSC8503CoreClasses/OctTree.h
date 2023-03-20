#pragma once

namespace NCL {
    namespace Maths {
        class Ray;
    }
    using namespace Maths;
    namespace CSC8503 {

        class GameObject;
        class OctTree;
        class NodeStack;

        struct OctTreeEntry {
            Vector3 pos;
            Vector3 size;
            GameObject* object;

            OctTreeEntry(GameObject* obj, Vector3 pos, Vector3 size) {
                object = obj;
                this->pos = pos;
                this->size = size;
            }
        };

        class OctTreeNode {
        public:
            typedef std::function<void(std::list<OctTreeEntry>&, const Vector3& nodePos, const Vector3& nodeSize)> OctTreeFunc;
            friend class OctTree;

            OctTreeNode(NodeStack& nodeStack);
            ~OctTreeNode();

            void Init(Vector3 pos, Vector3 size);
            void Insert(GameObject* object, const Vector3& objectPos, const Vector3& objectSize, int depthLeft, int maxSize);

            void OperateOnContents(OctTreeFunc& func);
            void OperateOnContents(OctTreeFunc& func, const Vector3& subsetPos, const Vector3& subsetSize);
            void OperateOnContents(OctTreeFunc& func, const Ray& ray);

            void Split();
            void Clear();
            void PushToStack();

            void DebugDraw();
        protected:
            NodeStack& nodeStack;

            std::list<OctTreeEntry> contents{};

            Vector3 position;
            Vector3 size;

            OctTreeNode* children[8]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
            bool isSplit;
        };

        class NodeStack {
        public:
            explicit NodeStack(int maxNodes);
            ~NodeStack() {}

            void Push(OctTreeNode* node);
            OctTreeNode* Pop();
        private:
            std::vector<OctTreeNode> nodes;
            std::stack<OctTreeNode*> stack;
        };

        class OctTree {
        public:
            OctTree(Vector3 size, int maxDepth = 6, int maxSize = 5);
            ~OctTree() = default;

            void Insert(GameObject* object, const Vector3& pos, const Vector3& size);
            void OperateOnContents(typename OctTreeNode::OctTreeFunc func);
            void OperateOnContents(typename OctTreeNode::OctTreeFunc func, const Vector3& subsetPos, const Vector3& subsetSize);
            void OperateOnContents(typename OctTreeNode::OctTreeFunc func, const Ray& ray);

            void Clear();

            void DebugDraw();
        protected:
            OctTreeNode root;
            NodeStack nodeStack;
            int maxDepth;
            int maxSize;
        };
    }
}
