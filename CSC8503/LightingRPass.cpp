/**
 * @file   LightingRPass.cpp
 * @brief  See LightingRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "LightingRPass.h"

#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "RenderObject.h"

using namespace NCL::CSC8503;

LightingRPass::LightingRPass(OGLRenderer& renderer, GameWorld& gameWorld, OGLTexture* depthTexIn, OGLTexture* normalTexIn) :
OGLMainRenderPass(renderer), gameWorld(gameWorld), depthTexIn(depthTexIn), normalTexIn(normalTexIn) {
	lightDiffuseOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGB16F);
	lightSpecularOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), GL_RGB16F);
	AddScreenTexture(lightDiffuseOutTex);
	AddScreenTexture(lightSpecularOutTex);

	frameBuffer = new OGLFrameBuffer();
	frameBuffer->Bind();
	frameBuffer->AddTexture(lightDiffuseOutTex);
	frameBuffer->AddTexture(lightSpecularOutTex);
	frameBuffer->DrawBuffers();
	frameBuffer->Unbind();

	sphere = new OGLMesh("Sphere.msh");
	sphere->SetPrimitiveType(GeometryPrimitive::Triangles);
	sphere->UploadToGPU();

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

	shader = new OGLShader("light.vert", "light.frag");

	shader->Bind();

	shader->SetUniformFloat("pixelSize", 1.0f / (float)renderer.GetWidth(), 1.0f / (float)renderer.GetHeight());

	shader->SetUniformInt("depthTex", 0);
	shader->SetUniformInt("normalTex", 1);

	shader->Unbind();
}

LightingRPass::~LightingRPass() {
	delete frameBuffer;

	delete lightDiffuseOutTex;
	delete lightSpecularOutTex;

	delete sphere;
	delete quad;

	delete shader;
}

void LightingRPass::OnWindowResize(int width, int height) {
	RenderPassBase::OnWindowResize(width, height);
	shader->Bind();

	shader->SetUniformFloat("pixelSize", 1.0f / (float)width, 1.0f / (float)height);

	shader->Unbind();
}

void LightingRPass::Render() {
	frameBuffer->Bind();

	glClear(GL_COLOR_BUFFER_BIT);

	frameBuffer->Unbind();

	gameWorld.OperateOnLights([&](const Light& light) {
		DrawLight(light);
	});
}

void LightingRPass::DrawLight(const Light& light) {
	frameBuffer->Bind();
	shader->Bind();

	renderer.GetConfig().SetBlend(true, BlendFuncSrc::One, BlendFuncDst::One);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	depthTexIn->Bind(0);
	normalTexIn->Bind(1);

	shader->SetUniformFloat("cameraPos", gameWorld.GetMainCamera()->GetPosition());

	Matrix4 modelMatrix = Matrix4();
	modelMatrix.SetDiagonal(Vector4(1));
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 inverseViewProj = (projMatrix * viewMatrix).Inverse();

	shader->SetUniformFloat("lightPosition" , light.position);
	shader->SetUniformFloat("lightColour"   , light.colour);
	shader->SetUniformFloat("lightRadius"   , light.radius);
	shader->SetUniformFloat("lightDirection", light.direction);
	shader->SetUniformFloat("lightAngle"    , light.angle);

	shader->SetUniformMatrix("inverseProjView", inverseViewProj);
	shader->SetUniformMatrix("modelMatrix"    , modelMatrix);
	shader->SetUniformMatrix("viewMatrix"     , viewMatrix);
	shader->SetUniformMatrix("projMatrix"     , projMatrix);

	if (light.position.w == 0.0f) {
		quad->Draw();
	} else {
		renderer.GetConfig().SetCullFace(true, CullFace::Back);
		sphere->Draw();
		renderer.GetConfig().ResetCullFace();
	}

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	renderer.GetConfig().ResetBlend();

	shader->Unbind();
	frameBuffer->Unbind();
}
