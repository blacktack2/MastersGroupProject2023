/**
 * @file   GameTechRenderer.cpp
 * @brief  See GameTechRenderer.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
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

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	skyboxPass = new SkyboxRPass(*this, gameWorld);
	AddMainPass(skyboxPass);

	modelPass = new ModelRPass(*this, gameWorld);
	AddMainPass(modelPass);

	lightingPass = new LightingRPass(*this, gameWorld,
		modelPass->GetDepthOutTex(), modelPass->GetNormalOutTex());
	AddMainPass(lightingPass);

	combinePass = new CombineRPass(*this,
		skyboxPass->GetOutTex(), modelPass->GetDiffuseOutTex(),
		lightingPass->GetDiffuseOutTex(), lightingPass->GetSpecularOutTex(),
		modelPass->GetNormalOutTex(), modelPass->GetDepthOutTex());
	SetCombinePass(combinePass);

	bloomPass = new BloomRPass(*this);
	SetBloomAmount(bloomAmount);
	SetBloomBias(bloomBias);
	AddPostPass(bloomPass);

	hdrPass = new HDRRPass(*this);
	SetHDRExposure(hdrExposure);
	AddPostPass(hdrPass);

	presentPass = new PresentRPass(*this);
	SetGamma(gamma);
	SetPresentPass(presentPass);

	debugPass = new DebugRPass(*this, gameWorld);
	AddOverlayPass(debugPass);

	UpdatePipeline();
}

GameTechRenderer::~GameTechRenderer() {
	delete skyboxPass;
	delete modelPass;
	delete lightingPass;
	delete combinePass;
	delete presentPass;
	delete debugPass;
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
}
