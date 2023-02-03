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

using namespace NCL::CSC8503;

SkyboxRPass::SkyboxRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
OGLRenderPass(renderer), gameWorld(gameWorld) {
	quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-1,  1, -1),
		Vector3(-1, -1, -1),
		Vector3( 1, -1, -1),
		Vector3( 1,  1, -1)
	});
	quad->SetVertexIndices({0, 1, 2, 2, 3, 0});
	quad->UploadToGPU();

	shader = new OGLShader("skybox.vert", "skybox.frag");

	shader->Bind();

	glUniform3f(glGetUniformLocation(shader->GetProgramID(), "sunDir"), 0.1f, 0.6f, 0.2f);
	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "cirrus"), 0.5f);
	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "cumulus"), 0.5f);

	shader->Unbind();
}

SkyboxRPass::~SkyboxRPass() {
	delete quad;
	delete shader;
}

void SkyboxRPass::PreRender() {
}

void SkyboxRPass::Render() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = renderer.GetAspect();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	shader->Bind();

	int projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
	int texLocation = glGetUniformLocation(shader->GetProgramID(), "cubeTex");

	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "time"), gameWorld.GetRunTime());

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	quad->Draw();

	shader->Unbind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}
