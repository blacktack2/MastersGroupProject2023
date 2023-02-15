/**
 * @file   PresentRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLPresentRenderPass.h"

#include "GameWorld.h"

#include "OGLTexture.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class PresentRPass : public OGLPresentRenderPass {
	public:
		PresentRPass(OGLRenderer& renderer);
		~PresentRPass();

		virtual void Render() override;

		virtual void SetSceneTexIn(TextureBase* sceneTex) override {
			sceneTexIn = static_cast<OGLTexture*>(sceneTex);
		}

		void SetGamma(float gamma);
	private:
		OGLTexture* sceneTexIn;

		OGLMesh* quad;

		OGLShader* shader;

		GLint gammaUniform;
	};
}
