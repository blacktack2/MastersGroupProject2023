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
			AnimatedRenderObject* renderObj = (AnimatedRenderObject*)renderObject;

			renderObj->SetFrameTime(renderObj->GetFrameTime() - gameWorld.GetDeltaTime() * renderObj->GetAnimSpeed());
			while (renderObj->GetFrameTime() < 0.0f) {
				renderObj->SetCurrentFrame((renderObj->GetCurrentFrame() + 1) % renderObj->GetAnimation()->GetFrameCount());
				renderObj->SetNextFrame((renderObj->GetCurrentFrame() + 1) % renderObj->GetAnimation()->GetFrameCount());
				renderObj->SetFrameTime(renderObj->GetFrameTime() + (1.0f / renderObj->GetAnimation()->GetFrameRate()));
			}

			//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			((OGLShader*)AssetLibrary::GetShader("animation"))->Bind();
			glUniform1i(glGetUniformLocation(((OGLShader*)AssetLibrary::GetShader("animation"))->GetProgramID(), "diffuseTex"), 0);

			//Matrix4 modelMatrix = Matrix4::Translation({ 0,20,0 }) * Matrix4::Scale({ 50,50,50 });
			Matrix4 modelMatrix = renderObject->GetTransform()->GetGlobalMatrix();
			glUniformMatrix4fv(glGetUniformLocation(((OGLShader*)AssetLibrary::GetShader("animation"))->GetProgramID(), "modelMatrix"), 1, GL_FALSE, (GLfloat*)&modelMatrix);

			const Matrix4* bindPose = renderObj->GetAnimatedMesh()->GetBindPose().data();
			const Matrix4* invBindPose = renderObj->GetAnimatedMesh()->GetInverseBindPose().data();
			const Matrix4* frameData = renderObj->GetAnimation()->GetJointData(renderObj->GetCurrentFrame());
			const int* bindPoseIndices = renderObj->GetAnimatedMesh()->GetBindPoseIndices();

			int j = glGetUniformLocation(((OGLShader*)AssetLibrary::GetShader("animation"))->GetProgramID(), "joints");

			for (int i = 0; i < renderObj->GetAnimatedMesh()->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, (renderObj->GetMatTextures())[i]);
				/*
				A submesh *might* have a different set of bind pose matrices,
				depending on how it has been defined. The loader stores all
				of the bind pose matrices in contiguous memory, and then uses
				the SubMeshPoses struct to indicate what the first matrix, and
				how many matrices, are used for each submesh i.
				*/

				//SubMeshPoses pose;
				//renderObj->GetAnimatedMesh()->GetBindPoseState(i, pose);

				vector<Matrix4> frameMatrices;
				for (unsigned int i = 0; i < renderObj->GetAnimatedMesh()->GetJointCount(); ++i) {			// i < pose.count;
					/*
					We can now grab the correct matrix for a given pose.
					Each matrix is relative to a given joint on the original mesh.
					We can perform the lookup for this by grabbing a set of indices
					from the mesh.
					*/
					//int jointID = bindPoseIndices[pose.start + i];

					//Matrix4 mat = frameData[jointID] * invBindPose[pose.start + i];
					Matrix4 mat = frameData[i] * invBindPose[i];

					frameMatrices.emplace_back(mat);
				}
				//Frame matrices now contains the correct set of matrices for this submesh
				glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
				renderObj->GetAnimatedMesh()->Draw(i);
			}
			((OGLShader*)AssetLibrary::GetShader("animation"))->Unbind();
		} else { // AnimatedRenderObject returns nullptr when GetMesh(), thus all AnimatedRenderObjects will return from the next block.
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
