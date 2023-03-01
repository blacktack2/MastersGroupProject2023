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
	OGLTexture* diffuseLightTexIn, OGLTexture* specularLightTexIn, OGLTexture* ssaoTexIn,
	OGLTexture* normalTexIn, OGLTexture* depthTexIn) :
OGLCombineRenderPass(renderer), skyboxTexIn(skyboxTexIn), diffuseTexIn(diffuseTexIn),
diffuseLightTexIn(diffuseLightTexIn), specularLightTexIn(specularLightTexIn), ssaoTexIn(ssaoTexIn),
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

	shader->SetUniformInt("mode", 0);
	shader->SetUniformFloat("ambienceColour", 0.1f, 0.1f, 0.1f);

	shader->SetUniformInt("skyboxTex", 0);
	shader->SetUniformInt("diffuseTex", 1);
	shader->SetUniformInt("diffuseLightTex", 2);
	shader->SetUniformInt("specularLightTex", 3);
	shader->SetUniformInt("ssaoTex", 4);
	shader->SetUniformInt("normalTex", 5);
	shader->SetUniformInt("depthTex", 6);

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
	renderer.ClearBuffers(ClearBit::Color);
	shader->Bind();

	skyboxTexIn->Bind(0);
	diffuseTexIn->Bind(1);
	diffuseLightTexIn->Bind(2);
	specularLightTexIn->Bind(3);
	ssaoTexIn->Bind(4);
	normalTexIn->Bind(5);
	depthTexIn->Bind(6);

	quad->Draw();

	shader->Unbind();
	frameBuffer->Unbind();
}

void CombineRPass::SetRenderMode(RenderMode mode) {
	shader->Bind();

	shader->SetUniformInt("mode", (int)mode);

	shader->Unbind();
}
