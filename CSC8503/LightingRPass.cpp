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
OGLRenderPass(renderer), gameWorld(gameWorld), depthTexIn(depthTexIn), normalTexIn(normalTexIn) {
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

	cameraPosUniform       = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
	pixelSizeUniform       = glGetUniformLocation(shader->GetProgramID(), "pixelSize");
	inverseProjViewUniform = glGetUniformLocation(shader->GetProgramID(), "inverseProjView");

	lightPositionUniform   = glGetUniformLocation(shader->GetProgramID(), "lightPosition");
	lightColourUniform     = glGetUniformLocation(shader->GetProgramID(), "lightColour");
	lightRadiusUniform     = glGetUniformLocation(shader->GetProgramID(), "lightRadius");
	lightDirectionUniform  = glGetUniformLocation(shader->GetProgramID(), "lightDirection");
	lightAngleUniform      = glGetUniformLocation(shader->GetProgramID(), "lightAngle");

	modelMatrixUniform     = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
	viewMatrixUniform      = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
	projMatrixUniform      = glGetUniformLocation(shader->GetProgramID(), "projMatrix");

	glUniform2f(pixelSizeUniform, 1.0f / (float)renderer.GetWidth(), 1.0f / (float)renderer.GetHeight());
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "depthTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "normalTex"), 1);

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
	glUniform2f(pixelSizeUniform, 1.0f / (float)width, 1.0f / (float)height);
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

	glBlendFunc(GL_ONE, GL_ONE);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	depthTexIn->Bind(0);
	normalTexIn->Bind(1);

	glUniform3fv(cameraPosUniform, 1, (GLfloat*)&gameWorld.GetMainCamera()->GetPosition()[0]);
	Matrix4 modelMatrix = Matrix4();
	modelMatrix.SetDiagonal(Vector3(1));
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 inverseViewProj = (projMatrix * viewMatrix).Inverse();

	glUniform4fv(lightPositionUniform, 1, (GLfloat*)&light.position);
	glUniform4fv(lightColourUniform, 1, (GLfloat*)&light.colour);
	glUniform1f(lightRadiusUniform, light.radius);
	glUniform3fv(lightDirectionUniform, 1, (GLfloat*)&light.direction);
	glUniform1f(lightAngleUniform, light.angle);

	glUniformMatrix4fv(inverseProjViewUniform, 1, GL_FALSE, (GLfloat*)&inverseViewProj);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE, (GLfloat*)projMatrix.array);

	if (light.position.w == 0.0f) {
		quad->Draw();
	} else {
		glCullFace(GL_FRONT);
		sphere->Draw();
		glCullFace(GL_BACK);
	}

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->Unbind();
	frameBuffer->Unbind();
}
