/**
 * @file   PresentRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"

#include "GameWorld.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class PresentRPass : public OGLRenderPass {
	public:
		PresentRPass(OGLRenderer& renderer, OGLTexture* sceneTexIn);
		~PresentRPass();

		virtual void Render() override;
	private:
		OGLTexture* sceneTexIn;

		OGLMesh* quad;

		OGLShader* shader;
	};
}
