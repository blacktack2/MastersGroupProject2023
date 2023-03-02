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
OGLMainRenderPass(renderer), gameWorld(gameWorld) {
	colourOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGB16F);
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

	shader->SetUniformFloat("sunDir", 0.1f, 0.6f, 0.2f);

	shader->SetUniformFloat("cirrus", 0.5f);
	shader->SetUniformFloat("cumulus", 0.5f);

	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix();
	shader->SetUniformMatrix("projMatrix", projMatrix);

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
	renderer.ClearBuffers(ClearBit::Color);

	shader->Bind();

	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	shader->SetUniformMatrix("viewMatrix", viewMatrix);

	shader->SetUniformFloat("time", gameWorld.GetRunTime());

	quad->Draw();

	shader->Unbind();

	frameBuffer->Unbind();
}

void SkyboxRPass::SetSunDir(const Vector3& direction) {
	shader->Bind();

	shader->SetUniformFloat("sunDir", direction);

	shader->Unbind();
}
