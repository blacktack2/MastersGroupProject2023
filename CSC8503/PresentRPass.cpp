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

PresentRPass::PresentRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn) :
OGLRenderPass(renderer), sceneTexIn(sceneTexIn) {
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	shader->Bind();

	sceneTexIn->Bind(0);

	quad->Draw();

	shader->Unbind();
	//glDisable(GL_FRAMEBUFFER_SRGB);
}
