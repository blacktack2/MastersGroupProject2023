/**
 * @file   CombineRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLCombineRenderPass.h"

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

	enum class RenderMode {
		Default = 0,
		Normals,
		Depth,
		Diffuse,
		DiffuseLight,
		SpecularLight,
		AmbientOcclusion,
	};
	class CombineRPass : public OGLCombineRenderPass {
	public:
		CombineRPass();
		~CombineRPass();

		virtual void Render() override;

		void SetRenderMode(RenderMode mode);

		inline TextureBase& GetOutTex() const override {
			return *sceneOutTex;
		}

		inline void SetSkyboxTexIn(TextureBase& tex) {
			skyboxTexIn = tex;
		}
		inline void SetDiffuseTexIn(TextureBase& tex) {
			diffuseTexIn = tex;
		}
		inline void SetDiffuseLightTexIn(TextureBase& tex) {
			diffuseLightTexIn = tex;
		}
		inline void SetSpecularLightTexIn(TextureBase& tex) {
			specularLightTexIn = tex;
		}
		inline void SetSSAOTexIn(TextureBase& tex) {
			ssaoTexIn = tex;
		}
		inline void SetNormalTexIn(TextureBase& tex) {
			normalTexIn = tex;
		}
		inline void SetDepthTexIn(TextureBase& tex) {
			depthTexIn = tex;
		}
	private:
		GameTechRenderer& renderer;

		std::shared_ptr<MeshGeometry> quad;

		std::unique_ptr<FrameBuffer> frameBuffer;

		std::optional<std::reference_wrapper<TextureBase>> skyboxTexIn;
		std::optional<std::reference_wrapper<TextureBase>> diffuseTexIn;
		std::optional<std::reference_wrapper<TextureBase>> diffuseLightTexIn;
		std::optional<std::reference_wrapper<TextureBase>> specularLightTexIn;
		std::optional<std::reference_wrapper<TextureBase>> ssaoTexIn;
		std::optional<std::reference_wrapper<TextureBase>> normalTexIn;
		std::optional<std::reference_wrapper<TextureBase>> depthTexIn;

		std::unique_ptr<TextureBase> sceneOutTex;

		std::unique_ptr<ShaderBase> shader;
	};
}
