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

using namespace NCL::CSC8503;

BloomRPass::BloomRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn) :
OGLRenderPass(renderer), sceneTexIn(sceneTexIn) {
	filterTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGBA16F);
	AddScreenTexture(filterTex);
	blurTexA = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGBA16F);
	AddScreenTexture(blurTexA);
	blurTexB = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGBA16F);
	AddScreenTexture(blurTexB);
	colourOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGBA16F);
	AddScreenTexture(colourOutTex);

	filterFrameBuffer = new OGLFrameBuffer();
	filterFrameBuffer->Bind();
	filterFrameBuffer->AddTexture(filterTex);
	filterFrameBuffer->DrawBuffers();
	filterFrameBuffer->Unbind();

	blurFrameBufferA = new OGLFrameBuffer();
	blurFrameBufferA->Bind();
	blurFrameBufferA->AddTexture(blurTexA);
	blurFrameBufferA->DrawBuffers();
	blurFrameBufferA->Unbind();

	blurFrameBufferB = new OGLFrameBuffer();
	blurFrameBufferB->Bind();
	blurFrameBufferB->AddTexture(blurTexB);
	blurFrameBufferB->DrawBuffers();
	blurFrameBufferB->Unbind();

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

	filterShader  = new OGLShader("bloomFilter.vert", "bloomFilter.frag");
	blurShader    = new OGLShader("blur.vert", "blur.frag");
	combineShader = new OGLShader("bloomCombine.vert", "bloomCombine.frag");

	filterShader->Bind();

	thresholdUniform = glGetUniformLocation(filterShader->GetProgramID(), "threshold");

	glUniform1i(glGetUniformLocation(filterShader->GetProgramID(), "sceneTex"), 0);

	filterShader->Unbind();

	blurShader->Bind();

	horizontalUniform = glGetUniformLocation(blurShader->GetProgramID(), "horizontal");

	glUniform1i(horizontalUniform, 0);

	glUniform1i(glGetUniformLocation(blurShader->GetProgramID(), "sceneTex"), 0);

	blurShader->Unbind();

	combineShader->Bind();

	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "sceneTex"), 0);
	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "bloomTex"), 1);

	combineShader->Unbind();
}

BloomRPass::~BloomRPass() {
	delete filterTex;
	delete blurTexA;
	delete blurTexB;
	delete colourOutTex;

	delete filterFrameBuffer;
	delete blurFrameBufferA;
	delete blurFrameBufferB;
	delete combineFrameBuffer;

	delete quad;

	delete filterShader;
	delete blurShader;
	delete combineShader;
}

void BloomRPass::Render() {
	DrawFilter();
	ApplyBlur();
	Combine();
}

void BloomRPass::SetThreshold(float threshold) {
	filterShader->Bind();

	glUniform1f(thresholdUniform, threshold);

	filterShader->Unbind();
}

void BloomRPass::DrawFilter() {
	filterFrameBuffer->Bind();
	filterShader->Bind();

	sceneTexIn->Bind(0);

	quad->Draw();

	filterShader->Unbind();
	filterFrameBuffer->Unbind();
}

void BloomRPass::ApplyBlur() {
	blurShader->Bind();

	for (size_t i = 0; i < blurAmount; i++) {
		bool horizontal = i & 1;
		(horizontal ? blurFrameBufferB : blurFrameBufferA)->Bind();
		(i == 0 ? filterTex : (horizontal ? blurTexA : blurTexB))->Bind(0);
		glUniform1i(horizontalUniform, horizontal);
		quad->Draw();
	}

	blurFrameBufferB->Unbind();
	blurShader->Unbind();
}

void BloomRPass::Combine() {
	combineFrameBuffer->Bind();
	combineShader->Bind();

	sceneTexIn->Bind(0);
	blurTexB->Bind(1);

	quad->Draw();

	combineShader->Unbind();
	combineFrameBuffer->Unbind();
}
