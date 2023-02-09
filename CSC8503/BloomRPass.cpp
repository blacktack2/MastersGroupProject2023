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

BloomRPass::BloomRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn, OGLTexture* bloomTexIn) :
OGLRenderPass(renderer), sceneTexIn(sceneTexIn), bloomTexIn(bloomTexIn) {
	blurTexA = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	AddScreenTexture(blurTexA);
	blurTexB = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	AddScreenTexture(blurTexB);
	colourOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	AddScreenTexture(colourOutTex);

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
		Vector3(1, -1, -1),
		Vector3(1,  1, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 1),
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->UploadToGPU();

	blurShader = new OGLShader("blur.vert", "blur.frag");
	combineShader = new OGLShader("bloomCombine.vert", "bloomCombine.frag");

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
	delete colourOutTex;

	delete blurFrameBufferA;
	delete blurFrameBufferB;

	delete quad;

	delete blurShader;
	delete combineShader;
}

void BloomRPass::Render() {
	ApplyBlur();
	Combine();
}

void BloomRPass::ApplyBlur() {
	blurShader->Bind();

	for (size_t i = 0; i < blurAmount; i++) {
		blurFrameBufferA->Bind();
		if (i == 0)
			bloomTexIn->Bind(0);
		else
			blurTexB->Bind(0);

		glUniform1i(horizontalUniform, GL_FALSE);

		quad->Draw();

		blurFrameBufferB->Bind();
		blurTexA->Bind(0);

		glUniform1i(horizontalUniform, GL_TRUE);

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
