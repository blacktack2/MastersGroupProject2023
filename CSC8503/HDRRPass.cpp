/**
 * @file   HDRRPass.cpp
 * @brief  See HDRRPass.h
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "HDRRPass.h"

#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"

using namespace NCL::CSC8503;

HDRRPass::HDRRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn) :
OGLPostRenderPass(renderer), sceneTexIn(sceneTexIn) {
	sceneOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGBA16F);
	AddScreenTexture(sceneOutTex);

	frameBuffer = new OGLFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(sceneOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

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

	shader = new OGLShader("hdr.vert", "hdr.frag");

	shader->Bind();

	exposureUniform = glGetUniformLocation(shader->GetProgramID(), "exposure");

	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "sceneTex"), 0);

	shader->Unbind();
}

HDRRPass::~HDRRPass() {
	delete sceneOutTex;

	delete frameBuffer;

	delete quad;

	delete shader;
}

void HDRRPass::Render() {
	frameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	shader->Bind();

	sceneTexIn->Bind(0);

	quad->Draw();

	shader->Unbind();
	frameBuffer->Unbind();
}

void HDRRPass::SetExposure(float exposure) {
	shader->Bind();

	glUniform1f(exposureUniform, exposure);

	shader->Unbind();
}
