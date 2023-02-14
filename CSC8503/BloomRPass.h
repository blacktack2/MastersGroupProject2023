/**
 * @file   BloomRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"

#include <vector>

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

		virtual void OnWindowResize(int width, int height) override;

		virtual void Render() override;

		void SetBloomDepth(size_t depth);

		void SetBias(float bias);

		inline OGLTexture* GetOutTex() const {
			return colourOutTex;
		}
	private:
		struct BloomMip {
			float width, height;
			OGLTexture* texture;
		};
		void Downsample();
		void Upsample();
		void Combine();

		OGLFrameBuffer* bloomFrameBuffer;
		std::vector<BloomMip> mipChain;

		OGLFrameBuffer* combineFrameBuffer;
		OGLTexture* colourOutTex;

		OGLTexture* sceneTexIn;

		OGLMesh* quad;

		OGLShader* downsampleShader;
		OGLShader* upsampleShader;
		OGLShader* combineShader;

		GLint sourcePixelSizeUniform;
		GLint filterRadiusUniform;
		GLint biasUniform;

		size_t bloomDepth = 5;
	};
}
