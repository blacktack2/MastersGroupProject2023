/**
  * @author Yifei Hu
  * @date   February 2023
  */
#pragma once
#include "OGLOverlayRenderPass.h"

#include "GameWorld.h"

namespace NCL::Rendering {
	class OGLShader;
	class OGLTexture;
	class OGLFrameBuffer;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class Button : public OGLOverlayRenderPass {
	public:
		Button(OGLRenderer& renderer, GameWorld& gameWorld);
		~Button();

		virtual void Render() override;

		void SetTexture();

		inline void SetScale(Vector2 s) { scale = s; }
		inline void SetPosition(Vector2 p) { position = p; }
		inline void SetColour(Vector4 c) { colour = c; }
		inline void SetFilename(std::string f) { filename = f; }

	private:
		GameWorld& gameWorld;
		OGLMesh* quad;

		OGLShader* btnShader;
		OGLTexture* btnTexture;

		Vector2 scale;
		Vector2 position;
		Vector4 colour;
		std::string filename;
	};
}