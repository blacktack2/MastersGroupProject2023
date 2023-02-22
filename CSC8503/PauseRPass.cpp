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
}

PauseRPass::~PauseRPass() {
	delete pauseShader;
	delete pauseTexture;
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
		pBtn->SetScale(Vector2(0.17f, 0.08f));
		pBtn->SetPosition(Vector2(0, 0.45f - (i * 0.3f)));
		pBtn->Render();
	}

	Update();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void PauseRPass::Update() {
	float mousex = Window::GetMouse()->GetAbsolutePosition().x;
	float mousey = Window::GetMouse()->GetAbsolutePosition().y;

	if (mousex > 525 && mousex < 735 && mousey > 160 && mousey < 210) {
		pBtn->SetFilename("buttonslide4.jpg");
		pBtn->SetTexture();
		pBtn->SetScale(Vector2(0.18f, 0.09f));;
		pBtn->SetPosition(Vector2(0, 0.45f));
		pBtn->Render();
	}

	if (mousex > 525 && mousex < 735 && mousey > 265 && mousey < 315) {
		pBtn->SetFilename("buttonslide5.jpg");
		pBtn->SetTexture();
		pBtn->SetScale(Vector2(0.18f, 0.09f));
		pBtn->SetPosition(Vector2(0, 0.15f));
		pBtn->Render();
	}


	if (mousex > 525 && mousex < 735 && mousey > 365 && mousey < 415) {
		pBtn->SetFilename("buttonslide6.jpg");
		pBtn->SetTexture();
		pBtn->SetScale(Vector2(0.18f, 0.09f));
		pBtn->SetPosition(Vector2(0, -0.15f));
		pBtn->Render();
	}

	if (mousex > 525 && mousex < 735 && mousey > 470 && mousey < 520) {
		pBtn->SetFilename("buttonslide7.jpg");
		pBtn->SetTexture();
		pBtn->SetScale(Vector2(0.18f, 0.09f));
		pBtn->SetPosition(Vector2(0, -0.45f));
		pBtn->Render();
	}
}