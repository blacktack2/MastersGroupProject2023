/**
 * @file   GameTechRenderer.cpp
 * @brief  See GameTechRenderer.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "GameTechRenderer.h"

#include "GameWorld.h"

#include "Camera.h"

using namespace NCL;
using namespace NCL::Rendering;
using namespace NCL::CSC8503;

GameTechRenderer::GameTechRenderer() : OGLRenderer(*Window::GetWindow()), gameWorld(GameWorld::instance()) {
	baseInstance = this;
	for (size_t i = 0; i < 4; i++) {
		playersHP[i] = -1;
	}
}

void GameTechRenderer::SetGameWorldDeltaTime(float dt) {
	gameWorld.SetDeltaTime(dt);
}

void GameTechRenderer::InitPipeline() {
	paintingRPass = std::make_unique<PaintingRPass>();
	AddMainPass(*paintingRPass);

	skyboxPass = std::make_unique<SkyboxRPass>();
	AddMainPass(*skyboxPass);

	modelPass = std::make_unique<ModelRPass>();
	AddMainPass(*modelPass);

	lightingPass = std::make_unique<LightingRPass>();
	lightingPass->SetDepthTexIn(modelPass->GetDepthOutTex());
	lightingPass->SetNormalTexIn(modelPass->GetNormalOutTex());
	lightingPass->SetSpecTexIn(modelPass->GetSpecOutTex());
	AddMainPass(*lightingPass);

	ssaoPass = std::make_unique<SSAORPass>();
	ssaoPass->SetDepthTexIn(modelPass->GetDepthOutTex());
	ssaoPass->SetNormalTexIn(modelPass->GetNormalOutTex());
	AddMainPass(*ssaoPass);

	combinePass = std::make_unique<CombineRPass>();
	combinePass->SetSkyboxTexIn(skyboxPass->GetOutTex());
	combinePass->SetAlbedoTexIn(modelPass->GetAlbedoOutTex());
	combinePass->SetDiffuseTexIn(lightingPass->GetDiffuseOutTex());
	combinePass->SetSpecularTexIn(lightingPass->GetSpecularOutTex());
	combinePass->SetSSAOTexIn(ssaoPass->GetOutTex());
	combinePass->SetNormalTexIn(modelPass->GetNormalOutTex());
	combinePass->SetDepthTexIn(modelPass->GetDepthOutTex());
	SetCombinePass(*combinePass);

	bloomPass = std::make_unique<BloomRPass>();
	SetBloomAmount(bloomAmount);
	SetBloomBias(bloomBias);
	AddPostPass(*bloomPass, "Bloom");

	hdrPass = std::make_unique<HDRRPass>();
	SetHDRExposure(hdrExposure);
	AddPostPass(*hdrPass, "HDR");

	presentPass = std::make_unique<PresentRPass>();
	SetGamma(gamma);
	SetPresentPass(*presentPass);

	menuPass = std::make_unique<MenuRPass>();
	AddOverlayPass(*menuPass, "Menu");

	debugPass = std::make_unique<DebugRPass>();
	AddOverlayPass(*debugPass, "Debug");

	hudPass = std::make_unique<HudRPass>();
	AddOverlayPass(*hudPass, "Hud");

	UpdatePipeline();
}

void GameTechRenderer::SetGamma(float g) {
	gamma = g;
	modelPass->SetGamma(gamma);
	presentPass->SetGamma(gamma);
}

void GameTechRenderer::SetSunDir(Vector3 direction) {
	sunDirection = direction;
	skyboxPass->SetSunDir(direction);
}

void GameTechRenderer::SetBloomAmount(size_t depth) {
	bloomAmount = std::min(std::max(depth, 1ull), 100ull);
	bloomPass->SetBloomDepth(bloomAmount);
}

void GameTechRenderer::SetBloomBias(float bias) {
	bloomBias = bias;
	bloomPass->SetBias(bloomBias);
}

void GameTechRenderer::SetHDRExposure(float exposure) {
	hdrExposure = exposure;
	hdrPass->SetExposure(hdrExposure);
}

void GameTechRenderer::SetSSAORadius(float radius) {
	ssaoRadius = radius;
	ssaoPass->SetRadius(ssaoRadius);
}

void GameTechRenderer::SetSSAOBias(float bias) {
	ssaoBias = bias;
	ssaoPass->SetBias(ssaoBias);
}

void GameTechRenderer::SetGameWorldMainCamera(int cameraNum) {
	gameWorld.SetMainCamera(cameraNum);
}

int GameTechRenderer::GetGameWorldMainCamera() {
	return gameWorld.GetMainCameraIndex();
}

void GameTechRenderer::DisplayWinLoseInformation(int playerID) {
	gameWorld.GetCamera(playerID)->GetHud();
	if (gameWorld.GetCamera(playerID)->GetHud().GetPlayerHealth() != nullptr) {
		if(gameWorld.GetCamera(playerID)->GetHud().GetPlayerHealth()->GetHealth() <= 0)
			debugPass->RenderWinLoseInformation(false);
	}
}
