/**
 * @file   GBufferRPass.cpp
 * @brief  See GBufferRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "CombineRPass.h"

#include "GameTechRenderer.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace CSC8503;

CombineRPass::CombineRPass() : OGLCombineRenderPass(), renderer(GameTechRenderer::instance()) {
	quad = AssetLibrary::GetMesh("quad");

	sceneOutTex = AssetLoader::CreateTexture(TextureType::ColourRGB16F, renderer.GetWidth(), renderer.GetHeight());
	AddScreenTexture(*sceneOutTex);

	frameBuffer = AssetLoader::CreateFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(*sceneOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	shader = AssetLoader::CreateShader("combine.vert", "combine.frag");

	shader->Bind();

	shader->SetUniformInt("mode", 0);
	shader->SetUniformFloat("ambienceColour", 0.1f, 0.1f, 0.1f);

	shader->SetUniformInt("skyboxTex", 0);
	shader->SetUniformInt("diffuseTex", 1);
	shader->SetUniformInt("diffuseLightTex", 2);
	shader->SetUniformInt("specularLightTex", 3);
	shader->SetUniformInt("ssaoTex", 4);
	shader->SetUniformInt("normalTex", 5);
	shader->SetUniformInt("depthTex", 6);

	shader->Unbind();
}

CombineRPass::~CombineRPass() {
}

void CombineRPass::Render() {
	frameBuffer->Bind();
	renderer.ClearBuffers(ClearBit::Color);
	shader->Bind();

	skyboxTexIn.value().get().Bind(0);
	diffuseTexIn.value().get().Bind(1);
	diffuseLightTexIn.value().get().Bind(2);
	specularLightTexIn.value().get().Bind(3);
	ssaoTexIn.value().get().Bind(4);
	normalTexIn.value().get().Bind(5);
	depthTexIn.value().get().Bind(6);

	quad->Draw();

	shader->Unbind();
	frameBuffer->Unbind();
}

void CombineRPass::SetRenderMode(RenderMode mode) {
	shader->Bind();

	shader->SetUniformInt("mode", (int)mode);

	shader->Unbind();
}
