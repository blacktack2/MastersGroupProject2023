/**
 * @file   DebugRPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLOverlayRenderPass.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

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

	using namespace NCL::Maths;
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class GameTechRenderer;
		class GameWorld;

		class DebugRPass : public OGLOverlayRenderPass {
		public:
			DebugRPass();
			~DebugRPass() = default;

			void Render() override;

			void RenderWinLoseInformation(bool win);
		private:
			void RenderLines();
			void RenderText();

			GameTechRenderer& renderer;
			GameWorld& gameWorld;

			std::unique_ptr<ShaderBase> lineShader;
			std::unique_ptr<ShaderBase> textShader;

			std::vector<Vector3> debugLineData;

			std::vector<Vector3> debugTextPos;
			std::vector<Vector4> debugTextColours;
			std::vector<Vector2> debugTextUVs;

			std::unique_ptr<MeshGeometry> lineMesh;
			std::unique_ptr<MeshGeometry> textMesh;

			size_t lineCount;
			size_t textCount;
		};
	}
}
