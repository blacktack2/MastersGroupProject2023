/**
 * @file   BloomRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLPostRenderPass.h"

#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace NCL {
	class MeshGeometry;
}

namespace NCL::Rendering {
	class FrameBuffer;
	class ShaderBase;
	class TextureBase;
}

using namespace NCL;
using namespace Rendering;

namespace NCL::CSC8503 {
	class GameTechRenderer;

	class BloomRPass : public OGLPostRenderPass {
	public:
		BloomRPass();
		~BloomRPass();

		virtual void Render() override;

		TextureBase& GetOutTex() const override {
			return *colourOutTex;
		}
		void SetSceneTexIn(TextureBase& sceneTex) override {
			sceneTexIn = &sceneTex;
		}

		void SetBloomDepth(size_t depth);

		void SetBias(float bias);
	protected:
		virtual void OnWindowResize() override;
	private:
		struct BloomMip {
			float width, height;
			std::unique_ptr<TextureBase> texture;
		};
		void Downsample();
		void Upsample();
		void Combine();

		GameTechRenderer& renderer;

		std::shared_ptr<MeshGeometry> quad;

		std::unique_ptr<FrameBuffer> bloomFrameBuffer;
		std::unique_ptr<FrameBuffer> combineFrameBuffer;

		TextureBase* sceneTexIn = nullptr;

		std::vector<BloomMip> mipChain;
		std::unique_ptr<TextureBase> colourOutTex;

		std::unique_ptr<ShaderBase> downsampleShader;
		std::unique_ptr<ShaderBase> upsampleShader;
		std::unique_ptr<ShaderBase> combineShader;

		size_t bloomDepth = 5;
	};
}
