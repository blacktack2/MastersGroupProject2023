/**
 * @file   PresentRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLOverlayRenderPass.h"

#include "GameWorld.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class PresentRPass : public OGLOverlayRenderPass {
	public:
		PresentRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn);
		~PresentRPass();

		virtual void Render() override;

		void SetGamma(float gamma);
	private:
		OGLTexture* sceneTexIn;

		OGLMesh* quad;

		OGLShader* shader;

		GLint gammaUniform;
	};
}
