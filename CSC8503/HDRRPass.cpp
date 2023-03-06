/**
 * @file   HDRRPass.cpp
 * @brief  See HDRRPass.h
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "HDRRPass.h"

#include "GameTechRenderer.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace CSC8503;

HDRRPass::HDRRPass() : OGLPostRenderPass(), renderer(GameTechRenderer::instance()) {
	quad = AssetLibrary::instance().GetMesh("quad");

	sceneOutTex = AssetLoader::CreateTexture(TextureType::ColourRGBA16F, renderer.GetWidth(), renderer.GetHeight());
	AddScreenTexture(*sceneOutTex);

	frameBuffer = AssetLoader::CreateFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(*sceneOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	shader = AssetLoader::CreateShader("hdr.vert", "hdr.frag");

	shader->Bind();

	shader->SetUniformInt("sceneTex", 0);

	shader->Unbind();
}

HDRRPass::~HDRRPass() {
}

void HDRRPass::Render() {
	frameBuffer->Bind();
	renderer.ClearBuffers(ClearBit::Color);
	shader->Bind();

	sceneTexIn.value().get().Bind(0);

	quad->Draw();

	shader->Unbind();
	frameBuffer->Unbind();
}

void HDRRPass::SetExposure(float exposure) {
	shader->Bind();

	shader->SetUniformFloat("exposure", exposure);

	shader->Unbind();
}
