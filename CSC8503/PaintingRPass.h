#pragma once
#include "OGLMainRenderPass.h"

#include <functional>
#include <memory>
#include <optional>

namespace NCL {
	class DebugViewPoint;

	namespace Rendering {
		class FrameBuffer;
		class ShaderBase;
	}

	using namespace NCL::Rendering;

	namespace CSC8503 {
		class GameTechRenderer;
		class GameWorld;

		class PaintingRPass : public OGLMainRenderPass {
		public:
			PaintingRPass();
			~PaintingRPass() = default;

			void Render() override;
		protected:
			DebugViewPoint& debugView;
			GameTechRenderer& renderer;
			GameWorld& gameWorld;

			std::unique_ptr<FrameBuffer> frameBuffer;

			std::unique_ptr<ShaderBase> shader;
		};
	}
}