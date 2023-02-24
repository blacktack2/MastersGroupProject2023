/**
 * @file   HDRRPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLPostRenderPass.h"

#include "OGLTexture.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class TextureBase;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class HDRRPass : public OGLPostRenderPass {
	public:
		HDRRPass(OGLRenderer& renderer);
		~HDRRPass();

		virtual void Render() override;

		void SetExposure(float exposure);

		OGLTexture* GetOutTex() const override {
			return sceneOutTex;
		}

		void SetSceneTexIn(TextureBase* sceneTex) override {
			sceneTexIn = static_cast<OGLTexture*>(sceneTex);
		}
	private:
		OGLFrameBuffer* frameBuffer;
		OGLTexture* sceneOutTex;

		OGLTexture* sceneTexIn;

		OGLMesh* quad;

		OGLShader* shader;
	};
}
