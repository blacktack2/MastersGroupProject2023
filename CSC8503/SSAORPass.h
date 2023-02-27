/**
 * @file   SSAORPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLMainRenderPass.h"

#include "GameWorld.h"
#include "Light.h"

#include <random>
#include <vector>

namespace NCL::Rendering {
	class OGLBufferObject;
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class SSAORPass : public OGLMainRenderPass {
	public:
		SSAORPass(OGLRenderer& renderer, OGLTexture* depthTexIn, OGLTexture* normalTexIn);
		~SSAORPass();

		virtual void OnWindowResize(int width, int height) override;

		virtual void Render() override;

		void SetRadius(float radius);
		void SetBias(float bias);
		void SetNumKernels(size_t numKernels);

		inline OGLTexture* GetOutTex() const {
			return ssaoOutTex;
		}
	private:
		void DrawSSAO();
		void BlurSSAO();

		void GenerateKernels();
		void GenerateNoiseTex();

		OGLFrameBuffer* ssaoFrameBuffer;
		OGLTexture* ssaoTex;

		OGLFrameBuffer* blurFrameBuffer;
		OGLTexture* ssaoOutTex;

		OGLTexture* depthTexIn;
		OGLTexture* normalTexIn;
		OGLTexture* noiseTex = nullptr;

		OGLMesh* quad;

		OGLShader* ssaoShader;
		OGLShader* blurShader;

		OGLBufferObject* kernelSSBO;

		GLsizei noiseTexSize = 4;
		size_t numKernels = 64;
		std::vector<Vector3> kernels{};

		std::uniform_real_distribution<float> random = std::uniform_real_distribution(0.0f, 1.0f);
		std::default_random_engine generator;
	};
}

