/**
 * @file   SSAORPass.cpp
 * @brief  See SSAORPass.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "SSAORPass.h"

#include "OGLBufferObject.h"
#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"

using namespace NCL::CSC8503;

SSAORPass::SSAORPass(OGLRenderer& renderer, OGLTexture* depthTexIn, OGLTexture* normalTexIn) :
OGLMainRenderPass(renderer), depthTexIn(depthTexIn), normalTexIn(normalTexIn) {
	ssaoTex    = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	ssaoOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	AddScreenTexture(ssaoTex);
	AddScreenTexture(ssaoOutTex);

	ssaoFrameBuffer = new OGLFrameBuffer();
	ssaoFrameBuffer->Bind();
	ssaoFrameBuffer->AddTexture(ssaoTex);
	ssaoFrameBuffer->DrawBuffers();
	ssaoFrameBuffer->Unbind();

	blurFrameBuffer = new OGLFrameBuffer();
	blurFrameBuffer->Bind();
	blurFrameBuffer->AddTexture(ssaoOutTex);
	blurFrameBuffer->DrawBuffers();
	blurFrameBuffer->Unbind();

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

	ssaoShader = new OGLShader("ssao.vert", "ssao.frag");
	blurShader = new OGLShader("ssaoBlur.vert", "ssaoBlur.frag");

	ssaoShader->Bind();

	viewMatrixUniform = glGetUniformLocation(ssaoShader->GetProgramID(), "viewMatrix");
	projMatrixUniform = glGetUniformLocation(ssaoShader->GetProgramID(), "projMatrix");
	noiseScaleUniform = glGetUniformLocation(ssaoShader->GetProgramID(), "noiseScale");

	radiusUniform = glGetUniformLocation(ssaoShader->GetProgramID(), "radius");
	biasUniform   = glGetUniformLocation(ssaoShader->GetProgramID(), "bias");

	glUniform1i(glGetUniformLocation(ssaoShader->GetProgramID(), "depthTex"), 0);
	glUniform1i(glGetUniformLocation(ssaoShader->GetProgramID(), "normalTex"), 1);
	glUniform1i(glGetUniformLocation(ssaoShader->GetProgramID(), "noiseTex"), 2);

	glUniform2f(noiseScaleUniform, (float)renderer.GetWidth() / (float)noiseTexSize, (float)renderer.GetHeight() / (float)noiseTexSize);

	ssaoShader->Unbind();
	blurShader->Bind();

	glUniform1i(glGetUniformLocation(blurShader->GetProgramID(), "ssaoTex"), 0);

	blurShader->Unbind();

	kernelSSBO = new OGLBufferObject(numKernels, 1);
	GenerateKernels();
	GenerateNoiseTex();
}

SSAORPass::~SSAORPass() {
	delete ssaoFrameBuffer;
	delete blurFrameBuffer;

	delete ssaoTex;
	delete noiseTex;
	delete ssaoOutTex;

	delete quad;

	delete ssaoShader;
	delete blurShader;
}

void SSAORPass::OnWindowResize(int width, int height) {
	ssaoShader->Bind();

	glUniform2f(noiseScaleUniform, (float)width / (float)noiseTexSize, (float)height / (float)noiseTexSize);

	ssaoShader->Unbind();
}

void SSAORPass::Render() {
	/*glDisable(GL_BLEND);
	DrawSSAO();
	BlurSSAO();
	glEnable(GL_BLEND);*/
}

void SSAORPass::SetRadius(float radius) {
	ssaoShader->Bind();

	glUniform1f(radiusUniform, radius);

	ssaoShader->Unbind();
}

void SSAORPass::SetBias(float bias) {
	ssaoShader->Bind();

	glUniform1f(biasUniform, bias);

	ssaoShader->Unbind();
}

void SSAORPass::SetNumKernels(size_t num) {
	numKernels = num;
	GenerateKernels();
}

void SSAORPass::DrawSSAO() {
	ssaoFrameBuffer->Bind();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	ssaoShader->Bind();

	depthTexIn->Bind(0);
	normalTexIn->Bind(1);
	noiseTex->Bind(2);

	Matrix4 viewMatrix = GameWorld::instance().GetMainCamera()->BuildViewMatrix();
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, (GLfloat*)viewMatrix.array);
	Matrix4 projMatrix = GameWorld::instance().GetMainCamera()->BuildProjectionMatrix();
	glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE, (GLfloat*)projMatrix.array);

	quad->Draw();

	ssaoShader->Unbind();
	ssaoFrameBuffer->Unbind();
}

void SSAORPass::BlurSSAO() {
	blurFrameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	blurShader->Bind();

	ssaoTex->Bind(0);

	quad->Draw();

	blurShader->Unbind();
	blurFrameBuffer->Unbind();
}

void SSAORPass::GenerateKernels() {
	kernelSSBO->Bind();
	if (kernels.size() != numKernels) {
		kernels.resize(numKernels);
		kernelSSBO->Resize(numKernels * sizeof(Vector3));
	}

	for (size_t i = 0; i < numKernels; i++) {
		Vector3 sample = Vector3(
			random(generator) * 2.0f - 1.0f,
			random(generator) * 2.0f - 1.0f,
			random(generator)
		);

		sample.Normalise();
		sample *= random(generator);
		
		float scale = (float)i / (float)numKernels;
		scale = 0.1f + (scale * scale) * 0.9f;
		sample *= scale;

		kernels[i] = sample;
	}

	kernelSSBO->Write(kernels.data());

	kernelSSBO->Unbind();
}

void SSAORPass::GenerateNoiseTex() {
	std::vector<Vector3> noiseData;
	noiseData.resize(noiseTexSize * noiseTexSize);

	for (size_t i = 0; i < noiseTexSize * noiseTexSize; i++) {
		Vector3 noise = Vector3(
			random(generator) * 2.0f - 1.0f,
			random(generator) * 2.0f - 1.0f,
			0.0f
		);
		noiseData[i] = noise;
	}

	if (noiseTex) {
		delete noiseTex;
	}
	noiseTex = new OGLTexture(noiseTexSize, noiseTexSize, GL_RGB16F, GL_RGB, GL_FLOAT, noiseData.data());
	noiseTex->Bind();

	noiseTex->SetEdgeRepeat();

	noiseTex->Unbind();
}
