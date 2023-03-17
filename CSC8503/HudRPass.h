/**
 * @file   HudRPass.h
 * @brief
 *
 * @author Shantao Liu
 * @author Xiaoyang Liu
 * @date   March 2023
 */

#pragma once
#include "OGLOverlayRenderPass.h"
#include "GameWorld.h"
#include "UIObject.h"
#include "Hud.h"
#include "OGLTexture.h"
#include "OGLShader.h"
namespace NCL::Rendering {
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class GameTechRenderer;
	class HudRPass : public OGLOverlayRenderPass {
	public:
		HudRPass();
		~HudRPass();

		virtual void Render() override;

		Hud& GetHud() {
			return hud;
		};
	private:
		GameTechRenderer& renderer;
		GameWorld& gameWorld;
		std::unique_ptr<ShaderBase> defaultShader;
		std::unique_ptr<TextureBase> defaultTexture;
		Hud hud;
	};
}