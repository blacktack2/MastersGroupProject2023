#include "PaintingRPass.h"

#include "GameTechRenderer.h"
#include "GameWorld.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "DebugViewPoint.h"
#include "CollisionVolume.h"
#include "PaintRenderObject.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Rendering;

PaintingRPass::PaintingRPass() : OGLMainRenderPass(),
debugView(DebugViewPoint::Instance()), gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {
	frameBuffer = AssetLoader::CreateFrameBuffer();

	shader = AssetLoader::CreateShaderAndInit("paintStencil.vert", "paintStencil.frag");
}

void PaintingRPass::Render() {
	frameBuffer->Bind();
	shader->Bind();

	renderer.GetConfig().SetCullFace(false);
	renderer.GetConfig().SetBlend(true, BlendFuncSrc::SrcAlpha, BlendFuncDst::OneMinusSrcAlpha);

	GameWorld& world = GameWorld::instance();

	world.OperateOnContents([&](GameObject* gameObject) {
		const CollisionVolume* volume = gameObject->GetBoundingVolume();
		if (!volume || volume->layer != CollisionLayer::PaintAble) {
			return;
		}

		PaintRenderObject* renderObj = (PaintRenderObject*)gameObject->GetRenderObject();

		frameBuffer->AddTexture(renderObj->GetPaintTexture(), 0);

		Matrix4 modelMatrix = renderObj->GetTransform().GetGlobalMatrix();
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

	renderer.GetConfig().SetViewport();

	renderer.GetConfig().SetBlend();
	renderer.GetConfig().SetCullFace();

	shader->Unbind();
	frameBuffer->Unbind();
}
