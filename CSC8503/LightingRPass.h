/**
 * @file   LightingRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLMainRenderPass.h"

#include "Light.h"

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
	class GameWorld;
	class GameTechRenderer;

	class LightingRPass : public OGLMainRenderPass {
	public:
		LightingRPass();
		~LightingRPass();

		virtual void OnWindowResize(int width, int height) override;

		virtual void Render() override;

		inline TextureBase& GetDiffuseOutTex() const {
			return *lightDiffuseOutTex;
		}
		inline TextureBase& GetSpecularOutTex() const {
			return *lightSpecularOutTex;
		}

		inline void SetDepthTexIn(TextureBase& tex) {
			depthTexIn = &tex;
		}

		inline void SetNormalTexIn(TextureBase& tex) {
			normalTexIn = &tex;
		}
	private:
		void DrawLight(const Light& light);

		GameTechRenderer& renderer;
		GameWorld& gameWorld;

		std::shared_ptr<MeshGeometry> sphere;
		std::shared_ptr<MeshGeometry> quad;

		std::unique_ptr<FrameBuffer> frameBuffer;

		TextureBase* depthTexIn = nullptr;
		TextureBase* normalTexIn = nullptr;

		std::unique_ptr<TextureBase> lightDiffuseOutTex;
		std::unique_ptr<TextureBase> lightSpecularOutTex;

		std::unique_ptr<ShaderBase> shader;
	};
}
