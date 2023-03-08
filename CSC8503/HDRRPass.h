/**
 * @file   HDRRPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLPostRenderPass.h"

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

	class HDRRPass : public OGLPostRenderPass {
	public:
		HDRRPass();
		~HDRRPass();

		virtual void Render() override;

		void SetExposure(float exposure);

		TextureBase& GetOutTex() const override {
			return *sceneOutTex;
		}

		void SetSceneTexIn(TextureBase& sceneTex) override {
			sceneTexIn = &sceneTex;
		}
	private:
		GameTechRenderer& renderer;

		std::shared_ptr<MeshGeometry> quad;

		std::unique_ptr<FrameBuffer> frameBuffer;

		TextureBase* sceneTexIn = nullptr;

		std::unique_ptr<TextureBase> sceneOutTex;

		std::unique_ptr<ShaderBase> shader;
	};
}
