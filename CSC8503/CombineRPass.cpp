/**
 * @file   GBufferRPass.cpp
 * @brief  See GBufferRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "CombineRPass.h"

#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"

using namespace NCL::CSC8503;

CombineRPass::CombineRPass(OGLRenderer& renderer,
	OGLTexture* skyboxTexIn, OGLTexture* diffuseTexIn,
	OGLTexture* diffuseLightTexIn, OGLTexture* specularLightTexIn,
	OGLTexture* normalTexIn, OGLTexture* depthTexIn) :
OGLCombineRenderPass(renderer), skyboxTexIn(skyboxTexIn), diffuseTexIn(diffuseTexIn),
diffuseLightTexIn(diffuseLightTexIn), specularLightTexIn(specularLightTexIn),
normalTexIn(normalTexIn), depthTexIn(depthTexIn) {
	sceneOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGB16F);
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

	shader = new OGLShader("combine.vert", "combine.frag");

	shader->Bind();

	modeUniform = glGetUniformLocation(shader->GetProgramID(), "mode");
	ambienceColourUniform = glGetUniformLocation(shader->GetProgramID(), "ambienceColour");

	glUniform1i(modeUniform, 0);
	glUniform3f(ambienceColourUniform, 0.01f, 0.01f, 0.01f);

	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "skyboxTex"       ), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "diffuseTex"      ), 1);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "diffuseLightTex" ), 2);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "specularLightTex"), 3);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "normalTex"       ), 4);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "depthTex"        ), 5);

	shader->Unbind();
}

CombineRPass::~CombineRPass() {
	delete sceneOutTex;

	delete frameBuffer;

	delete quad;

	delete shader;
}

void CombineRPass::Render() {
	frameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	shader->Bind();

	skyboxTexIn->Bind(0);
	diffuseTexIn->Bind(1);
	diffuseLightTexIn->Bind(2);
	specularLightTexIn->Bind(3);
	normalTexIn->Bind(4);
	depthTexIn->Bind(5);

	quad->Draw();

	shader->Unbind();
	frameBuffer->Unbind();
}

void CombineRPass::SetRenderMode(RenderMode mode) {
	shader->Bind();

	glUniform1i(modeUniform, (GLint)mode);

	shader->Unbind();
}
