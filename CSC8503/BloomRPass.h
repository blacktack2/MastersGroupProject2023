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
		BloomRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn, OGLTexture* bloomTexIn);
		~BloomRPass();

		virtual void Render() override;

		inline void SetBlurAmount(size_t amount) {
			blurAmount = amount;
		}

		inline OGLTexture* GetOutTex() const {
			return colourOutTex;
		}
	private:
		void ApplyBlur();
		void Combine();

		OGLFrameBuffer* blurFrameBufferA;
		OGLTexture* blurTexA;

		OGLFrameBuffer* blurFrameBufferB;
		OGLTexture* blurTexB;

		OGLFrameBuffer* combineFrameBuffer;
		OGLTexture* colourOutTex;

		OGLTexture* sceneTexIn;
		OGLTexture* bloomTexIn;

		OGLMesh* quad;

		OGLShader* blurShader;
		OGLShader* combineShader;

		GLint horizontalUniform;

		size_t blurAmount = 5;
	};
}
