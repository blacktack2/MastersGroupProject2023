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
	class MenuRPass : public OGLOverlayRenderPass{
	public:
		MenuRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~MenuRPass();

		virtual void Render() override;

	private:
		GameWorld& gameWorld;
		OGLMesh* quad;

		OGLShader* menuShader;
		OGLTexture* menuTexture;

		//btn
		std::vector<Button*> pBtns;
		Button* pBtn;
		Vector2 position;

		//text
		void RenderText();
		void SetDebugStringBufferSizes(size_t newVertCount);

		std::vector<Vector3> debugTextPos;
		std::vector<Vector4> debugTextColours;
		std::vector<Vector2> debugTextUVs;

		OGLShader* textShader;

		GLuint textVAO;
		GLuint textVertVBO;
		GLuint textColourVBO;
		GLuint textTexVBO;
		size_t textCount;
	};
}