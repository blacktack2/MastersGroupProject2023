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
	GetConfig().viewport.Push({ 0, 0 });
	GetConfig().enableDepthMask.Push(GL_TRUE);
	GetConfig().enableDepthTest.Push(GL_TRUE);
	GetConfig().depthFunc.Push(GL_LEQUAL);
	GetConfig().enableCullFace.Push(GL_TRUE);
	GetConfig().cullFace.Push(GL_BACK);
	GetConfig().enableBlend.Push(GL_TRUE);
	GetConfig().blendFunc.Push({ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA });
	GetConfig().patchVertices.Push(4);

	skyboxPass = new SkyboxRPass(*this, gameWorld);
	renderPasses.push_back(skyboxPass);

	modelPass = new ModelRPass(*this, gameWorld);
	renderPasses.push_back(modelPass);

	lightingPass = new LightingRPass(*this, gameWorld,
		modelPass->GetDepthOutTex(), modelPass->GetNormalOutTex());
	renderPasses.push_back(lightingPass);

	combinePass = new CombineRPass(*this,
		skyboxPass->GetOutTex(), modelPass->GetDiffuseOutTex(),
		lightingPass->GetDiffuseOutTex(), lightingPass->GetSpecularOutTex(),
		modelPass->GetNormalOutTex(), modelPass->GetDepthOutTex());
	renderPasses.push_back(combinePass);

	bloomPass = new BloomRPass(*this, combinePass->GetOutTex());
	SetBloomAmount(bloomAmount);
	SetBloomThreshold(bloomThreshold);
	renderPasses.push_back(bloomPass);

	hdrPass = new HDRRPass(*this, bloomPass->GetOutTex());
	SetHDRExposure(hdrExposure);
	renderPasses.push_back(hdrPass);

	presentPass = new PresentRPass(*this, hdrPass->GetOutTex());
	SetGamma(gamma);
	renderPasses.push_back(presentPass);

	debugPass = new DebugRPass(*this, gameWorld);
	renderPasses.push_back(debugPass);
}

GameTechRenderer::~GameTechRenderer() {
	delete skyboxPass;
	delete modelPass;
	delete lightingPass;
	delete combinePass;
	delete presentPass;
	delete debugPass;
}

void GameTechRenderer::RenderFrame() {
	glClearColor(0, 0, 0, 0);
	OGLRenderer::RenderFrame();
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
