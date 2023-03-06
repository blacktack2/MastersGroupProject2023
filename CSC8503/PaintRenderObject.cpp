/**
 * @file   PaintRenderObject.cpp
 * @brief  See PaintRenderObject.h.
 * 
 * @author Harry Brettell
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "PaintRenderObject.h"

#include "GameTechRenderer.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace CSC8503;
using namespace Rendering;

PaintRenderObject::PaintRenderObject(Transform& parentTransform, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<MeshMaterial> material) : RenderObject(parentTransform, mesh, material) {
	width = 1024;
	height = 1024;

	paintTexture = AssetLoader::CreateTexture(TextureType::ColourRGBA8, width, height);
	paintTexture->Bind();
	paintTexture->SetFilters(MinFilter::Linear, MagFilter::Linear);
	paintTexture->Unbind();

	std::unique_ptr<FrameBuffer> tempFramebuffer = AssetLoader::CreateFrameBuffer();
	tempFramebuffer->Bind();
	tempFramebuffer->AddTexture(*paintTexture, 0);
	GameTechRenderer::instance().ClearBuffers(ClearBit::Color);
	tempFramebuffer->Unbind();
}

PaintRenderObject::PaintRenderObject(RenderObject& other, Transform& parentTransform) : RenderObject(other, parentTransform) {
	width = 1024;
	height = 1024;
	paintTexture = AssetLoader::CreateTexture(TextureType::ColourRGBA8, width, height);
	paintTexture->Bind();
	paintTexture->SetFilters(MinFilter::Linear, MagFilter::Linear);
	paintTexture->Unbind();

	std::unique_ptr<FrameBuffer> tempFramebuffer = AssetLoader::CreateFrameBuffer();
	tempFramebuffer->Bind();
	tempFramebuffer->AddTexture(*paintTexture, 0);
	GameTechRenderer::instance().ClearBuffers(ClearBit::Color);
	tempFramebuffer->Unbind();
}

PaintRenderObject::~PaintRenderObject() {
}

void PaintRenderObject::PreDraw(int sublayer, ShaderBase& shader) {
	paintTexture->Bind(3, shader.GetUniformLocation("paintTex"));
}

ShaderBase& PaintRenderObject::GetDefaultShader() {
	return *AssetLibrary::instance().GetShader("paintDefault");
}
