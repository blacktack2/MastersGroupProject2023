/**
 * @author Yifei Hu
 * @date   February 2023
 */
#include "PauseRPass.h"

#include "OGLFrameBuffer.h"
#include "Debug.h"
#include "OGLShader.h"
#include "OGLTexture.h"

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
}

void PauseRPass::Render() {
	pauseShader->Bind();

	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(pauseShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);
	pauseTexture->Bind(0);

	quad->Draw();

	pauseShader->Unbind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}