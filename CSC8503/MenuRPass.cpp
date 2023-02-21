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

	menuTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultmain.jpg");

	menuShader = new OGLShader("menuVertex.vert", "menuFragment.frag");

	menuTexture->Bind();
	glUniform1i(glGetUniformLocation(menuShader->GetProgramID(), "diffuseTex"), 0);
	menuTexture->Unbind();
}

MenuRPass::~MenuRPass() {
	delete menuShader;
	delete menuTexture;
};

void MenuRPass::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	menuShader->Bind();

	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	menuTexture->Bind(0);
	quad->Draw();

	menuShader->Unbind();
	Debug::Print(std::string("Start"), Vector2(0, 0), Vector4(0, 0, 0, 1));
	int num = 3;
	pBtn = new Button(renderer, gameWorld);
	//Load Shadow here
	for (int i = 0; i < num; i++) {
		pBtn->SetColour(Vector4(0, 0, 0, 1));
		pBtn->SetScale(Vector2(0.25f, 0.1f));
		pBtn->SetPosition(Vector2(0.56f, -0.05 - (i * 0.3f)));
		pBtn->Render();
	}
	//Load Button here
	for (int i = 0; i < num; i++) {
		char path[50] = { 0 };
		sprintf_s(path, "button%d.jpg", i + 1);
		pBtn->SetFilename(path);
		pBtn->SetTexture();
		pBtn->SetScale(Vector2(0.25f, 0.1f));
		pBtn->SetPosition(Vector2(0.53f, -(i * 0.3f)));
		pBtn->Render();
	}
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}