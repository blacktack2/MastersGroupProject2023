#include"HudRPass.h"
#include "GameTechRenderer.h"
#include "GameWorld.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include "Debug.h"
#include "UIObject.h"
using namespace NCL;
using namespace CSC8503;
HudRPass::HudRPass():OGLOverlayRenderPass(),
gameWorld(GameWorld::instance()), renderer(GameTechRenderer::instance()) {

	//defaultTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultmain.jpg");
	defaultTexture = AssetLoader::LoadTexture("defaultmain.jpg");

	defaultShader = AssetLoader::CreateShaderAndInit("menuVertex.vert", "menuFragment.frag");

	defaultShader->Bind();

	defaultShader->SetUniformInt("diffuseTex", 0);

	defaultShader->Unbind();
};
HudRPass::~HudRPass() {
};
void HudRPass::Render() {
	renderer.GetConfig().SetCullFace(false);

	//if (BossBorder) {
	//	DrawUIObject((UIObject*)BossBorder);
	//}
	//if(BossHealth){ 
	//	DrawUIObject((UIObject*)BossHealth);
	//}

	for (auto& hud : huds) {
		//DrawUIObject((UIObject*)hud);
		hud->Draw();
	}

	renderer.GetConfig().SetCullFace();
}
//void HudRPass::DrawHud() {
//	DrawUIObject((UIObject*)health);
//}
//void HudRPass::DrawUIObject(UIObject* obj) {
//	MenuRenderObject* renderObject = obj->GetRenderObject();
//	if (!renderObject) {
//		return;
//	}
//	//std::cout << obj->GetDimension() << std::endl;
//	renderObject->Draw(obj->GetDimension());
//}
