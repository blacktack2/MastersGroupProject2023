/**
 * @file   ModelRPass.cpp
 * @brief  See ModelRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "ModelRPass.h"

#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "RenderObject.h"
#include "AnimatedRenderObject.h"
#include "AssetLibrary.h"
#include "Assets.h"

using namespace NCL::CSC8503;

ModelRPass::ModelRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
OGLMainRenderPass(renderer), gameWorld(gameWorld) {
	diffuseOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	normalOutTex  = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	depthOutTex   = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), TexType::Depth);
	AddScreenTexture(diffuseOutTex);
	AddScreenTexture(normalOutTex);
	AddScreenTexture(depthOutTex);

	frameBuffer = new OGLFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(diffuseOutTex);
	frameBuffer->AddTexture(normalOutTex);
	frameBuffer->AddTexture(depthOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	AddModelShader((OGLShader*)AssetLibrary::GetShader("defaultModel"));
}

ModelRPass::~ModelRPass() {
	delete frameBuffer;

	delete diffuseOutTex;
	delete normalOutTex;
	delete depthOutTex;
}

void ModelRPass::Render() {
	frameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (OGLShader* shader : modelShaders) {
		shader->Bind();

		Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
		float screenAspect = (float)renderer.GetWidth() / (float)renderer.GetHeight();
		Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

		shader->SetUniformMatrix("viewMatrix", viewMatrix);
		shader->SetUniformMatrix("projMatrix", projMatrix);
		shader->SetUniformFloat("gamma", gamma);

		shader->Unbind();
	}
	
	gameWorld.OperateOnContents([&](GameObject* gameObject) {
		RenderObject* renderObject = gameObject->GetRenderObject();
		if (!renderObject) {
			return;
		}

		if (!dynamic_cast<OGLMesh*>(renderObject->GetMesh())) {
			return;
		}

		renderObject->Draw();
	});

	frameBuffer->Unbind();
}

void ModelRPass::AddModelShader(OGLShader* shader) {
	modelShaders.push_back(shader);
	shader->Bind();

	shader->SetUniformInt("diffuseTex", 0);
	shader->SetUniformInt("bumpTex"   , 1);

	shader->Unbind();
}
