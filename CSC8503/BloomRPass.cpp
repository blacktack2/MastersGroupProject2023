/**
 * @file   BloomRPass.cpp
 * @brief  See BloomRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "BloomRPass.h"

#include "GameTechRenderer.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace CSC8503;

BloomRPass::BloomRPass() : OGLPostRenderPass(), renderer(GameTechRenderer::instance()) {
	quad = AssetLibrary<MeshGeometry>::GetAsset("quad");

	SetBloomDepth(bloomDepth);
	colourOutTex = AssetLoader::CreateTexture(TextureType::ColourRGBA16F, renderer.GetSplitWidth(), renderer.GetSplitHeight());
	AddScreenTexture(*colourOutTex);

	bloomFrameBuffer = AssetLoader::CreateFrameBuffer();
	bloomFrameBuffer->Bind();
	bloomFrameBuffer->DrawBuffers(1);
	bloomFrameBuffer->Unbind();

	combineFrameBuffer = AssetLoader::CreateFrameBuffer();
	combineFrameBuffer->Bind();
	combineFrameBuffer->AddTexture(*colourOutTex);
	combineFrameBuffer->DrawBuffers();
	combineFrameBuffer->Unbind();

	downsampleShader = AssetLoader::CreateShaderAndInit("downsample.vert", "downsample.frag");
	upsampleShader   = AssetLoader::CreateShaderAndInit("upsample.vert", "upsample.frag");
	combineShader    = AssetLoader::CreateShaderAndInit("bloomCombine.vert", "bloomCombine.frag");

	downsampleShader->Bind();

	downsampleShader->SetUniformInt("sourceTex", 0);

	upsampleShader->Bind();

	upsampleShader->SetUniformInt("sourceTex", 0);

	combineShader->Bind();

	combineShader->SetUniformInt("sceneTex", 0);
	combineShader->SetUniformInt("bloomTex", 1);

	combineShader->Unbind();
}

BloomRPass::~BloomRPass() {
}

void BloomRPass::Render() {
	bloomFrameBuffer->Bind();
	Downsample();
	Upsample();
	bloomFrameBuffer->Unbind();

	renderer.GetConfig().SetViewport();
	Combine();
}

void BloomRPass::SetBloomDepth(size_t depth) {
	bloomDepth = depth;
	mipChain.clear();

	float mipWidth  = (float)renderer.GetWidth();
	float mipHeight = (float)renderer.GetHeight();
	for (size_t i = 0; i < bloomDepth; i++) {
		mipWidth *= 0.5f;
		mipHeight *= 0.5f;

		BloomMip mip{
			mipWidth, mipHeight,
			AssetLoader::CreateTexture(TextureType::ColourRGBF, (unsigned int)mipWidth, (unsigned int)mipHeight)
		};
		mip.texture->Bind();
		mip.texture->SetEdgeWrap(EdgeWrap::ClampToEdge);
		mip.texture->SetFilters(MinFilter::Linear, MagFilter::Linear);

		mipChain.emplace_back(std::move(mip));
	}
}

void BloomRPass::SetBias(float bias) {
	combineShader->Bind();

	combineShader->SetUniformFloat("bias", bias);

	combineShader->Unbind();
}

void BloomRPass::OnWindowResize(int width, int height) {
	SetBloomDepth(bloomDepth);
}

void BloomRPass::Downsample() {
	downsampleShader->Bind();

	sceneTexIn->Bind(0);

	for (auto mip = mipChain.begin(); mip != mipChain.end(); mip++) {
		renderer.GetConfig().SetViewport(0, 0, (unsigned int)mip->width, (unsigned int)mip->height);
		bloomFrameBuffer->AddTexture(*mip->texture, 0);

		quad->Draw();

		downsampleShader->SetUniformFloat("sourcePixelSize", 1.0f / mip->width, 1.0f / mip->height);
		mip->texture->Bind(0);
	}

	downsampleShader->Unbind();
}

void BloomRPass::Upsample() {
	upsampleShader->Bind();

	renderer.GetConfig().SetBlend(true, BlendFuncSrc::One, BlendFuncDst::One);

	for (auto mip = mipChain.rbegin(); mip != std::prev(mipChain.rend()); mip++) {
		auto nextMip = std::next(mip);

		mip->texture->Bind(0);

		renderer.GetConfig().SetViewport(0, 0, (unsigned int)nextMip->width, (unsigned int)nextMip->height);
		bloomFrameBuffer->AddTexture(*nextMip->texture, 0);

		quad->Draw();
	}

	renderer.GetConfig().SetBlend();

	upsampleShader->Unbind();
}

void BloomRPass::Combine() {
	combineFrameBuffer->Bind();
	combineShader->Bind();

	sceneTexIn->Bind(0);
	mipChain.back().texture->Bind(1);

	quad->Draw();

	combineShader->Unbind();
	combineFrameBuffer->Unbind();
}
