/**
 * @file   MenuRenderObject.cpp
 * @brief  See MenuRenderObject.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "MenuRenderObject.h"

#include "AssetLibrary.h"

#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Maths;
using namespace NCL::Rendering;

MenuRenderObject::MenuRenderObject(std::shared_ptr<TextureBase> texture) : texture(texture) {
	shader = AssetLibrary<ShaderBase>::GetAsset("menu");
	quad   = AssetLibrary<MeshGeometry>::GetAsset("quad");
}

MenuRenderObject::~MenuRenderObject() {
}

void MenuRenderObject::Draw(const Vector4& dimensions) {
	shader->Bind();

	texture->Bind(0);

	float data[] = {
		 dimensions.z,  0.0f        , 0.0f,
		 0.0f        ,  dimensions.w, 0.0f,
		-dimensions.x, -dimensions.y, 1.0f,
	};
	Matrix3 quadMatrix = Matrix3(data);

	shader->SetUniformMatrix("textureMatrix", quadMatrix);
	
	texture->Bind(0);

	quad->Draw();

	shader->Unbind();
}

void MenuRenderObject::SetTexture(std::shared_ptr<TextureBase> texture) {
	this->texture = texture;
}
