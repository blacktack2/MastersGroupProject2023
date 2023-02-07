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
#include "Debug.h"
#include "TextureLoader.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

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

	presentPass = new PresentRPass(*this, gbufferPass->GetOutTex());
	renderPasses.push_back(presentPass);

	glGenVertexArrays(1, &lineVAO);
	glGenVertexArrays(1, &textVAO);

	glGenBuffers(1, &lineVertVBO);
	glGenBuffers(1, &textVertVBO);
	glGenBuffers(1, &textColourVBO);
	glGenBuffers(1, &textTexVBO);

	SetDebugStringBufferSizes(10000);
	SetDebugLineBufferSizes(1000);
}

GameTechRenderer::~GameTechRenderer() {
	delete skyboxPass;
	delete modelPass;
	delete lightingPass;
	delete gbufferPass;
	delete presentPass;
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

void GameTechRenderer::NewRenderLines() {
	//const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
	//if (lines.empty()) {
	//	return;
	//}
	//float screenAspect = (float)windowWidth / (float)windowHeight;
	//Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	//Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	//
	//Matrix4 viewProj  = projMatrix * viewMatrix;

	//debugShader->Bind();

	//int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	//GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	//glUniform1i(texSlot, 0);

	//glUniformMatrix4fv(matSlot, 1, false, (float*)viewProj.array);

	//debugLineData.clear();

	//int frameLineCount = lines.size() * 2;

	//SetDebugLineBufferSizes(frameLineCount);

	//glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(Debug::DebugLineEntry), lines.data());

	//glBindVertexArray(lineVAO);
	//glDrawArrays(GL_LINES, 0, frameLineCount);
	//glBindVertexArray(0);

	//debugShader->Unbind();
}

void GameTechRenderer::NewRenderText() {
	//const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
	//if (strings.empty()) {
	//	return;
	//}

	//debugShader->Bind();

	//OGLTexture* texture = (OGLTexture*)Debug::GetDebugFont()->GetTexture();

	//if (texture) {
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, texture->GetObjectID());
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	texture->Bind(0, glGetUniformLocation(debugShader->GetProgramID(), "mainTex"));
	//}
	//Matrix4 proj = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);

	//int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	//glUniformMatrix4fv(matSlot, 1, false, (float*)proj.array);

	//GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	//glUniform1i(texSlot, 1);

	//debugTextPos.clear();
	//debugTextColours.clear();
	//debugTextUVs.clear();

	//int frameVertCount = 0;
	//for (const auto& s : strings) {
	//	frameVertCount += Debug::GetDebugFont()->GetVertexCountForString(s.data);
	//}
	//SetDebugStringBufferSizes(frameVertCount);

	//for (const auto& s : strings) {
	//	float size = 20.0f;
	//	Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, size, debugTextPos, debugTextUVs, debugTextColours);
	//}

	//glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector3), debugTextPos.data());
	//glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector4), debugTextColours.data());
	//glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector2), debugTextUVs.data());

	//glBindVertexArray(textVAO);
	//glDrawArrays(GL_TRIANGLES, 0, frameVertCount);
	//glBindVertexArray(0);

	//debugShader->Unbind();
}



TextureBase* GameTechRenderer::LoadTexture(const std::string& name) {
	return TextureLoader::LoadAPITexture(name);
}

ShaderBase* GameTechRenderer::LoadShader(const std::string& vertex, const std::string& fragment) {
	return new OGLShader(vertex, fragment);
}

void GameTechRenderer::Update(float dt) {
}

void GameTechRenderer::SetDebugStringBufferSizes(size_t newVertCount) {
	if (newVertCount > textCount) {
		textCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector2), nullptr, GL_DYNAMIC_DRAW);

		debugTextPos.reserve(textCount);
		debugTextColours.reserve(textCount);
		debugTextUVs.reserve(textCount);

		glBindVertexArray(textVAO);

		glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, textVertVBO, 0, sizeof(Vector3));

		glVertexAttribFormat(1, 4, GL_FLOAT, false, 0);
		glVertexAttribBinding(1, 1);
		glBindVertexBuffer(1, textColourVBO, 0, sizeof(Vector4));

		glVertexAttribFormat(2, 2, GL_FLOAT, false, 0);
		glVertexAttribBinding(2, 2);
		glBindVertexBuffer(2, textTexVBO, 0, sizeof(Vector2));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
}

void GameTechRenderer::SetDebugLineBufferSizes(size_t newVertCount) {
	if (newVertCount > lineCount) {
		lineCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
		glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(Debug::DebugLineEntry), nullptr, GL_DYNAMIC_DRAW);

		debugLineData.reserve(lineCount);

		glBindVertexArray(lineVAO);

		int realStride = sizeof(Debug::DebugLineEntry) / 2;

		glVertexAttribFormat(0, 3, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, start));
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, lineVertVBO, 0, realStride);

		glVertexAttribFormat(1, 4, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, colourA));
		glVertexAttribBinding(1, 0);
		glBindVertexBuffer(1, lineVertVBO, sizeof(Vector4), realStride);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}