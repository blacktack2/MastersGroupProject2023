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
	shadowTex = new OGLTexture(SHADOWSIZE, SHADOWSIZE, TexType::Shadow);

	lightDiffuseOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), TexType::Colour8);
	lightSpecularOutTex = new OGLTexture(renderer.GetWidth(), renderer.GetHeight(), TexType::Colour8);
	AddScreenTexture(lightDiffuseOutTex);
	AddScreenTexture(lightSpecularOutTex);

	shadowFrameBuffer = new OGLFrameBuffer();
	shadowFrameBuffer->Bind();
	shadowFrameBuffer->AddTexture(shadowTex);
	shadowFrameBuffer->DrawBuffers();
	shadowFrameBuffer->Unbind();

	lightFrameBuffer = new OGLFrameBuffer();
	lightFrameBuffer->Bind();
	lightFrameBuffer->AddTexture(lightDiffuseOutTex);
	lightFrameBuffer->AddTexture(lightSpecularOutTex);
	lightFrameBuffer->DrawBuffers();
	lightFrameBuffer->Unbind();

	sphere = new OGLMesh("Sphere.msh");

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

	defaultShadowShader = new OGLShader("depth.vert", "depth.frag");
	AddShadowShader(defaultShadowShader);
	lightShader = new OGLShader("light.vert", "light.frag");

	lightShader->Bind();

	cameraPosUniform       = glGetUniformLocation(lightShader->GetProgramID(), "cameraPos");
	pixelSizeUniform       = glGetUniformLocation(lightShader->GetProgramID(), "pixelSize");
	inverseProjViewUniform = glGetUniformLocation(lightShader->GetProgramID(), "inverseProjView");

	lightPositionUniform   = glGetUniformLocation(lightShader->GetProgramID(), "lightPos");
	lightColourUniform     = glGetUniformLocation(lightShader->GetProgramID(), "lightColour");
	lightRadiusUniform     = glGetUniformLocation(lightShader->GetProgramID(), "lightRadius");
	lightDirectionUniform  = glGetUniformLocation(lightShader->GetProgramID(), "lightDirection");
	lightAngleUniform      = glGetUniformLocation(lightShader->GetProgramID(), "lightAngle");

	shadowMatrixUniform    = glGetUniformLocation(lightShader->GetProgramID(), "shadowMatrix");
	viewMatrixUniform      = glGetUniformLocation(lightShader->GetProgramID(), "viewMatrix");
	projMatrixUniform      = glGetUniformLocation(lightShader->GetProgramID(), "projMatrix");

	glUniform2f(pixelSizeUniform, 1.0f / (float)renderer.GetWidth(), 1.0f / (float)renderer.GetHeight());
	glUniform1i(glGetUniformLocation(lightShader->GetProgramID(), "depthTex"), 0);
	glUniform1i(glGetUniformLocation(lightShader->GetProgramID(), "normalTex"), 1);
	glUniform1i(glGetUniformLocation(lightShader->GetProgramID(), "shadowTex"), 2);

	lightShader->Unbind();
}

LightingRPass::~LightingRPass() {
	delete shadowFrameBuffer;
	delete lightFrameBuffer;

	delete shadowTex;

	delete lightDiffuseOutTex;
	delete lightSpecularOutTex;

	delete sphere;
	delete quad;

	delete defaultShadowShader;
	delete lightShader;
}

void LightingRPass::OnWindowResize(int width, int height) {
	RenderPassBase::OnWindowResize(width, height);
	glUniform2f(pixelSizeUniform, 1.0f / (float)width, 1.0f / (float)height);
}

void LightingRPass::Render() {
	gameWorld.OperateOnLights([&](const Light& light) {
		Matrix4 projView;
		if (light.position.w == 0.0f) {
			projView = Matrix4::Orthographic(-50.0f, 50.0f, -50.0f, 50.0f, -1.0f, 50.0f) *
				       Matrix4::BuildViewMatrix(light.direction * 20.0f, Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f));
		} else {
			projView = Matrix4::Perspective(1.0f, 100.0f, 1.0f, 170.0f) *
				       Matrix4::BuildViewMatrix(Vector3(light.position), Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f));
		}
		DrawShadows(light, projView);
		DrawLight(light, projView);
	});
}

void LightingRPass::AddShadowShader(OGLShader* shader) {
	shadowShaders.push_back(shader);
}

void LightingRPass::DrawShadows(const Light& light, const Matrix4& projView) {
	shadowFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	for (OGLShader* shader : shadowShaders) {
		shader->Bind();

		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projViewMatrix"), 1, GL_FALSE, (GLfloat*)&projView);
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
		// TODO - Implement per-gameObject shadowShaders
		if (true) {
			shader = defaultShadowShader;
		}

		shader->Bind();

		Matrix4 modelMatrix = renderObject->GetTransform()->GetGlobalMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "modelMatrix"), 1, GL_FALSE, (GLfloat*)&modelMatrix);

		mesh->Draw();

		shader->Unbind();
	});

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, renderer.GetWidth(), renderer.GetHeight());
	shadowFrameBuffer->Unbind();
}

void LightingRPass::DrawLight(const Light& light, const Matrix4& projView) {
	lightFrameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	lightShader->Bind();

	depthTexIn->Bind(0);
	normalTexIn->Bind(1);
	shadowTex->Bind(2);

	glUniform3fv(cameraPosUniform, 1, (GLfloat*)&gameWorld.GetMainCamera()->GetPosition()[0]);
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 inverseViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(inverseProjViewUniform, 1, GL_FALSE, (GLfloat*)&inverseViewProj);

	glUniform4fv(lightPositionUniform, 1, (GLfloat*)&light.position);
	glUniform4fv(lightColourUniform, 1, (GLfloat*)&light.colour);
	glUniform1f(lightRadiusUniform, light.radius);
	glUniform3fv(lightDirectionUniform, 1, (GLfloat*)&light.direction);
	glUniform1f(lightAngleUniform, light.angle);

	glUniformMatrix4fv(shadowMatrixUniform, 1, GL_FALSE, (GLfloat*)&projView);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE, (GLfloat*)projMatrix.array);

	if (light.position.w == 0.0f) {
		quad->Draw();
	} else {
		sphere->Draw();
	}
	
	lightShader->Unbind();

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

	lightFrameBuffer->Unbind();
}
