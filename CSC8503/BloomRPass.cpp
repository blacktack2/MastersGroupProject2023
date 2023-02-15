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

BloomRPass::BloomRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn) :
	OGLPostRenderPass(renderer), sceneTexIn(sceneTexIn) {
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

	sourcePixelSizeUniform = glGetUniformLocation(downsampleShader->GetProgramID(), "sourcePixelSize");

	glUniform1i(glGetUniformLocation(downsampleShader->GetProgramID(), "sourceTex"), 0);

	downsampleShader->Unbind();

	upsampleShader->Bind();

	filterRadiusUniform = glGetUniformLocation(upsampleShader->GetProgramID(), "filterRadius");

	glUniform1i(glGetUniformLocation(upsampleShader->GetProgramID(), "sourceTex"), 0);

	upsampleShader->Unbind();

	combineShader->Bind();

	biasUniform = glGetUniformLocation(combineShader->GetProgramID(), "bias");

	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "sceneTex"), 0);
	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "bloomTex"), 1);

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

	glViewport(0, 0, (GLsizei)renderer.GetWidth(), (GLsizei)renderer.GetHeight());
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
			new OGLTexture(mip.width, mip.height, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT)
		};
		mip.texture->Bind();
		mip.texture->SetEdgeClamp();
		mip.texture->SetFilters(GL_LINEAR, GL_LINEAR);

		mipChain.emplace_back(mip);
	}
}

void BloomRPass::SetBias(float bias) {
	combineShader->Bind();

	glUniform1f(biasUniform, bias);

	combineShader->Unbind();
}

void BloomRPass::Downsample() {
	downsampleShader->Bind();

	sceneTexIn->Bind(0);

	for (auto mip = mipChain.begin(); mip != mipChain.end(); mip++) {
		glViewport(0, 0, mip->width, mip->height);
		bloomFrameBuffer->AddTexture(mip->texture, 0);

		quad->Draw();

		glUniform2f(sourcePixelSizeUniform, 1.0f / mip->width, 1.0f / mip->height);
		mip->texture->Bind(0);
	}

	downsampleShader->Unbind();
}

void BloomRPass::Upsample() {
	upsampleShader->Bind();

	glBlendFunc(GL_ONE, GL_ONE);

	for (auto mip = mipChain.rbegin(); mip != std::prev(mipChain.rend()); mip++) {
		auto nextMip = std::next(mip);

		mip->texture->Bind(0);

		glViewport(0, 0, nextMip->width, nextMip->height);
		bloomFrameBuffer->AddTexture(nextMip->texture, 0);

		quad->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
