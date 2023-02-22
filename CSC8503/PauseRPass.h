/**
 * @author Yifei Hu
 * @date   February 2023
 */
#pragma once
#include "OGLOverlayRenderPass.h"

#include "GameWorld.h"
#include "Button.h"

namespace NCL::Rendering {
	class OGLShader;
	class OGLTexture;
	class OGLFrameBuffer;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class PauseRPass : public OGLOverlayRenderPass {
	public:
		PauseRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~PauseRPass();

		virtual void Render() override;

		void Update();
	private:
		GameWorld& gameWorld;
		OGLMesh* quad;
		OGLShader* pauseShader;
		OGLTexture* pauseTexture;

		//Button
		Button* pBtn;
		Vector2 position;
	};
}
