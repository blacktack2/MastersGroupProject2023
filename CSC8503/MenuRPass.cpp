/**
 * @author Yifei Hu
 * @date   February 2023
 */
#include "MenuRPass.h"

#include "Debug.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLFrameBuffer.h"

#include "MenuManager.h"

using namespace NCL::CSC8503;

MenuRPass::MenuRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
	OGLOverlayRenderPass(renderer), gameWorld(gameWorld) {

	menuManager = &MenuManager::instance();

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
	
	//DrawMenu();
	DrawButtons();


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void MenuRPass::DrawMenu()
{
	Vector4 menuDimension = menuManager->GetMenuDimension();
	
	Vector4 menuScreenDimension = menuManager->PixelToScreenSpace((float)renderer.GetWidth(), (float)renderer.GetHeight(), menuDimension);

	quad->SetVertexPositions({
		Vector3(menuScreenDimension.x, menuScreenDimension.y, -1),
		Vector3(menuScreenDimension.x, menuScreenDimension.w, -1),
		Vector3(menuScreenDimension.z, menuScreenDimension.w, -1),
		Vector3(menuScreenDimension.z, menuScreenDimension.y, -1),
	});

	quad->UploadToGPU();

	menuShader->Bind();

	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	menuTexture->Bind(0);

	quad->Draw();

	menuShader->Unbind();
}

void MenuRPass::DrawButton(Button* btn)
{
	Vector4 btnDimension = btn->GetDimension();

	Vector4 btnScreenDimension = menuManager->PixelToScreenSpace((float)renderer.GetWidth(), (float)renderer.GetHeight(), btnDimension);

	quad->SetVertexPositions({
		Vector3(btnScreenDimension.x, btnScreenDimension.y, -1),
		Vector3(btnScreenDimension.x, btnScreenDimension.w, -1),
		Vector3(btnScreenDimension.z, btnScreenDimension.w, -1),
		Vector3(btnScreenDimension.z, btnScreenDimension.y, -1),
		});
	if (btn->isMouseHover) {
		btnScreenDimension.x -= 0.01;
		btnScreenDimension.y -= 0.01;
		btnScreenDimension.z += 0.01;
		btnScreenDimension.w += 0.01;
		quad->SetVertexPositions({
		Vector3(btnScreenDimension.x, btnScreenDimension.y, -1),
		Vector3(btnScreenDimension.x, btnScreenDimension.w, -1),
		Vector3(btnScreenDimension.z, btnScreenDimension.w, -1),
		Vector3(btnScreenDimension.z, btnScreenDimension.y, -1),
			});
	}

	quad->UploadToGPU();

	menuShader->Bind();

	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	menuTexture->Bind(0);

	quad->Draw();

	menuShader->Unbind();
}

void MenuRPass::DrawButtons()
{
	Menu* menu = menuManager->GetCurrentMenu();
	for (Button* btn : *(menu->GetButtons())) {
		DrawButton(btn);
	}
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


