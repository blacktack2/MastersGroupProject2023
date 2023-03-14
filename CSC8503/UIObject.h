/**
 * @file   UIObject.h
 * @brief  A class which lets other UI related class inherits from.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "MenuRenderObject.h"

#include "Vector4.h"

#include <functional>
#include <memory>
#include <vector>

namespace NCL::Rendering {
	class TextureBase;
}

using namespace NCL::Maths;
using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class MenuRenderObject;

	class UIObject{
	public:
		UIObject(std::shared_ptr<TextureBase> texture);
		~UIObject() = default;

		virtual void Update(float dt) = 0;
		virtual Vector4 GetDimension() const = 0;
		
		void Draw();

		MenuRenderObject& GetRenderObject() const;

		bool HasRenderObject() const;

		void AddChild(UIObject& child) {
			children.push_back(child);
		}
	protected:
		virtual void DrawExtras() {}

		std::unique_ptr<MenuRenderObject> renderObject;

		std::vector<std::reference_wrapper<UIObject>> children{};
	};
}
