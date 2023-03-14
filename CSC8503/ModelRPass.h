/**
 * @file   ModelRPass.h
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
#include <vector>

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

		class ModelRPass : public OGLMainRenderPass {
		public:
			ModelRPass();
			~ModelRPass();

			virtual void Render() override;

			void AddModelShader(std::shared_ptr<ShaderBase> shader);

			inline void SetGamma(float g) {
				gamma = g;
			}

			inline TextureBase& GetAlbedoOutTex() const {
				return *albedoOutTex;
			}
			inline TextureBase& GetNormalOutTex() const {
				return *normalOutTex;
			}
			inline TextureBase& GetDepthOutTex() const {
				return *depthOutTex;
			}
		private:
			GameTechRenderer& renderer;
			GameWorld& gameWorld;

			std::unique_ptr<FrameBuffer> frameBuffer;

			std::unique_ptr<TextureBase> albedoOutTex;
			std::unique_ptr<TextureBase> normalOutTex;
			std::unique_ptr<TextureBase> depthOutTex;

			std::vector<std::shared_ptr<ShaderBase>> modelShaders{};

			float gamma = 2.2f;
		};
	}
}
