/**
 * @author Yifei Hu
 * @date   February 2023
 */
#include "PauseRPass.h"

#include "Debug.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLFrameBuffer.h"

using namespace NCL::CSC8503;

PauseRPass::PauseRPass(OGLRenderer& renderer, GameWorld& gameWorld) :
	OGLOverlayRenderPass(renderer), gameWorld(gameWorld) {
	quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-0.3, 0.7, -1),
		Vector3(-0.3, -0.7, -1),
		Vector3(0.3, -0.7, -1),
		Vector3(0.3, 0.7, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 1),
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->SetVertexColours({
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		});
	quad->UploadToGPU();

	pauseTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultpause.jpg");

	pauseShader = new OGLShader("menuVertex.vert", "menuFragment.frag");

	pauseTexture->Bind();
	glUniform1i(glGetUniformLocation(pauseShader->GetProgramID(), "diffuseTex"), 0);
	pauseTexture->Unbind();

	//text
	textShader = new OGLShader("debugText.vert", "debugText.frag");
	textShader->Bind();

	Matrix4 viewProjMatrix = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgramID(), "viewProjMatrix"), 1, GL_FALSE, (GLfloat*)viewProjMatrix.array);
	glUniform1i(glGetUniformLocation(textShader->GetProgramID(), "fontTex"), 0);

	textShader->Unbind();

	glGenVertexArrays(1, &textVAO);

	glGenBuffers(1, &textVertVBO);
	glGenBuffers(1, &textColourVBO);
	glGenBuffers(1, &textTexVBO);

	SetDebugStringBufferSizes(10000);
}

PauseRPass::~PauseRPass() {
	delete pauseShader;
	delete pauseTexture;

	glDeleteBuffers(1, &textVertVBO);
	glDeleteBuffers(1, &textColourVBO);
	glDeleteBuffers(1, &textTexVBO);

	glDeleteVertexArrays(1, &textVAO);
};

void PauseRPass::Render() {
	pauseShader->Bind();

	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);

	pauseTexture->Bind(0);
	quad->Draw();

	pauseShader->Unbind();
	Debug::Print(std::string("Start"), Vector2(0, 0), Vector4(0, 0, 0, 1));

	int num = 4;
	pBtn = new Button(renderer, gameWorld);
	//Load Buttons here
	for (int i = 0; i < num; i++) {
		char path[50] = { 0 };
		sprintf_s(path, "button%d.jpg", i + 4);
		pBtn->SetFilename(path);
		pBtn->SetTexture();
		pBtn->SetScale(Vector2(0.15f, 0.07f));
		pBtn->SetPosition(Vector2(0, 0.45f - (i * 0.3f)));
		pBtn->Render();
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void PauseRPass::SetDebugStringBufferSizes(size_t newVertCount) {
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

void PauseRPass::RenderText() {
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

	Debug::Print("Start", Vector2(55, 50), Vector4(0, 0, 0, 1));

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
