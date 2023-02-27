/**
 * @file   MenuRenderObject.cpp
 * @brief  See MenuRenderObject.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "MenuRenderObject.h"

#include "AssetLibrary.h"
#include "UIObject.h"

using namespace NCL;
using namespace CSC8503;

MenuRenderObject::MenuRenderObject(TextureBase* texture) {
	this->texture = texture;
}

MenuRenderObject::~MenuRenderObject() {
}

void MenuRenderObject::Draw(const Vector4& dimensions) {
	ShaderBase* shader = AssetLibrary::GetShader("menu");

	shader->Bind();

	texture->Bind(0);

	Matrix4 quadMatrix = Matrix4();
	quadMatrix.SetDiagonal(Vector4(1));
	quadMatrix = quadMatrix *
		Matrix4::Translation(Vector3((dimensions.x), (dimensions.y), 0.0f)) *
		Matrix4::Scale(Vector3((dimensions.z), (dimensions.w), 1.0f));

	shader->SetUniformMatrix("modelMatrix", quadMatrix);
	
	//glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)quadMatrix.array);
	//glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	//glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	//glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	texture->Bind(0);

	MeshGeometry* quad = AssetLibrary::GetMesh("quad");
	quad->Draw();

	shader->Unbind();
}
