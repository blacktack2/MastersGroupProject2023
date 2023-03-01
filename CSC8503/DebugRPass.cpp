/**
 * @file   DebugRPass.cpp
 * @brief  See DebugRPass.h
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "DebugRPass.h"

#include "Debug.h"
#include "OGLShader.h"
#include "OGLTexture.h"

using namespace NCL::CSC8503;

DebugRPass::DebugRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
OGLOverlayRenderPass(renderer), gameWorld(gameWorld) {
	lineShader = new OGLShader("debugLines.vert", "debugLines.frag");
	textShader = new OGLShader("debugText.vert" , "debugText.frag" );

	textShader->Bind();

	Matrix4 viewProjMatrix = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);
	textShader->SetUniformMatrix("viewProjMatrix", viewProjMatrix);
	textShader->SetUniformInt("fontTex", 0);

	textShader->Unbind();

	glGenVertexArrays(1, &lineVAO);
	glGenVertexArrays(1, &textVAO);

	glGenBuffers(1, &lineVertVBO);
	glGenBuffers(1, &textVertVBO);
	glGenBuffers(1, &textColourVBO);
	glGenBuffers(1, &textTexVBO);

	SetDebugStringBufferSizes(10000);
	SetDebugLineBufferSizes(1000);
}

DebugRPass::~DebugRPass() {
	delete lineShader;
	delete textShader;

	glDeleteBuffers(1, &lineVertVBO);
	glDeleteBuffers(1, &textVertVBO);
	glDeleteBuffers(1, &textColourVBO);
	glDeleteBuffers(1, &textTexVBO);

	glDeleteVertexArrays(1, &lineVAO);
	glDeleteVertexArrays(1, &textVAO);
}

void DebugRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	RenderLines();
	RenderText();

	renderer.GetConfig().SetCullFace();
}

void DebugRPass::SetDebugStringBufferSizes(size_t newVertCount) {
	if (newVertCount <= textCount) {
		return;
	}
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

void DebugRPass::SetDebugLineBufferSizes(size_t newVertCount) {
	if (newVertCount <= lineCount) {
		return;
	}
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

void DebugRPass::RenderLines() {
	const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
	if (lines.empty()) {
		return;
	}

	float screenAspect = (float)renderer.GetWidth() / (float)renderer.GetHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	
	Matrix4 viewProjMatrix  = projMatrix * viewMatrix;

	lineShader->Bind();

	lineShader->SetUniformMatrix("viewProjMatrix", viewProjMatrix);

	debugLineData.clear();

	size_t frameLineCount = lines.size() * 2;

	SetDebugLineBufferSizes(frameLineCount);

	glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(Debug::DebugLineEntry), lines.data());

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, frameLineCount);
	glBindVertexArray(0);

	lineShader->Unbind();
}

void DebugRPass::RenderText() {
	const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
	if (strings.empty()) {
		return;
	}

	textShader->Bind();

	OGLTexture* texture = (OGLTexture*)Debug::GetDebugFont()->GetTexture();

	if (!texture) {
		return;
	}
	texture->Bind(0);

	debugTextPos.clear();
	debugTextColours.clear();
	debugTextUVs.clear();

	int frameVertCount = 0;
	for (const auto& s : strings) {
		frameVertCount += Debug::GetDebugFont()->GetVertexCountForString(s.data);
	}
	SetDebugStringBufferSizes(frameVertCount);

	for (const auto& s : strings) {
		float size = 20.0f;
		Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, size, debugTextPos, debugTextUVs, debugTextColours);
	}

	glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector3), debugTextPos.data());
	glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector4), debugTextColours.data());
	glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector2), debugTextUVs.data());

	glBindVertexArray(textVAO);
	glDrawArrays(GL_TRIANGLES, 0, frameVertCount);
	glBindVertexArray(0);

	textShader->Unbind();
}
