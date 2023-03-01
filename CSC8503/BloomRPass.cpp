/**
 * @file   BloomRPass.cpp
 * @brief  See BloomRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "BloomRPass.h"

#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"

#include <algorithm>

using namespace NCL::CSC8503;

BloomRPass::BloomRPass(OGLRenderer& renderer) :
	OGLPostRenderPass(renderer) {
	SetBloomDepth(bloomDepth);
	colourOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGBA16F);
	AddScreenTexture(colourOutTex);

	bloomFrameBuffer = new OGLFrameBuffer();
	bloomFrameBuffer->Bind();
	bloomFrameBuffer->DrawBuffers(1);
	bloomFrameBuffer->Unbind();

	combineFrameBuffer = new OGLFrameBuffer();
	combineFrameBuffer->Bind();
	combineFrameBuffer->AddTexture(colourOutTex);
	combineFrameBuffer->DrawBuffers();
	combineFrameBuffer->Unbind();

	quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-1,  1, -1),
		Vector3(-1, -1, -1),
		Vector3( 1, -1, -1),
		Vector3( 1,  1, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 1),
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->UploadToGPU();

	downsampleShader = new OGLShader("downsample.vert", "downsample.frag");
	upsampleShader   = new OGLShader("upsample.vert", "upsample.frag");
	combineShader    = new OGLShader("bloomCombine.vert", "bloomCombine.frag");

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
	for (auto& mip : mipChain) {
		delete mip.texture;
	}
	mipChain.clear();
	delete colourOutTex;

	delete bloomFrameBuffer;
	delete combineFrameBuffer;

	delete quad;

	delete downsampleShader;
	delete upsampleShader;
	delete combineShader;
}

void BloomRPass::OnWindowResize(int width, int height) {
	RenderPassBase::OnWindowResize(width, height);
	SetBloomDepth(bloomDepth);
}

void BloomRPass::Render() {
	bloomFrameBuffer->Bind();
	Downsample();
	Upsample();
	bloomFrameBuffer->Unbind();

	renderer.GetConfig().ResetViewport();
	Combine();
}

void BloomRPass::SetBloomDepth(size_t depth) {
	bloomDepth = depth;
	if (!mipChain.empty()) {
		for (auto& mip : mipChain) {
			delete mip.texture;
		}
		mipChain.clear();
	}
	float mipWidth  = (float)renderer.GetWidth();
	float mipHeight = (float)renderer.GetHeight();
	for (size_t i = 0; i < bloomDepth; i++) {
		mipWidth *= 0.5f;
		mipHeight *= 0.5f;

		BloomMip mip{
			mipWidth, mipHeight,
			new OGLTexture(mipWidth, mipHeight, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT)
		};
		mip.texture->Bind();
		mip.texture->SetEdgeClamp();
		mip.texture->SetFilters(GL_LINEAR, GL_LINEAR);

		mipChain.emplace_back(mip);
	}
}

void BloomRPass::SetBias(float bias) {
	combineShader->Bind();

	combineShader->SetUniformFloat("bias", bias);

	combineShader->Unbind();
}

void BloomRPass::Downsample() {
	downsampleShader->Bind();

	sceneTexIn->Bind(0);

	for (auto mip = mipChain.begin(); mip != mipChain.end(); mip++) {
		renderer.GetConfig().SetViewport(0, 0, mip->width, mip->height);
		bloomFrameBuffer->AddTexture(mip->texture, 0);

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

		renderer.GetConfig().SetViewport(0, 0, nextMip->width, nextMip->height);
		bloomFrameBuffer->AddTexture(nextMip->texture, 0);

		quad->Draw();
	}

	renderer.GetConfig().ResetBlend();

	upsampleShader->Unbind();
}

void BloomRPass::Combine() {
	combineFrameBuffer->Bind();
	combineShader->Bind();

	sceneTexIn->Bind(0);
	std::prev(mipChain.end())->texture->Bind(1);

	quad->Draw();

	combineShader->Unbind();
	combineFrameBuffer->Unbind();
}
