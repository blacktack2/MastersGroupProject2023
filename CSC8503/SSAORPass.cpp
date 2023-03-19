/**
 * @file   SSAORPass.cpp
 * @brief  See SSAORPass.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "SSAORPass.h"

#include "GameTechRenderer.h"
#include "GameWorld.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "BufferObjectBase.h"
#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Maths;
using namespace NCL::Rendering;

SSAORPass::SSAORPass() : OGLMainRenderPass(), renderer(GameTechRenderer::instance()) {
	quad = AssetLibrary<MeshGeometry>::GetAsset("quad");

	ssaoTex    = AssetLoader::CreateTexture(TextureType::ColourR8, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	ssaoOutTex = AssetLoader::CreateTexture(TextureType::ColourR8, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	AddScreenTexture(*ssaoTex);
	AddScreenTexture(*ssaoOutTex);

	ssaoFrameBuffer = AssetLoader::CreateFrameBuffer();
	ssaoFrameBuffer->Bind();
	ssaoFrameBuffer->AddTexture(*ssaoTex);
	ssaoFrameBuffer->DrawBuffers();

	blurFrameBuffer = AssetLoader::CreateFrameBuffer();
	blurFrameBuffer->Bind();
	blurFrameBuffer->AddTexture(*ssaoOutTex);
	blurFrameBuffer->DrawBuffers();
	blurFrameBuffer->Unbind();

	ssaoShader = AssetLoader::CreateShaderAndInit("ssao.vert", "ssao.frag");
	blurShader = AssetLoader::CreateShaderAndInit("ssaoBlur.vert", "ssaoBlur.frag");

	ssaoShader->Bind();

	ssaoShader->SetUniformInt("depthTex" , 0);
	ssaoShader->SetUniformInt("normalTex", 1);
	ssaoShader->SetUniformInt("noiseTex" , 2);

	ssaoShader->SetUniformFloat("noiseScale", (float)renderer.GetSplitWidth() / (float)noiseTexSize, (float)renderer.GetSplitHeight() / (float)noiseTexSize);

	blurShader->Bind();

	blurShader->SetUniformInt("ssaoTex", 0);

	blurShader->Unbind();

	kernelSSBO = AssetLoader::CreateBufferObject(numKernels, 1);
	GenerateKernels();
	GenerateNoiseTex();
}

void SSAORPass::OnWindowResize(int width, int height) {
	RenderPassBase::OnWindowResize(width, height);
	ssaoShader->Bind();

	ssaoShader->SetUniformFloat("noiseScale", (float)renderer.GetSplitWidth() / (float)noiseTexSize, (float)renderer.GetSplitHeight() / (float)noiseTexSize);

	ssaoShader->Unbind();
}

void SSAORPass::Render() {
	DrawSSAO();
	BlurSSAO();
}

void SSAORPass::SetRadius(float radius) {
	ssaoShader->Bind();

	ssaoShader->SetUniformFloat("radius", radius);

	ssaoShader->Unbind();
}

void SSAORPass::SetBias(float bias) {
	ssaoShader->Bind();

	ssaoShader->SetUniformFloat("bias", bias);

	ssaoShader->Unbind();
}

void SSAORPass::SetNumKernels(size_t num) {
	numKernels = num;
	GenerateKernels();
}

void SSAORPass::DrawSSAO() {
	ssaoFrameBuffer->Bind();
	renderer.ClearBuffers(ClearBit::Color);
	ssaoShader->Bind();

	depthTexIn->Bind(0);
	normalTexIn->Bind(1);
	noiseTex->Bind(2);

	Matrix4 viewMatrix = GameWorld::instance().GetMainCamera()->BuildViewMatrix();
	ssaoShader->SetUniformMatrix("viewMatrix", viewMatrix);
	float aspect = (float)renderer.GetSplitWidth() / (float)renderer.GetSplitHeight();
	Matrix4 projMatrix = GameWorld::instance().GetMainCamera()->BuildProjectionMatrix(aspect);
	ssaoShader->SetUniformMatrix("projMatrix", projMatrix);

	quad->Draw();

	ssaoShader->Unbind();
	ssaoFrameBuffer->Unbind();
}

void SSAORPass::BlurSSAO() {
	blurFrameBuffer->Bind();
	renderer.ClearBuffers(ClearBit::Color);
	blurShader->Bind();

	ssaoTex->Bind(0);

	quad->Draw();

	blurShader->Unbind();
	blurFrameBuffer->Unbind();
}

void SSAORPass::GenerateKernels() {
	static std::uniform_real_distribution<float> random01 = std::uniform_real_distribution(0.0f, 1.0f);
	static std::uniform_real_distribution<float> random11 = std::uniform_real_distribution(-1.0f, 1.0f);
	static std::default_random_engine generator;

	kernelSSBO->Bind();
	if (kernels.size() != numKernels) {
		kernels.resize(numKernels);
		kernelSSBO->Resize(numKernels * sizeof(Vector3));
	}

	for (size_t i = 0; i < numKernels; i++) {
		Vector3 sample = Vector3(
			random11(generator),
			random11(generator),
			random01(generator)
		);

		sample.Normalise();
		sample *= random01(generator);
		
		float scale = (float)i / (float)numKernels;
		scale = 0.1f + (scale * scale) * 0.9f;
		sample *= scale;

		kernels[i] = sample;
	}

	kernelSSBO->Write(kernels.data());

	kernelSSBO->Unbind();
}

void SSAORPass::GenerateNoiseTex() {
	static std::uniform_real_distribution<float> random = std::uniform_real_distribution(-1.0f, 1.0f);
	static std::default_random_engine generator;

	std::vector<Vector3> noiseData;
	noiseData.resize((size_t)noiseTexSize * (size_t)noiseTexSize);

	for (size_t i = 0; i < (size_t)noiseTexSize * (size_t)noiseTexSize; i++) {
		noiseData[i] = Vector3(
			random(generator),
			random(generator),
			0.0f
		);
	}

	noiseTex = AssetLoader::CreateTexture(TextureType::ColourRGB16F, noiseTexSize, noiseTexSize);
	noiseTex->Bind();

	noiseTex->Upload(noiseData.data(), PixelDataFormat::RGB, PixelDataType::Float);
	noiseTex->SetEdgeWrap(EdgeWrap::Repeat);

	noiseTex->Unbind();
}
