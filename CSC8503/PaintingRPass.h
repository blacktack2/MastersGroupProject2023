#pragma once
#include "OGLMainRenderPass.h"

#include "GameWorld.h"

#include <vector>

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503{
	class PaintingRPass : public OGLMainRenderPass {
	public:
		PaintingRPass(OGLRenderer& renderer);
		~PaintingRPass();

		void Render() override;

	protected:
		OGLShader* shader;
		OGLFrameBuffer* frameBuffer;
	};
}