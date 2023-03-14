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
	class MeshGeometry;
}

namespace NCL {
	namespace Rendering {
		class FrameBuffer;
		class ShaderBase;
		class TextureBase;
	}
}

using namespace NCL;
using namespace Rendering;

namespace NCL {
	namespace CSC8503 {
		class GameTechRenderer;
		class GameWorld;

		class MenuManager;
		class UIObject;

		class MenuRPass : public OGLOverlayRenderPass {
		public:
			MenuRPass();
			~MenuRPass();

			virtual void Render() override;
		private:
			GameTechRenderer& renderer;
			GameWorld& gameWorld;
			MenuManager& menuManager;

			std::unique_ptr<TextureBase> defaultTexture;

			std::unique_ptr<ShaderBase> defaultShader;
		};
	}
}