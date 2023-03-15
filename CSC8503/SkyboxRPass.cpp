/**
 * @file   SkyboxRPass.cpp
 * @brief  See SkyboxRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "SkyboxRPass.h"

#include "GameTechRenderer.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace CSC8503;

SkyboxRPass::SkyboxRPass() : OGLMainRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	quad = AssetLibrary<MeshGeometry>::GetAsset("quad");

	colourOutTex = AssetLoader::CreateTexture(TextureType::ColourRGB16F, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	AddScreenTexture(*colourOutTex);

	frameBuffer = AssetLoader::CreateFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(*colourOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	shader = AssetLoader::CreateShaderAndInit("skybox.vert", "skybox.frag");

	shader->Bind();

	shader->SetUniformFloat("sunDir", 0.1f, 0.6f, 0.2f);

	shader->SetUniformFloat("cirrus", 0.5f);
	shader->SetUniformFloat("cumulus", 0.5f);

	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(renderer.GetAspect());
	shader->SetUniformMatrix("projMatrix", projMatrix);

	shader->Unbind();
}

SkyboxRPass::~SkyboxRPass() {
}

void SkyboxRPass::Render() {
	frameBuffer->Bind();
	renderer.ClearBuffers(ClearBit::Color);

	shader->Bind();

	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	shader->SetUniformMatrix("viewMatrix", viewMatrix);

	shader->SetUniformFloat("time", gameWorld.GetRunTime());

	quad->Draw();

	shader->Unbind();

	frameBuffer->Unbind();
}

void SkyboxRPass::SetSunDir(const Vector3& direction) {
	shader->Bind();

	shader->SetUniformFloat("sunDir", direction);

	shader->Unbind();
}
