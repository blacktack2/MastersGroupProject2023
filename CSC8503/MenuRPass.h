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
		
		void LoadButton();
		void RenderButton();

	private:
		void SelectTexture(int n);

		GameWorld& gameWorld;
		OGLMesh* quad;
		OGLShader* lineShader;
		OGLShader* menuShader;
		OGLTexture* menuTexture;
		OGLTexture* diffuseOutTex;

		OGLFrameBuffer* frameBuffer;

		GLuint viewProjMatrixUniform;

		OGLShader* textShader;

		std::vector<Vector3> debugLineData;

		std::vector<Vector3> debugTextPos;
		std::vector<Vector4> debugTextColours;
		std::vector<Vector2> debugTextUVs;

		GLuint lineVAO;
		GLuint lineVertVBO;
		size_t lineCount;

		GLuint textVAO;
		GLuint textVertVBO;
		GLuint textColourVBO;
		GLuint textTexVBO;
		size_t textCount;

		//btn
		OGLShader* btnShader;
		OGLMesh* btn;

	};
}