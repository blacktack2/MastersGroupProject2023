/**
 * @file   UIObject.h
 * @brief  A class which lets other UI related class inherits from.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "MenuRenderObject.h"

namespace NCL {
	namespace CSC8503 {
		class UIObject{
		public:
			UIObject();
			~UIObject();

			virtual void Update(float dt){};

			MenuRenderObject* GetRenderObject() const {
				return renderObject;
			}

			void SetRenderObject(MenuRenderObject* newObject) {
				renderObject = newObject;
			}

			virtual Vector4 GetDimension() { return Vector4(0.0f); };
		protected:
			MenuRenderObject* renderObject;
		};
	}
}
