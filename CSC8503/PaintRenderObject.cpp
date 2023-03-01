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
#include "GameTechRenderer.h"
#include "OGLFrameBuffer.h"

using namespace NCL;
using namespace CSC8503;
using namespace Rendering;

PaintRenderObject::PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, MeshMaterial* material) : RenderObject(parentTransform, mesh, material) {
	width = 1024;
	height = 1024;
	paintTexture = new OGLTexture(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	paintTexture->Bind();
	paintTexture->SetFilters(GL_LINEAR, GL_LINEAR);
	paintTexture->Unbind();

	OGLFrameBuffer tempFramebuffer;
	tempFramebuffer.Bind();
	tempFramebuffer.AddTexture(paintTexture, 0);
	GameTechRenderer::instance().ClearBuffers(ClearBit::Color);
	tempFramebuffer.Unbind();
}

PaintRenderObject::PaintRenderObject(RenderObject& other, Transform* parentTransform) : RenderObject(other, parentTransform) {
	width = 1024;
	height = 1024;
	paintTexture = new OGLTexture(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	paintTexture->Bind();
	paintTexture->SetFilters(GL_LINEAR, GL_LINEAR);
	paintTexture->Unbind();
}

PaintRenderObject::~PaintRenderObject() {
	delete paintTexture;
}

void PaintRenderObject::PreDraw(int sublayer, ShaderBase* shader) {
	paintTexture->Bind(3, shader->GetUniformLocation("paintTex"));
}

ShaderBase* PaintRenderObject::GetDefaultShader() {
	return AssetLibrary::GetShader("paintDefault");
}
