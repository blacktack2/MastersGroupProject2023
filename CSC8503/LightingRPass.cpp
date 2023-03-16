/**
 * @file   LightingRPass.cpp
 * @brief  See LightingRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "LightingRPass.h"

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

LightingRPass::LightingRPass() : OGLMainRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	sphere = AssetLibrary<MeshGeometry>::GetAsset("sphere");
	quad = AssetLibrary<MeshGeometry>::GetAsset("quad");

	shadowMapTex = AssetLoader::CreateTexture(TextureType::Shadow, SHADOWSIZE, SHADOWSIZE);

	lightDiffuseOutTex = AssetLoader::CreateTexture(TextureType::ColourRGB16F, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	lightSpecularOutTex = AssetLoader::CreateTexture(TextureType::ColourRGB16F, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	AddScreenTexture(*lightDiffuseOutTex);
	AddScreenTexture(*lightSpecularOutTex);

	shadowFrameBuffer = AssetLoader::CreateFrameBuffer();
	shadowFrameBuffer->Bind();
	shadowFrameBuffer->AddTexture(*shadowMapTex);
	shadowFrameBuffer->DrawBuffers(0);
	shadowFrameBuffer->Unbind();

	lightFrameBuffer = AssetLoader::CreateFrameBuffer();
	lightFrameBuffer->Bind();
	lightFrameBuffer->AddTexture(*lightDiffuseOutTex);
	lightFrameBuffer->AddTexture(*lightSpecularOutTex);
	lightFrameBuffer->DrawBuffers();
	lightFrameBuffer->Unbind();

	lightShader = AssetLoader::CreateShaderAndInit("light.vert", "light.frag");

	lightShader->Bind();

	lightShader->SetUniformFloat("pixelSize", 1.0f / (float)renderer.GetSplitWidth(), 1.0f / (float)renderer.GetSplitHeight());

	lightShader->SetUniformInt("depthTex", 0);
	lightShader->SetUniformInt("normalTex", 1);
	lightShader->SetUniformInt("shadowTex", 2);

	lightShader->Unbind();

	AddShadowShader(AssetLibrary<ShaderBase>::GetAsset("shadowDefault"));
}

LightingRPass::~LightingRPass() {
}

void LightingRPass::OnWindowResize(int width, int height) {
	RenderPassBase::OnWindowResize(width, height);
	lightShader->Bind();

	lightShader->SetUniformFloat("pixelSize", 1.0f / (float)renderer.GetSplitWidth(), 1.0f / (float)renderer.GetSplitHeight());

	lightShader->Unbind();
}

void LightingRPass::Render() {
	lightFrameBuffer->Bind();

	renderer.ClearBuffers(ClearBit::Color);

	lightFrameBuffer->Unbind();

	gameWorld.OperateOnLights([&](const Light& light) {
		const Matrix4& shadowMatrix = light.GetShadowMatrix();

		DrawShadowMap(light, shadowMatrix);
		DrawLight(light, shadowMatrix);
	});
}

void LightingRPass::AddShadowShader(std::shared_ptr<ShaderBase> shader) {
	shadowShaders.push_back(shader);
}

void LightingRPass::DrawShadowMap(const Light& light, const Matrix4& shadowMatrix) {
	for (const auto& shader : shadowShaders) {
		shader->Bind();

		shader->SetUniformMatrix("projViewMatrix", shadowMatrix);

		shader->Unbind();
	}

	shadowFrameBuffer->Bind();

	renderer.GetConfig().SetViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	renderer.GetConfig().SetColourMask(false, false, false, false);
	renderer.GetConfig().SetDepthMask(true);
	renderer.GetConfig().SetDepthTest(true);

	renderer.ClearBuffers(ClearBit::Depth);

	gameWorld.OperateOnContents([&](GameObject* gameObject) {
		RenderObject* renderObject = gameObject->GetRenderObject();
		if (!renderObject) {
			return;
		}

		renderObject->DrawToShadowMap();
	});

	renderer.GetConfig().SetDepthTest();
	renderer.GetConfig().SetDepthMask();
	renderer.GetConfig().SetColourMask();

	renderer.GetConfig().SetViewport();

	shadowFrameBuffer->Unbind();
}

void LightingRPass::DrawLight(const Light& light, const Matrix4& shadowMatrix) {
	lightFrameBuffer->Bind();
	lightShader->Bind();

	renderer.GetConfig().SetBlend(true, BlendFuncSrc::One, BlendFuncDst::One);
	renderer.GetConfig().SetDepthTest(true, DepthTestFunc::Always);

	depthTexIn->Bind(0);
	normalTexIn->Bind(1);
	shadowMapTex->Bind(2);

	lightShader->SetUniformFloat("cameraPos", gameWorld.GetMainCamera()->GetPosition());

	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(renderer.GetAspect());
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 inverseViewProj = (projMatrix * viewMatrix).Inverse();

	light.Upload(*lightShader);

	lightShader->SetUniformMatrix("inverseProjView", inverseViewProj);
	lightShader->SetUniformMatrix("viewMatrix"     , viewMatrix);
	lightShader->SetUniformMatrix("projMatrix"     , projMatrix);
	lightShader->SetUniformMatrix("shadowMatrix"   , shadowMatrix);

	if (dynamic_cast<const DirectionalLight*>(&light)) {
		quad->Draw();
	} else {
		renderer.GetConfig().SetCullFace(true, CullFace::Back);
		sphere->Draw();
		renderer.GetConfig().SetCullFace();
	}

	renderer.GetConfig().SetDepthTest();
	renderer.GetConfig().SetBlend();

	lightShader->Unbind();
	lightFrameBuffer->Unbind();
}
