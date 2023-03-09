/**
 * @file   SkyboxRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLMainRenderPass.h"

#include "Vector3.h"

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
using namespace Maths;

namespace NCL::CSC8503 {
	class GameTechRenderer;
	class GameWorld;

	class SkyboxRPass : public OGLMainRenderPass {
	public:
		SkyboxRPass();
		~SkyboxRPass();

		virtual void Render() override;

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
