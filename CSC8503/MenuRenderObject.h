/**
 * @file   MenuRenderObject.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "Vector4.h"

#include <memory>
#include <string>
#include <vector>

namespace NCL {
	class MeshGeometry;
}

namespace NCL::Rendering {
	class ShaderBase;
	class TextureBase;
}

using namespace NCL;
using namespace NCL::Maths;
using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class MenuRenderObject {
	public:
		MenuRenderObject(std::shared_ptr<TextureBase> texture);
		~MenuRenderObject();

		void Draw(const Vector4& dimensions);
	private:
		std::shared_ptr<TextureBase> texture;

		std::shared_ptr<ShaderBase> shader;
		std::shared_ptr<MeshGeometry> quad;
	};
}
