#pragma once
#include "OGLMainRenderPass.h"

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
		class GameTechRenderer;
		class GameWorld;

		class PaintingRPass : public OGLMainRenderPass {
		public:
			PaintingRPass();
			~PaintingRPass();

			void Render() override;

		protected:
			GameTechRenderer& renderer;
			GameWorld& gameWorld;

			std::unique_ptr<FrameBuffer> frameBuffer;

			std::unique_ptr<ShaderBase> shader;
		};
	}
}