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
	class PauseRPass : public OGLOverlayRenderPass {
	public:
		PauseRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~PauseRPass();

		virtual void Render() override;
	private:
		void SetDebugStringBufferSizes(size_t newVertCount);
		void SetDebugLineBufferSizes(size_t newVertCount);

		void RenderLines();
		void RenderText();

		GameWorld& gameWorld;
		OGLMesh* quad;
		OGLShader* lineShader;
		OGLShader* pauseShader;
		OGLTexture* pauseTexture;
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

	};
}
