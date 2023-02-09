/**
 * @file   SkyboxRPass.cpp
 * @brief  See SkyboxRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "SkyboxRPass.h"

#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"

using namespace NCL::CSC8503;

SkyboxRPass::SkyboxRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
OGLRenderPass(renderer), gameWorld(gameWorld) {
	colourOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	AddScreenTexture(colourOutTex);

	frameBuffer = new OGLFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(colourOutTex);
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
	quad->SetVertexIndices({0, 1, 2, 2, 3, 0});
	quad->UploadToGPU();

	shader = new OGLShader("skybox.vert", "skybox.frag");

	shader->Bind();

	// TODO - Replace with call to the shader class
	viewMatrixUniform = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
	sunDirUniform = glGetUniformLocation(shader->GetProgramID(), "sunDir");
	timeUniform = glGetUniformLocation(shader->GetProgramID(), "time");

	glUniform3f(sunDirUniform, 0.1f, 0.6f, 0.2f);
	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "cirrus"), 0.5f);
	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "cumulus"), 0.5f);
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projMatrix"), 1, GL_FALSE, (GLfloat*)projMatrix.array);

	shader->Unbind();
}

SkyboxRPass::~SkyboxRPass() {
	delete frameBuffer;

	delete colourOutTex;

	delete quad;

	delete shader;
}

void SkyboxRPass::Render() {
	frameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	shader->Bind();

	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, (GLfloat*)viewMatrix.array);
	glUniform1f(timeUniform, gameWorld.GetRunTime());

	quad->Draw();

	shader->Unbind();

	glDepthMask(GL_TRUE);
	frameBuffer->Unbind();
}

void SkyboxRPass::SetSunDir(const Vector3& direction) {
	shader->Bind();
	glUniform3fv(sunDirUniform, 1, (GLfloat*)&direction);
	shader->Unbind();
}
