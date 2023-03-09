/**
 * @file   GameTechRenderer.cpp
 * @brief  See GameTechRenderer.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "GameTechRenderer.h"

#include "Camera.h"
#include "GameObject.h"
#include "RenderObject.h"

#include "MeshAnimation.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "AssetLoader.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

GameTechRenderer::GameTechRenderer() : OGLRenderer(*Window::GetWindow()), gameWorld(GameWorld::instance()) {
}

GameTechRenderer::~GameTechRenderer() {
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

	UpdatePipeline();
}

void GameTechRenderer::SortObjectList() {

}

void GameTechRenderer::Update(float dt) {
}
