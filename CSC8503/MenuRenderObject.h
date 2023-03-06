/**
 * @file   MenuRenderObject.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RenderObject.h"

#include <memory>
#include <string>
#include <vector>

namespace NCL::Rendering {
	class TextureBase;
}

namespace NCL::CSC8503 {
	class MenuRenderObject {
	public:
		MenuRenderObject(TextureBase* texture);
		~MenuRenderObject();

		void Draw(const Vector4& dimensions);
	private:
		TextureBase* texture;
	};
}
