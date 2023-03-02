/**
 * @file   PresentRPass.cpp
 * @brief  See PresentRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "PresentRPass.h"

#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"

using namespace NCL::CSC8503;

PresentRPass::PresentRPass(OGLRenderer& renderer) :
OGLPresentRenderPass(renderer) {
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

	shader = new OGLShader("present.vert", "present.frag");
}

PresentRPass::~PresentRPass() {
	delete quad;

	delete shader;
}

void PresentRPass::Render() {
	shader->Bind();

	sceneTexIn->Bind(0);

	quad->Draw();

	shader->Unbind();
}

void PresentRPass::SetGamma(float gamma) {
	shader->Bind();

	shader->SetUniformFloat("gamma", gamma);

	shader->Unbind();
}