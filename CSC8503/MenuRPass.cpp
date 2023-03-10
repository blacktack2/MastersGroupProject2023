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
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()), menuManager(MenuManager::instance()) {
	defaultTexture = AssetLoader::LoadTexture("defaultmain.jpg");

	defaultShader = AssetLoader::CreateShaderAndInit("menuVertex.vert", "menuFragment.frag");

	defaultShader->Bind();

	defaultShader->SetUniformInt("diffuseTex", 0);

	defaultShader->Unbind();
}

MenuRPass::~MenuRPass() {
};

void MenuRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	menuManager.GetCurrentMenu().Draw();

	renderer.GetConfig().SetCullFace();
}
