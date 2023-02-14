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

using namespace NCL::CSC8503;

ModelRPass::ModelRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
OGLRenderPass(renderer), gameWorld(gameWorld) {
	diffuseOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	normalOutTex  = new OGLTexture(renderer.GetWidth(), renderer.GetHeight());
	depthOutTex   = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), TexType::Depth);
	AddScreenTexture(diffuseOutTex);
	AddScreenTexture(normalOutTex);
	AddScreenTexture(depthOutTex);

	defaultDiffuse = (OGLTexture*)OGLTexture::RGBATextureFromFilename("GoatBody.png");
	defaultBump = (OGLTexture*)OGLTexture::RGBATextureFromFilename("DefaultBump.png");
	defaultBump->Bind();
	defaultBump->SetFilters(GL_LINEAR, GL_LINEAR);
	defaultBump->Unbind();

	frameBuffer = new OGLFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(diffuseOutTex);
	frameBuffer->AddTexture(normalOutTex);
	frameBuffer->AddTexture(depthOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	defaultShader = new OGLShader("modelDefault.vert", "modelDefault.frag");
	AddModelShader(defaultShader);
}

ModelRPass::~ModelRPass() {
	delete frameBuffer;

	delete diffuseOutTex;
	delete normalOutTex;
	delete depthOutTex;

	delete defaultShader;
}

void ModelRPass::Render() {
	frameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (OGLShader* shader : modelShaders) {
		shader->Bind();

		Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
		float screenAspect = (float)renderer.GetWidth() / (float)renderer.GetHeight();
		Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
		// TODO - Replace with call to the shader class
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrix"), 1, GL_FALSE, (GLfloat*)&viewMatrix);
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projMatrix"), 1, GL_FALSE, (GLfloat*)&projMatrix);
		glUniform1f(glGetUniformLocation(shader->GetProgramID(), "gamma"), gamma);

		shader->Unbind();
	}
	
	gameWorld.OperateOnContents([&](GameObject* gameObject) {
		const RenderObject* renderObject = gameObject->GetRenderObject();
		if (!renderObject) {
			return;
		}
		OGLMesh* mesh;
		if (!(mesh = dynamic_cast<OGLMesh*>(renderObject->GetMesh()))) {
			return;
		}
		OGLShader* shader;
		if (!(shader = dynamic_cast<OGLShader*>(renderObject->GetShader()))) {
			shader = defaultShader;
		}

		OGLTexture* diffuseTex;
		if (!(diffuseTex = dynamic_cast<OGLTexture*>(renderObject->GetDefaultTexture()))) {
			diffuseTex = defaultDiffuse;
		}
		OGLTexture* bumpTex;
		if (true) {
			bumpTex = defaultBump;
		}

		Vector4 colour = renderObject->GetColour();

		shader->Bind();

		diffuseTex->Bind(0);
		bumpTex->Bind(1);

		Matrix4 modelMatrix = renderObject->GetTransform()->GetGlobalMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "modelMatrix"), 1, GL_FALSE, (GLfloat*)&modelMatrix);
		glUniform4fv(glGetUniformLocation(shader->GetProgramID(), "modelColour"), 1, (GLfloat*)colour.array);

		mesh->Draw();

		shader->Unbind();
	});

	frameBuffer->Unbind();
}

void ModelRPass::AddModelShader(OGLShader* shader) {
	modelShaders.push_back(shader);
	shader->Bind();

	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "bumpTex"), 1);

	shader->Unbind();
}
