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

using namespace NCL;
using namespace CSC8503;

LightingRPass::LightingRPass() : OGLMainRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	sphere = AssetLibrary<MeshGeometry>::GetAsset("sphere");
	quad = AssetLibrary<MeshGeometry>::GetAsset("quad");

	lightDiffuseOutTex = AssetLoader::CreateTexture(TextureType::ColourRGB16F, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	lightSpecularOutTex = AssetLoader::CreateTexture(TextureType::ColourRGB16F, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	AddScreenTexture(*lightDiffuseOutTex);
	AddScreenTexture(*lightSpecularOutTex);

	frameBuffer = AssetLoader::CreateFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(*lightDiffuseOutTex);
	frameBuffer->AddTexture(*lightSpecularOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	shader = AssetLoader::CreateShaderAndInit("light.vert", "light.frag");

	shader->Bind();

	shader->SetUniformFloat("pixelSize", 1.0f / (float)renderer.GetSplitWidth(), 1.0f / (float)renderer.GetSplitHeight());

	shader->SetUniformInt("depthTex", 0);
	shader->SetUniformInt("normalTex", 1);

	shader->Unbind();
}

LightingRPass::~LightingRPass() {
}

void LightingRPass::Render() {
	frameBuffer->Bind();

	renderer.ClearBuffers(ClearBit::Color);

	frameBuffer->Unbind();

	gameWorld.OperateOnLights([&](const Light& light) {
		DrawLight(light);
	});
}

void LightingRPass::OnWindowResize() {
	shader->Bind();

	shader->SetUniformFloat("pixelSize", 1.0f / (float)renderer.GetSplitWidth(), 1.0f / (float)renderer.GetSplitHeight());

	shader->Unbind();
}

void LightingRPass::DrawLight(const Light& light) {
	frameBuffer->Bind();
	shader->Bind();

	renderer.GetConfig().SetBlend(true, BlendFuncSrc::One, BlendFuncDst::One);
	renderer.GetConfig().SetDepthTest(true, DepthTestFunc::Always);

	depthTexIn->Bind(0);
	normalTexIn->Bind(1);

	shader->SetUniformFloat("cameraPos", gameWorld.GetMainCamera()->GetPosition());

	Matrix4 modelMatrix = Matrix4();
	modelMatrix.SetDiagonal(Vector4(1));
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 inverseViewProj = (projMatrix * viewMatrix).Inverse();

	shader->SetUniformFloat("lightPosition" , light.position);
	shader->SetUniformFloat("lightColour"   , light.colour);
	shader->SetUniformFloat("lightRadius"   , light.radius);
	shader->SetUniformFloat("lightDirection", light.direction);
	shader->SetUniformFloat("lightAngle"    , light.angle);

	shader->SetUniformMatrix("inverseProjView", inverseViewProj);
	shader->SetUniformMatrix("modelMatrix"    , modelMatrix);
	shader->SetUniformMatrix("viewMatrix"     , viewMatrix);
	shader->SetUniformMatrix("projMatrix"     , projMatrix);

	if (light.position.w == 0.0f) {
		quad->Draw();
	} else {
		renderer.GetConfig().SetCullFace(true, CullFace::Back);
		sphere->Draw();
		renderer.GetConfig().SetCullFace();
	}

	renderer.GetConfig().SetDepthTest();
	renderer.GetConfig().SetBlend();

	shader->Unbind();
	frameBuffer->Unbind();
}
