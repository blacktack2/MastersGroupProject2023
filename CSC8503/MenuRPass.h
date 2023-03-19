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

#include <functional>
#include <memory>
#include <optional>

namespace NCL {
	namespace Rendering {
		class ShaderBase;
		class TextureBase;
	}

	using namespace NCL::Rendering;

	namespace CSC8503 {
		class GameTechRenderer;

		class MenuManager;

		class MenuRPass : public OGLOverlayRenderPass {
		public:
			MenuRPass();
			~MenuRPass() = default;

			virtual void Render() override;
		private:
			GameTechRenderer& renderer;
			MenuManager& menuManager;

			std::unique_ptr<TextureBase> defaultTexture;

			std::unique_ptr<ShaderBase> defaultShader;
		};
	}
}