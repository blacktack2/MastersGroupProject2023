#include "PaintingRPass.h"
#include "OGLShader.h"
#include "OGLFrameBuffer.h"
#include "GameWorld.h"
#include "CollisionVolume.h"
#include "PaintRenderObject.h"
#include "DebugViewPoint.h"

using namespace NCL::CSC8503;
using namespace NCL::Rendering;
using namespace paintHell::debug;

PaintingRPass::PaintingRPass(OGLRenderer& renderer) : OGLMainRenderPass(renderer) {
	shader = new OGLShader("paintStencil.vert", "paintStencil.frag");
	frameBuffer = new OGLFrameBuffer();
}
PaintingRPass::~PaintingRPass() {
	delete shader;
	delete frameBuffer;
}

void PaintingRPass::Render() {
	DebugViewPoint& debugView = DebugViewPoint::Instance();
	debugView.MarkTime("Paint Objects");
	frameBuffer->Bind();
	shader->Bind();
	glDisable(GL_CULL_FACE);

	GameWorld& world = GameWorld::instance();

	GLint paintPos = glGetUniformLocation(shader->GetProgramID(), "paintPos");
	GLint paintSize = glGetUniformLocation(shader->GetProgramID(), "paintSize");
	GLint paintColour = glGetUniformLocation(shader->GetProgramID(), "paintColour");

	world.OperateOnContents([&](GameObject* gameObject) {
		const CollisionVolume* volume = gameObject->GetBoundingVolume();
		if (!volume) return;
		if (volume->layer != CollisionLayer::PaintAble) return;

		PaintRenderObject* renderObj = (PaintRenderObject*)gameObject->GetRenderObject();

		frameBuffer->AddTexture(renderObj->GetPaintTexture(), 0);

		Matrix4 modelMatrix = renderObj->GetTransform()->GetGlobalMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "modelMatrix"), 1, GL_FALSE, (GLfloat*)&modelMatrix);

		glViewport(0, 0, renderObj->GetWidth(), renderObj->GetHeight());

		for (PaintCollision const paint : renderObj->GetPaintCollisions())
		{
			glUniform3fv(paintPos, 1, (GLfloat*)&paint.center);
			glUniform3fv(paintColour, 1, (GLfloat*)&paint.colour);
			glUniform1f(paintSize, paint.radius);
			renderObj->GetMesh()->Draw();
		}

		//renderObj->ClearPaintCollisions();
		
	});

	glViewport(0, 0, renderer.GetWidth(), renderer.GetHeight());

	glEnable(GL_CULL_FACE);
	shader->Unbind();
	frameBuffer->Unbind();
	debugView.FinishTime("Paint Objects");
}