/**
 * @file   BloomRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

namespace NCL::CSC8503 {
	class BloomRPass : public OGLRenderPass {
	public:
		BloomRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn);
		~BloomRPass();

		virtual void Render() override;

		void SetThreshold(float threshold);

		inline void SetBlurAmount(size_t amount) {
			blurAmount = amount;
		}

		inline OGLTexture* GetOutTex() const {
			return colourOutTex;
		}
	private:
		void DrawFilter();
		void ApplyBlur();
		void Combine();

		OGLFrameBuffer* filterFrameBuffer;
		OGLTexture* filterTex;

		OGLFrameBuffer* blurFrameBufferA;
		OGLTexture* blurTexA;

		OGLFrameBuffer* blurFrameBufferB;
		OGLTexture* blurTexB;

		OGLFrameBuffer* combineFrameBuffer;
		OGLTexture* colourOutTex;

		OGLTexture* sceneTexIn;

		OGLMesh* quad;

		OGLShader* filterShader;
		OGLShader* blurShader;
		OGLShader* combineShader;

		GLint horizontalUniform;
		GLint thresholdUniform;

		size_t blurAmount = 5;
	};
}
