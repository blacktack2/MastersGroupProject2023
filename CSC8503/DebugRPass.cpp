/**
 * @file   DebugRPass.cpp
 * @brief  See DebugRPass.h
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "DebugRPass.h"

#include "GameTechRenderer.h"
#include "GameWorld.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "Debug.h"

using namespace NCL;
using namespace CSC8503;

DebugRPass::DebugRPass() :
OGLOverlayRenderPass(), gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	lineShader = AssetLoader::CreateShaderAndInit("debugLines.vert", "debugLines.frag");
	textShader = AssetLoader::CreateShaderAndInit("debugText.vert" , "debugText.frag" );

	textShader->Bind();

	Matrix4 viewProjMatrix = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);
	textShader->SetUniformMatrix("viewProjMatrix", viewProjMatrix);
	textShader->SetUniformInt("fontTex", 0);

	textShader->Unbind();

	lineMesh = AssetLoader::CreateMesh();
	lineMesh->SetPrimitiveType(GeometryPrimitive::Lines);
	textMesh = AssetLoader::CreateMesh();
}

DebugRPass::~DebugRPass() {
}

void DebugRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	RenderLines();
	RenderText();

	renderer.GetConfig().SetCullFace();
}

void DebugRPass::RenderLines() {
	const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
	if (lines.empty()) {
		return;
	}

	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(renderer.GetAspect());
	
	Matrix4 viewProjMatrix  = projMatrix * viewMatrix;

	lineShader->Bind();

	lineShader->SetUniformMatrix("viewProjMatrix", viewProjMatrix);

	debugLineData.clear();

	size_t frameLineCount = lines.size() * 2;

	std::vector<Vector3> positions = std::vector<Vector3>(lines.size() * 2, Vector3());
	std::vector<Vector4> colours = std::vector<Vector4>(lines.size() * 2, Vector4());
	for (size_t i = 0; i < lines.size(); i++) {
		const Debug::DebugLineEntry& entry = lines[i];
		positions[i * 2] = entry.start;
		positions[i * 2 + 1] = entry.end;
		colours[i * 2] = entry.colourA;
		colours[i * 2 + 1] = entry.colourB;
	}

	lineMesh->SetVertexPositions(positions);
	lineMesh->SetVertexColours(colours);
	lineMesh->UploadToGPU();
	lineMesh->Draw();

	lineShader->Unbind();
}

void DebugRPass::RenderText() {
	const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
	if (strings.empty()) {
		return;
	}

	textShader->Bind();

	const TextureBase& texture = Debug::GetDebugFont()->GetTexture();

	texture.Bind(0);

	debugTextPos.clear();
	debugTextColours.clear();
	debugTextUVs.clear();

	for (const auto& s : strings) {
		float size = 20.0f;
		Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, size, debugTextPos, debugTextUVs, debugTextColours);
	}

	textMesh->SetVertexPositions(debugTextPos);
	textMesh->SetVertexColours(debugTextColours);
	textMesh->SetVertexTextureCoords(debugTextUVs);
	textMesh->UploadToGPU();
	textMesh->Draw();

	textShader->Unbind();
}

void DebugRPass::RenderWinLoseInformation(bool win) {
	renderer.GetConfig().SetCullFace(false);
	textShader->Bind();

	const TextureBase& texture = Debug::GetDebugFont()->GetTexture();

	texture.Bind(0);

	debugTextPos.clear();
	debugTextColours.clear();
	debugTextUVs.clear();

	float size = 20.0f;
	if (win) Debug::GetDebugFont()->BuildVerticesForString("You Win!", Vector2(5, 80), Vector4(0, 1, 0, 1), size, debugTextPos, debugTextUVs, debugTextColours);
	else Debug::GetDebugFont()->BuildVerticesForString("You got Inked!", Vector2(5, 80), Vector4(1, 0, 0, 1), size, debugTextPos, debugTextUVs, debugTextColours);

	textMesh->SetVertexPositions(debugTextPos);
	textMesh->SetVertexColours(debugTextColours);
	textMesh->SetVertexTextureCoords(debugTextUVs);
	textMesh->UploadToGPU();
	textMesh->Draw();

	textShader->Unbind();
	renderer.GetConfig().SetCullFace();
}
