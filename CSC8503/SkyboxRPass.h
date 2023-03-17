/**
 * @file   SkyboxRPass.h
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
		class Vector3;
	}

	namespace Rendering {
		class FrameBuffer;
		class ShaderBase;
		class TextureBase;
	}

	using namespace NCL::Maths;
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class GameTechRenderer;
		class GameWorld;

		class SkyboxRPass : public OGLMainRenderPass {
		public:
			SkyboxRPass();
			~SkyboxRPass() = default;

			void Render() override;

			void SetSunDir(const Vector3& direction);

			inline TextureBase& GetOutTex() const {
				return *colourOutTex;
			}
		private:
			GameTechRenderer& renderer;
			GameWorld& gameWorld;

			std::shared_ptr<MeshGeometry> quad;

			std::unique_ptr<FrameBuffer> frameBuffer;

			std::unique_ptr<TextureBase> colourOutTex;

			std::unique_ptr<ShaderBase> shader;
		};
	}
}
