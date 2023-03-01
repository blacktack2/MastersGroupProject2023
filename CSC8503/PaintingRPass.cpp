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
	frameBuffer = new OGLFrameBuffer();

	shader = new OGLShader("paintStencil.vert", "paintStencil.frag");
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


	world.OperateOnContents([&](GameObject* gameObject) {
		const CollisionVolume* volume = gameObject->GetBoundingVolume();
		if (!volume) return;
		if (volume->layer != CollisionLayer::PaintAble) return;

		PaintRenderObject* renderObj = (PaintRenderObject*)gameObject->GetRenderObject();

		frameBuffer->AddTexture(renderObj->GetPaintTexture(), 0);

		Matrix4 modelMatrix = renderObj->GetTransform()->GetGlobalMatrix();
		shader->SetUniformMatrix("modelMatrix", modelMatrix);

		renderer.GetConfig().SetViewport(0, 0, renderObj->GetWidth(), renderObj->GetHeight());

		for (const PaintCollision& paint : renderObj->GetPaintCollisions()) {
			shader->SetUniformFloat("paintPos", paint.center);
			shader->SetUniformFloat("paintColour", paint.colour);
			shader->SetUniformFloat("paintSize", paint.radius);
			renderObj->GetMesh()->Draw();
		}

		renderObj->ClearPaintCollisions();
		
	});

	renderer.GetConfig().ResetViewport();

	glEnable(GL_CULL_FACE);
	shader->Unbind();
	frameBuffer->Unbind();

	debugView.FinishTime("Paint Objects");
}