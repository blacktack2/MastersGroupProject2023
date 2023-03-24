/**
 * @file   LightingRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLMainRenderPass.h"

#include <functional>
#include <memory>
#include <optional>

namespace NCL {
	class MeshGeometry;

	namespace Maths {
		class Matrix4;
	}

	namespace Rendering {
		class FrameBuffer;
		class ShaderBase;
		class TextureBase;
	}

	using namespace NCL::Maths;
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class GameWorld;
		class GameTechRenderer;

		class Light;

		class LightingRPass : public OGLMainRenderPass {
		public:
			LightingRPass();
			~LightingRPass() = default;

			void OnWindowResize(int width, int height) override;

			void Render() override;

			void AddShadowShader(std::shared_ptr<ShaderBase> shader);

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
			inline void SetSpecTexIn(TextureBase& tex) {
				specTexIn = &tex;
			}
		private:
			void DrawShadowMap(const Light& light, const Matrix4& shadowMatrix);
			void DrawLight(const Light& light, const Matrix4& shadowMatrix);

			GameTechRenderer& renderer;
			GameWorld& gameWorld;

			std::shared_ptr<MeshGeometry> sphere;
			std::shared_ptr<MeshGeometry> quad;

			std::unique_ptr<FrameBuffer> shadowFrameBuffer;
			std::unique_ptr<FrameBuffer> lightFrameBuffer;

			TextureBase* depthTexIn  = nullptr;
			TextureBase* normalTexIn = nullptr;
			TextureBase* specTexIn   = nullptr;

			std::unique_ptr<TextureBase> shadowMapTex;

			std::unique_ptr<TextureBase> lightDiffuseOutTex;
			std::unique_ptr<TextureBase> lightSpecularOutTex;

			std::unique_ptr<ShaderBase> lightShader;

			std::vector<std::shared_ptr<ShaderBase>> shadowShaders{};
		};
	}
}
