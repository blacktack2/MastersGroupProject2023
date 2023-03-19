/**
 * @file   PresentRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLPresentRenderPass.h"

#include <functional>
#include <memory>
#include <optional>

namespace NCL {
	class MeshGeometry;

	namespace Rendering {
		class ShaderBase;
		class TextureBase;
	}

	using namespace NCL::Rendering;

	namespace CSC8503 {
		class PresentRPass : public OGLPresentRenderPass {
		public:
			PresentRPass();
			~PresentRPass() = default;

			void Render() override;

			void SetSceneTexIn(TextureBase& sceneTex) override {
				sceneTexIn = &sceneTex;
			}

			void SetGamma(float gamma);
		private:
			std::shared_ptr<MeshGeometry> quad;

			TextureBase* sceneTexIn = nullptr;

			std::unique_ptr<ShaderBase> shader;
		};
	}
}
