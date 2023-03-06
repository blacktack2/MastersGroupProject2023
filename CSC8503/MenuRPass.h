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

namespace NCL::Rendering {
	class FrameBuffer;
	class ShaderBase;
	class TextureBase;
}

using namespace NCL;
using namespace Rendering;

namespace NCL::CSC8503 {
	class GameTechRenderer;
	class GameWorld;

	class UIObject;

	class MenuRPass : public OGLOverlayRenderPass {
	public:
		MenuRPass();
		~MenuRPass();

		virtual void Render() override;
	private:
		void DrawMenu();
		void DrawButtons();
		void DrawUIObject(UIObject* obj);

		GameTechRenderer& renderer;
		GameWorld& gameWorld;

		std::unique_ptr<TextureBase> defaultTexture;

		std::unique_ptr<ShaderBase> defaultShader;
	};
}