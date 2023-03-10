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
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "RenderObject.h"

using namespace NCL;
using namespace CSC8503;

ModelRPass::ModelRPass() : OGLMainRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	diffuseOutTex = AssetLoader::CreateTexture(TextureType::ColourRGBA8, renderer.GetWidth(), renderer.GetHeight());
	normalOutTex  = AssetLoader::CreateTexture(TextureType::ColourRGBA8, renderer.GetWidth(), renderer.GetHeight());
	depthOutTex   = AssetLoader::CreateTexture(TextureType::Depth, renderer.GetWidth(), renderer.GetHeight());
	AddScreenTexture(*diffuseOutTex);
	AddScreenTexture(*normalOutTex);
	AddScreenTexture(*depthOutTex);

	frameBuffer = AssetLoader::CreateFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(*diffuseOutTex);
	frameBuffer->AddTexture(*normalOutTex);
	frameBuffer->AddTexture(*depthOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	AddModelShader(AssetLibrary<ShaderBase>::GetAsset("modelDefault"));
	AddModelShader(AssetLibrary<ShaderBase>::GetAsset("paintDefault"));
	AddModelShader(AssetLibrary<ShaderBase>::GetAsset("animationDefault"));
}

ModelRPass::~ModelRPass() {
}

void ModelRPass::Render() {
	frameBuffer->Bind();
	renderer.GetConfig().SetDepthMask(true);
	renderer.GetConfig().SetDepthTest(true);

	renderer.ClearBuffers(ClearBit::ColorDepth);

	for (const auto& shader : modelShaders) {
		shader->Bind();

		Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
		float screenAspect = (float)renderer.GetWidth() / (float)renderer.GetHeight();
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

		renderObject->Draw();
	});

	renderer.GetConfig().SetDepthTest();
	renderer.GetConfig().SetDepthMask();
	frameBuffer->Unbind();
}

void ModelRPass::AddModelShader(std::shared_ptr<ShaderBase> shader) {
	modelShaders.push_back(shader);
	shader->Bind();

	shader->SetUniformInt("diffuseTex", 0);
	shader->SetUniformInt("bumpTex"   , 1);

	shader->Unbind();
}
