/**
 * @file   Menu.cpp
 * @brief  See Menu.h.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "Menu.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

Menu::Menu(Vector2 screenPos, Vector2 dimension) {
	this->screenPos = screenPos;
	this->dimension = dimension;
}

Menu::~Menu() {
	for (Button* btn : buttons) {
		delete btn;
	}
	buttons.empty();
}

void Menu::Draw(OGLShader* menuShader, OGLTexture* menuTexture, OGLMesh* quad)
{
	menuShader->Bind();

	//glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	//glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	//glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	//glUniformMatrix4fv(glGetUniformLocation(menuShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	menuTexture->Bind(0);

	quad->Draw();

	menuShader->Unbind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	for (Button* btn:buttons) {
		btn->Draw(Debug::GREEN);
	}
}

void Menu::Update(float dt) {
	
}

