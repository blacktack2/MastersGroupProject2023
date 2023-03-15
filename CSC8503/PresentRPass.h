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
		class PresentRPass : public OGLPresentRenderPass {
		public:
			PresentRPass();
			~PresentRPass();

			virtual void Render() override;

			virtual void SetSceneTexIn(TextureBase& sceneTex) override {
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
