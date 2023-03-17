/**
 * @file   HudRPass.cpp
 * @brief  See HudRPass.h
 *
 * @author Shantao Liu
 * @author Xiaoyang Liu
 * @date   March 2023
 */
#include "HudRPass.h"

#include "GameWorld.h"
#include "GameTechRenderer.h"

using namespace NCL::CSC8503;

HudRPass::HudRPass() : OGLOverlayRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
}

void HudRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	gameWorld.GetMainCamera()->GetHud().Draw();

	renderer.GetConfig().SetCullFace();
}
