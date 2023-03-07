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
#include "UIObject.h"

using namespace NCL;
using namespace CSC8503;

MenuRenderObject::MenuRenderObject(TextureBase* texture) {
	this->texture = texture;
}

MenuRenderObject::~MenuRenderObject() {
}

void MenuRenderObject::Draw(const Vector4& dimensions) {
	std::shared_ptr<ShaderBase> shader = AssetLibrary::GetShader("menu");

	shader->Bind();

	texture->Bind(0);

	float data[] = {
		dimensions.z, 0.0f        , 0.0f,
		0.0f        , dimensions.w, 0.0f,
		-dimensions.x, -dimensions.y, 1.0f
	};
	Matrix3 quadMatrix = Matrix3(data);

	shader->SetUniformMatrix("textureMatrix", quadMatrix);
	
	texture->Bind(0);

	std::shared_ptr<MeshGeometry> quad = AssetLibrary::GetMesh("quad");
	quad->Draw();

	shader->Unbind();
}
