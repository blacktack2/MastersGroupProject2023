/**
 * @author Yifei Hu
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "OGLOverlayRenderPass.h"

#include "GameWorld.h"
#include "UIObject.h"


namespace NCL::Rendering {
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class MenuRPass : public OGLOverlayRenderPass{
	public:
		MenuRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~MenuRPass();

		virtual void Render() override;
		
		void DrawMenu();
		void DrawUIObject(UIObject* obj);
		void DrawButtons();

	private:
		GameWorld& gameWorld;
		OGLMesh* quad;
		OGLShader* lineShader;
		OGLShader* defaultShader;
		OGLTexture* defaultTexture;

		//btn
		OGLShader* btnShader;
		OGLMesh* btn;

	};
}