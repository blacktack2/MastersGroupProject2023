/**
 * @file   MenuRenderObject.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RenderObject.h"

#include "TextureBase.h"
#include "MeshAnimation.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"

#include <string>
#include <vector>

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
