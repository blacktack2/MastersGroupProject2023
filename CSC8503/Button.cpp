/**
 * @author Yifei Hu
 * @date   February 2023
 */
#include "Button.h"

#include "Debug.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLFrameBuffer.h"

using namespace NCL::CSC8503;

Button::Button(OGLRenderer& renderer, GameWorld& gameWorld) :
	OGLOverlayRenderPass(renderer), gameWorld(gameWorld) {
	quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-0.1, 0, -1),
		Vector3(-0.1, -0.1, -1),
		Vector3(0.1, -0.1, -1),
		Vector3(0.1, 0, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 1),
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->UploadToGPU();

	btnTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultbutton.jpg");

	btnShader = new OGLShader("buttonVertex.vert", "buttonFragment.frag");

	btnShader->Bind();
	glUniform1i(glGetUniformLocation(btnShader->GetProgramID(), "diffuseTex"), 0);
	btnShader->Unbind();

}

Button::~Button() {
	delete btnShader;
	delete btnTexture;
};

void Button::Render() {
	btnShader->Bind();
	
	glUniformMatrix4fv(glGetUniformLocation(btnShader->GetProgramID(), "modelMatrix"), 1, false, (GLfloat*)modelMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(btnShader->GetProgramID(), "viewMatrix"), 1, false, (GLfloat*)viewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(btnShader->GetProgramID(), "projMatrix"), 1, false, (GLfloat*)projMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(btnShader->GetProgramID(), "textureMatrix"), 1, false, (GLfloat*)textureMatrix.array);
	
	quad->SetVertexPositions({
		Vector3(position.x - scale.x, position.y + scale.y, -1),
		Vector3(position.x - scale.x, position.y - scale.y, -1),
		Vector3(position.x + scale.x, position.y - scale.y, -1),
		Vector3(position.x + scale.x, position.y + scale.y, -1),
		});
	quad->SetVertexColours({ colour, colour, colour, colour });
	quad->UploadToGPU();
	quad->Draw();

	btnTexture->Unbind();

	btnShader->Unbind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Button::SetTexture() {
	btnTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename(filename);
	btnTexture->Bind();
	glUniform1i(glGetUniformLocation(btnShader->GetProgramID(), "diffuseTex"), 0);
}