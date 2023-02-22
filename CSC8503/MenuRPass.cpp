/**
 * @author Yifei Hu
 * @author Felix Chiu
 * @date   February 2023
 */
#include "MenuRPass.h"

#include "Debug.h"

#include "MenuManager.h"

using namespace NCL::CSC8503;

MenuRPass::MenuRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
	OGLOverlayRenderPass(renderer), gameWorld(gameWorld) {

	defaultTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultmain.jpg");

	defaultShader = new OGLShader("menuVertex.vert", "menuFragment.frag");

	defaultTexture->Bind();
	glUniform1i(glGetUniformLocation(defaultShader->GetProgramID(), "diffuseTex"), 0);
	defaultTexture->Unbind();
}

MenuRPass::~MenuRPass() {
	delete defaultShader;
	delete defaultTexture;
};

void MenuRPass::Render() {
	
	DrawMenu();
	DrawButtons();


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void MenuRPass::DrawMenu()
{
	MenuManager* menuManager = &MenuManager::instance();
	Menu* menu = menuManager->GetCurrentMenu();
	DrawUIObject((UIObject *) menu);
}

void MenuRPass::DrawUIObject(UIObject* obj)
{
	MenuManager* menuManager = &MenuManager::instance();
	const RenderObject* renderObject = obj->GetRenderObject();
	if (!renderObject) {
		return;
	}
	OGLMesh* mesh = (OGLMesh*)renderObject->GetMesh();
	if (!(mesh = dynamic_cast<OGLMesh*>(renderObject->GetMesh()))) {
		return;
	}
	OGLShader* shader;
	if (!(shader = dynamic_cast<OGLShader*>(renderObject->GetShader()))) {
		shader = defaultShader;
	}

	OGLTexture* texture;
	if (!(texture = dynamic_cast<OGLTexture*>(renderObject->GetDefaultTexture()))) {
		texture = defaultTexture;
	}

	Vector4 dimension = obj->GetDimension();

	Vector4 screenDimension = menuManager->PixelToScreenSpace((float)renderer.GetWidth(), (float)renderer.GetHeight(), dimension);

	mesh->UploadToGPU();

	shader->Bind();

	Matrix4 quadMatrix = modelMatrix;
	quadMatrix = quadMatrix *
		Matrix4::Translation(
			Vector3(
					(screenDimension.x + screenDimension.z) / 2,
					(screenDimension.y + screenDimension.w) / 2,
					0.0f
				) 
			) * 
		Matrix4::Scale(
			Vector3(
					(screenDimension.z - screenDimension.x),
					(screenDimension.y - screenDimension.w),
					1.0f
				)
			);

	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)quadMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	texture->Bind(0);

	mesh->Draw();

	shader->Unbind();
}

void MenuRPass::DrawButtons()
{
	MenuManager* menuManager = &MenuManager::instance();
	Menu* menu = menuManager->GetCurrentMenu();
	for (Button* btn : *(menu->GetButtons())) {
		DrawUIObject((UIObject*)btn);
	}
}