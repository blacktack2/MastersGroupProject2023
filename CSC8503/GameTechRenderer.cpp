/**
 * @file   GameTechRenderer.cpp
 * @brief  See GameTechRenderer.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @author Yifei Hu
 * @date   February 2023
 */
#include "GameTechRenderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer() : OGLRenderer(*Window::GetWindow()), gameWorld(GameWorld::instance()) {
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	paintingRPass = new PaintingRPass(*this);
	AddMainPass(paintingRPass);

	skyboxPass = new SkyboxRPass(*this, gameWorld);
	AddMainPass(skyboxPass);

	modelPass = new ModelRPass(*this, gameWorld);
	AddMainPass(modelPass);

	lightingPass = new LightingRPass(*this, gameWorld,
		modelPass->GetDepthOutTex(), modelPass->GetNormalOutTex());
	AddMainPass(lightingPass);

	ssaoPass = new SSAORPass(*this,
		modelPass->GetDepthOutTex(), modelPass->GetNormalOutTex());
	AddMainPass(ssaoPass);

	combinePass = new CombineRPass(*this,
		skyboxPass->GetOutTex(), modelPass->GetDiffuseOutTex(),
		lightingPass->GetDiffuseOutTex(), lightingPass->GetSpecularOutTex(), ssaoPass->GetOutTex(),
		modelPass->GetNormalOutTex(), modelPass->GetDepthOutTex());
	SetCombinePass(combinePass);

	bloomPass = new BloomRPass(*this);
	SetBloomAmount(bloomAmount);
	SetBloomBias(bloomBias);
	AddPostPass(bloomPass, "Bloom");

	hdrPass = new HDRRPass(*this);
	SetHDRExposure(hdrExposure);
	AddPostPass(hdrPass, "HDR");

	presentPass = new PresentRPass(*this);
	SetGamma(gamma);
	SetPresentPass(presentPass);

	menuPass = new MenuRPass(*this, gameWorld);
	AddOverlayPass(menuPass, "Menu");


	debugPass = new DebugRPass(*this, gameWorld);
	AddOverlayPass(debugPass, "Debug");

	UpdatePipeline();

}

GameTechRenderer::~GameTechRenderer() {
	delete skyboxPass;
	delete modelPass;
	delete lightingPass;
	delete combinePass;
	delete presentPass;
	delete bloomPass;
	delete hdrPass;
	delete debugPass;
	delete menuPass;
	delete pausePass;
}

void GameTechRenderer::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);
}

void GameTechRenderer::SortObjectList() {

}

MeshGeometry* GameTechRenderer::LoadMesh(const std::string& name) {
	OGLMesh* mesh = new OGLMesh(name);
	mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	mesh->UploadToGPU();
	return mesh;
}

TextureBase* GameTechRenderer::LoadTexture(const std::string& name) {
	return TextureLoader::LoadAPITexture(name);
}

ShaderBase* GameTechRenderer::LoadShader(const std::string& vertex, const std::string& fragment) {
	return new OGLShader(vertex, fragment);
}

void GameTechRenderer::Update(float dt) {
	/*if (dt)
		menuPass->Update(dt);*/
	/*for (Billboard* billboard : blood) {
		billboard->update(player->position);
	}*/
}
