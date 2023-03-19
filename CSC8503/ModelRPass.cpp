/**
 * @file   ModelRPass.cpp
 * @brief  See ModelRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "ModelRPass.h"

#include "GameTechRenderer.h"
#include "GameWorld.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "RenderObject.h"

using namespace NCL::CSC8503;
using namespace NCL::Rendering;

ModelRPass::ModelRPass() : OGLMainRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	albedoOutTex = AssetLoader::CreateTexture(TextureType::ColourRGBA8, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	normalOutTex = AssetLoader::CreateTexture(TextureType::ColourRGBA8, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	specOutTex   = AssetLoader::CreateTexture(TextureType::ColourRGBA8, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	depthOutTex  = AssetLoader::CreateTexture(TextureType::Depth      , renderer.GetSplitWidth(), renderer.GetSplitHeight());
	AddScreenTexture(*albedoOutTex);
	AddScreenTexture(*normalOutTex);
	AddScreenTexture(*specOutTex);
	AddScreenTexture(*depthOutTex);

	frameBuffer = AssetLoader::CreateFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(*albedoOutTex);
	frameBuffer->AddTexture(*normalOutTex);
	frameBuffer->AddTexture(*specOutTex);
	frameBuffer->AddTexture(*depthOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	AddModelShader(AssetLibrary<ShaderBase>::GetAsset("modelDefault"));
	AddModelShader(AssetLibrary<ShaderBase>::GetAsset("paintDefault"));
	AddModelShader(AssetLibrary<ShaderBase>::GetAsset("animationDefault"));
}

void ModelRPass::Render() {
	frameBuffer->Bind();
	renderer.GetConfig().SetDepthMask(true);
	renderer.GetConfig().SetDepthTest(true);

	renderer.ClearBuffers(ClearBit::ColorDepth);

	for (const auto& shader : modelShaders) {
		shader->Bind();

		Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
		float screenAspect = (float)renderer.GetSplitWidth() / (float)renderer.GetSplitHeight();
		Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

		shader->SetUniformMatrix("viewMatrix", viewMatrix);
		shader->SetUniformMatrix("projMatrix", projMatrix);
		shader->SetUniformFloat("gamma", gamma);

		shader->Unbind();
	}
	
	gameWorld.OperateOnContents([&](GameObject* gameObject) {
		RenderObject* renderObject = gameObject->GetRenderObject();
		if (!renderObject) {
			return;
		}

		renderObject->DrawToGBuffer();
	});

	renderer.GetConfig().SetDepthTest();
	renderer.GetConfig().SetDepthMask();
	frameBuffer->Unbind();
}

void ModelRPass::AddModelShader(std::shared_ptr<ShaderBase> shader) {
	modelShaders.push_back(shader);
	shader->Bind();

	shader->SetUniformInt("albedoTex", 0);
	shader->SetUniformInt("bumpTex"  , 1);
	shader->SetUniformInt("specTex"  , 2);

	shader->Unbind();
}
