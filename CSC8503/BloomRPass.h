/**
 * @file   BloomRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLPostRenderPass.h"

#include "OGLTexture.h"

#include <vector>

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class TextureBase;
}

namespace NCL::CSC8503 {
	class BloomRPass : public OGLPostRenderPass {
	public:
		BloomRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn);
		~BloomRPass();

		virtual void OnWindowResize(int width, int height) override;

		virtual void Render() override;

		OGLTexture* GetOutTex() const override {
			return colourOutTex;
		}
		void SetSceneTexIn(TextureBase* sceneTex) override {
			sceneTexIn = static_cast<OGLTexture*>(sceneTex);
		}

		void SetBloomDepth(size_t depth);

		void SetBias(float bias);
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
