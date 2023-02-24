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

	defaultModelShader = new OGLShader("modelDefault.vert", "modelDefault.frag");
	AddModelShader(defaultModelShader);
}

ModelRPass::~ModelRPass() {
	delete frameBuffer;

	delete diffuseOutTex;
	delete normalOutTex;
	delete depthOutTex;

	delete defaultModelShader;
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
		const RenderObject* renderObject = gameObject->GetRenderObject();
		if (!renderObject) {
			return;
		}

		if (renderObject->HasAnimation()) {
			OGLShader* defaultAnimShader = static_cast<OGLShader*>(AssetLibrary::GetShader("animation"));
			AnimatedRenderObject* renderObj = (AnimatedRenderObject*)renderObject;

			renderObj->SetFrameTime(renderObj->GetFrameTime() - gameWorld.GetDeltaTime() * renderObj->GetAnimSpeed());
			while (renderObj->GetFrameTime() < 0.0f) {
				renderObj->SetCurrentFrame((renderObj->GetCurrentFrame() + 1) % renderObj->GetAnimation()->GetFrameCount());
				renderObj->SetNextFrame((renderObj->GetCurrentFrame() + 1) % renderObj->GetAnimation()->GetFrameCount());
				renderObj->SetFrameTime(renderObj->GetFrameTime() + (1.0f / renderObj->GetAnimation()->GetFrameRate()));
			}

			defaultAnimShader->Bind();

			defaultAnimShader->SetUniformInt("diffuseTex", 0);

			Matrix4 modelMatrix = renderObject->GetTransform()->GetGlobalMatrix();
			defaultAnimShader->SetUniformMatrix("modelMatrix", modelMatrix);

			const Matrix4* bindPose = renderObj->GetAnimatedMesh()->GetBindPose().data();
			const Matrix4* invBindPose = renderObj->GetAnimatedMesh()->GetInverseBindPose().data();
			const Matrix4* frameData = renderObj->GetAnimation()->GetJointData(renderObj->GetCurrentFrame());
			const int* bindPoseIndices = renderObj->GetAnimatedMesh()->GetBindPoseIndices();

			for (int i = 0; i < renderObj->GetAnimatedMesh()->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, (renderObj->GetMatTextures())[i]);
				vector<Matrix4> frameMatrices;
				for (unsigned int i = 0; i < renderObj->GetAnimatedMesh()->GetJointCount(); ++i) {
					Matrix4 mat = frameData[i] * invBindPose[i];

					frameMatrices.emplace_back(mat);
				}
				defaultAnimShader->SetUniformMatrix("joints", frameMatrices.size(), frameMatrices.data());
				renderObj->GetAnimatedMesh()->Draw(i);
			}
			((OGLShader*)AssetLibrary::GetShader("animation"))->Unbind();
		} else {
			OGLMesh* mesh;
			if (!(mesh = dynamic_cast<OGLMesh*>(renderObject->GetMesh()))) {
				return;
			}
			OGLShader* shader;
			if (!(shader = dynamic_cast<OGLShader*>(renderObject->GetShader()))) {
				shader = defaultModelShader;
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
			shader->SetUniformMatrix("modelMatrix", modelMatrix);

			shader->SetUniformFloat("modelColour", colour);

			renderObject->ConfigerShaderExtras(shader);

			int layerCount = mesh->GetSubMeshCount();
			for (int i = 0; i < layerCount; i++) {
				mesh->Draw(i);
			}
			shader->Unbind();
		}
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
