/**
 * @file   MenuRPass.cpp
 * @brief  See MenuRPass.h.
 * 
 * @author Yifei Hu
 * @author Felix Chiu
 * @author Stuart Lewis
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

	defaultShader->Bind();

	defaultShader->SetUniformInt("diffuseTex", 0);

	defaultShader->Unbind();
}

MenuRPass::~MenuRPass() {
	delete defaultShader;
	delete defaultTexture;
};

void MenuRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	DrawMenu();
	DrawButtons();

	renderer.GetConfig().SetCullFace();
}

void MenuRPass::DrawMenu(){
	MenuManager& menuManager = MenuManager::instance();
	Menu* menu = menuManager.GetCurrentMenu();
	if (menu) {
		DrawUIObject((UIObject*)menu);
	}
}

void MenuRPass::DrawButtons() {
	MenuManager& menuManager = MenuManager::instance();
	Menu* menu = menuManager.GetCurrentMenu();
	if (!menu) {
		return;
	}
	for (Button* btn : *menu->GetButtons()) {
		if (!btn)
			return;
		DrawUIObject((UIObject*)btn);
	}
}

void MenuRPass::DrawUIObject(UIObject* obj){
	MenuRenderObject* renderObject = obj->GetRenderObject();
	if (!renderObject) {
		return;
	}

	renderObject->Draw(obj->GetDimension());
}
