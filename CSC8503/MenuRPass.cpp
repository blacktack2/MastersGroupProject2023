/**
 * @author Yifei Hu
 * @date   February 2023
 */
#include "MenuRPass.h"

#include "Debug.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLFrameBuffer.h"

using namespace NCL::CSC8503;

MenuRPass::MenuRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
	OGLOverlayRenderPass(renderer), gameWorld(gameWorld) {
	quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-1, 1, -1),
		Vector3(-1, -1, -1),
		Vector3(1, -1, -1),
		Vector3(1, 1, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 1),
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->SetVertexColours({
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		});
	quad->UploadToGPU();

	menuTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultstart.jpg");

	menuShader = new OGLShader("menuVertex.vert", "menuFragment.frag");

	menuTexture->Bind();
	glUniform1i(glGetUniformLocation(menuShader->GetProgramID(), "diffuseTex"), 0);
	menuTexture->Unbind();

	//LoadButton();
}

MenuRPass::~MenuRPass() {
	delete menuShader;
	delete menuTexture;
};

void MenuRPass::Render() {
	menuShader->Bind();

	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	menuTexture->Bind(0);

	quad->Draw();

	menuShader->Unbind();

	//btnShader->Bind();

	//btn->Draw();

	//btnShader->Unbind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void MenuRPass::LoadButton() {
	btn = new OGLMesh();
	btn->SetVertexPositions({
		Vector3(-0.1, 0.1, 0),
		Vector3(-0.1, -0.1, 0),
		Vector3(0.1, -0.1, 0),
		Vector3(0.1, 0.1, 0),
		});
	btn->SetVertexTextureCoords({
		Vector2(0, 1),
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		});
	btn->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	btn->SetVertexColours({
		Vector4(1, 1, 1, 1),
		Vector4(0, 1, 1, 1),
		Vector4(1, 0, 1, 1),
		Vector4(1, 1, 0, 1),
		});
	btn->UploadToGPU();

	btnShader = new OGLShader("btnVertex.vert", "btnFragment.frag");
}


