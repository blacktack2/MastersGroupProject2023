/**
 * @file   HudRPass.cpp
 * @brief  See HudRPass.h
 *
 * @author Shantao Liu
 * @author Xiaoyang Liu
 * @date   March 2023
 */

#include"HudRPass.h"
#include "GameTechRenderer.h"
#include "GameWorld.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "Debug.h"
#include "UIObject.h"
using namespace NCL;
using namespace CSC8503;
HudRPass::HudRPass():OGLOverlayRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {

	//defaultTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultmain.jpg");
	defaultTexture = AssetLoader::LoadTexture("defaultmain.jpg");

	defaultShader = AssetLoader::CreateShaderAndInit("menuVertex.vert", "menuFragment.frag");

	defaultShader->Bind();

	defaultShader->SetUniformInt("diffuseTex", 0);

	defaultShader->Unbind();
};
HudRPass::~HudRPass() {
};
void HudRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	gameWorld.GetMainCamera()->GetHud().Draw();

	renderer.GetConfig().SetCullFace();
}
