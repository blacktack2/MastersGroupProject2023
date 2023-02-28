/**
 * @file   MenuRPass.cpp
 * @brief  
 * 
 * @author Yifei Hu
 * @author Felix Chiu
 * @author Stuart Lewis
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
	class MenuRPass : public OGLOverlayRenderPass {
	public:
		MenuRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~MenuRPass();

		virtual void Render() override;
	private:
		void DrawMenu();
		void DrawButtons();
		void DrawUIObject(UIObject* obj);

		GameWorld& gameWorld;

		ShaderBase* defaultShader;
		TextureBase* defaultTexture;
	};
}