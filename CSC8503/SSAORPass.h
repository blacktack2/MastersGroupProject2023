/**
 * @file   SSAORPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLMainRenderPass.h"

#include "Vector3.h"

#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <vector>

namespace NCL {
	class MeshGeometry;
}

namespace NCL::Rendering {
	class BufferObjectBase;
	class FrameBuffer;
	class ShaderBase;
	class TextureBase;
}

using namespace NCL;
using namespace Rendering;
using namespace Maths;

namespace NCL::CSC8503 {
	class GameTechRenderer;

	class SSAORPass : public OGLMainRenderPass {
	public:
		SSAORPass();
		~SSAORPass();

		virtual void OnWindowResize(int width, int height) override;

		virtual void Render() override;

		void SetRadius(float radius);
		void SetBias(float bias);
		void SetNumKernels(size_t numKernels);

		inline TextureBase& GetOutTex() const {
			return *ssaoOutTex;
		}

		inline void SetDepthTexIn(TextureBase& tex) {
			depthTexIn = tex;
		}

		inline void SetNormalTexIn(TextureBase& tex) {
			normalTexIn = tex;
		}
	private:
		void DrawSSAO();
		void BlurSSAO();

		void GenerateKernels();
		void GenerateNoiseTex();

		GameTechRenderer& renderer;

		std::shared_ptr<MeshGeometry> quad;

		std::unique_ptr<FrameBuffer> ssaoFrameBuffer;
		std::unique_ptr<FrameBuffer> blurFrameBuffer;

		std::optional<std::reference_wrapper<TextureBase>> depthTexIn;
		std::optional<std::reference_wrapper<TextureBase>> normalTexIn;

		std::unique_ptr<TextureBase> noiseTex;
		std::unique_ptr<TextureBase> ssaoTex;
		std::unique_ptr<TextureBase> ssaoOutTex;

		std::unique_ptr<ShaderBase> ssaoShader;
		std::unique_ptr<ShaderBase> blurShader;

		std::unique_ptr<BufferObjectBase> kernelSSBO;

		unsigned int noiseTexSize = 4;
		size_t numKernels = 64;
		std::vector<Vector3> kernels{};

		std::uniform_real_distribution<float> random = std::uniform_real_distribution(0.0f, 1.0f);
		std::default_random_engine generator;
	};
}
