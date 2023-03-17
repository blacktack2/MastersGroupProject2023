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

#include "AssetLoader.h"

#include "ShaderBase.h"
#include "TextureBase.h"

#include "MenuManager.h"

using namespace NCL::CSC8503;
using namespace NCL::Rendering;

MenuRPass::MenuRPass() : OGLOverlayRenderPass(),
renderer(GameTechRenderer::instance()), menuManager(MenuManager::instance()) {
	defaultTexture = AssetLoader::LoadTexture("defaultmain.jpg");

	defaultShader = AssetLoader::CreateShaderAndInit("menuVertex.vert", "menuFragment.frag");

	defaultShader->Bind();

	defaultShader->SetUniformInt("diffuseTex", 0);
	defaultShader->SetUniformInt("selectedTex", 1);

	defaultShader->Unbind();
}

void MenuRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	menuManager.GetCurrentMenu().Draw();

	renderer.GetConfig().SetCullFace();
}
