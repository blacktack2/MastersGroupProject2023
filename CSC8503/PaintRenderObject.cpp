/**
 * @file   PaintRenderObject.cpp
 * @brief  See PaintRenderObject.h.
 * 
 * @author Harry Brettell
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "PaintRenderObject.h"
#include "AssetLibrary.h"
#include "OGLShader.h"

using namespace NCL::CSC8503;

PaintRenderObject::PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, MeshMaterial* material) : RenderObject(parentTransform, mesh, material) {
	paintTexture = new OGLTexture(1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	paintTexture->Bind();
	paintTexture->SetFilters(GL_LINEAR, GL_LINEAR);
	paintTexture->Unbind();

	paintCollisions.push_back(PaintCollision(Vector3(0, 0, 0), 1));

	width = 1000;
	height = 1000;
}

PaintRenderObject::PaintRenderObject(RenderObject& other, Transform* parentTransform) : RenderObject(other, parentTransform) {
	paintTexture = new OGLTexture(1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	width = 1000;
	height = 1000;
}

PaintRenderObject::~PaintRenderObject() {
	delete paintTexture;
}

void PaintRenderObject::PreDraw(int sublayer, ShaderBase* shader) {
	paintTexture->Bind(3, shader->GetUniformLocation("paintTex"));
}
