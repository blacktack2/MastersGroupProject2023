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
#include "Hud.h"

namespace NCL::Rendering {
	class ShaderBase;
	class TextureBase;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class GameWorld;
	class GameTechRenderer;

	class HudRPass : public OGLOverlayRenderPass {
	public:
		HudRPass();
		~HudRPass();

		virtual void Render() override;

	private:
		GameTechRenderer& renderer;
		GameWorld& gameWorld;
		std::unique_ptr<ShaderBase> defaultShader;
		std::unique_ptr<TextureBase> defaultTexture;
		Hud hud;
	};
}