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

	namespace Rendering {
		class FrameBuffer;
		class ShaderBase;
		class TextureBase;
	}

	using namespace NCL::Rendering;

	namespace CSC8503 {
		class GameTechRenderer;

		enum class RenderMode {
			Default = 0,
			Normals,
			Depth,
			Albedo,
			Diffuse,
			Specular,
			AmbientOcclusion,
		};
		class CombineRPass : public OGLCombineRenderPass {
		public:
			CombineRPass();
			~CombineRPass() = default;

			void Render() override;

			void SetRenderMode(RenderMode mode);

			inline TextureBase& GetOutTex() const override {
				return *sceneOutTex;
			}

			inline void SetSkyboxTexIn(TextureBase& tex) {
				skyboxTexIn = &tex;
			}
			inline void SetAlbedoTexIn(TextureBase& tex) {
				albedoTexIn = &tex;
			}
			inline void SetDiffuseTexIn(TextureBase& tex) {
				diffuseTexIn = &tex;
			}
			inline void SetSpecularTexIn(TextureBase& tex) {
				specularTexIn = &tex;
			}
			inline void SetSSAOTexIn(TextureBase& tex) {
				ssaoTexIn = &tex;
			}
			inline void SetNormalTexIn(TextureBase& tex) {
				normalTexIn = &tex;
			}
			inline void SetDepthTexIn(TextureBase& tex) {
				depthTexIn = &tex;
			}
		private:
			GameTechRenderer& renderer;

			std::shared_ptr<MeshGeometry> quad;

			std::unique_ptr<FrameBuffer> frameBuffer;

			TextureBase* skyboxTexIn   = nullptr;
			TextureBase* albedoTexIn   = nullptr;
			TextureBase* diffuseTexIn  = nullptr;
			TextureBase* specularTexIn = nullptr;
			TextureBase* ssaoTexIn     = nullptr;
			TextureBase* normalTexIn   = nullptr;
			TextureBase* depthTexIn    = nullptr;

			std::unique_ptr<TextureBase> sceneOutTex;

			std::unique_ptr<ShaderBase> shader;
		};
	}
}
