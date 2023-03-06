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

#include "GameTechRenderer.h"
#include "GameWorld.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "Debug.h"
#include "MenuManager.h"
#include "UIObject.h"

using namespace NCL;
using namespace CSC8503;

MenuRPass::MenuRPass() : OGLOverlayRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	defaultTexture = AssetLoader::LoadTexture("defaultmain.jpg");

	defaultShader = AssetLoader::CreateShader("menuVertex.vert", "menuFragment.frag");

	defaultShader->Bind();

	defaultShader->SetUniformInt("diffuseTex", 0);

	defaultShader->Unbind();
}

MenuRPass::~MenuRPass() {
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
