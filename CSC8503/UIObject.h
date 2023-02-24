/**
 * @file   UIObject.h
 * @brief  A class which lets other UI related class inherits from.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "RenderObject.h"

namespace NCL {
	namespace CSC8503 {
		using namespace std;
		class UIObject{
		public:
			UIObject();
			~UIObject();

			virtual void Update(float dt){};

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			virtual Vector4 GetDimension() { return Vector4(0.0f); };
		protected:
			RenderObject* renderObject;
		};
	}
}
