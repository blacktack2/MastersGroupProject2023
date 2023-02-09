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
	renderPasses.push_back(skyboxPass);

	modelPass = new ModelRPass(*this, gameWorld);
	renderPasses.push_back(modelPass);

	lightingPass = new LightingRPass(*this, gameWorld,
		modelPass->GetDepthOutTex(), modelPass->GetNormalOutTex());
	renderPasses.push_back(lightingPass);

	gbufferPass = new GBufferRPass(*this,
		skyboxPass->GetOutTex(), modelPass->GetDiffuseOutTex(),
		lightingPass->GetDiffuseOutTex(), lightingPass->GetSpecularOutTex(),
		modelPass->GetNormalOutTex(), modelPass->GetDepthOutTex());
	renderPasses.push_back(gbufferPass);

	bloomPass = new BloomRPass(*this,
		gbufferPass->GetSceneOutTex(), gbufferPass->GetBloomOutTex());
	bloomPass->SetBlurAmount(5);
	renderPasses.push_back(bloomPass);

	presentPass = new PresentRPass(*this, bloomPass->GetOutTex());
	renderPasses.push_back(presentPass);

	debugPass = new DebugRPass(*this, gameWorld);
	renderPasses.push_back(debugPass);
}

GameTechRenderer::~GameTechRenderer() {
	delete skyboxPass;
	delete modelPass;
	delete lightingPass;
	delete gbufferPass;
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