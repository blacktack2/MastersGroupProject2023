/**
 * @file   SkyboxRPass.h
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
	class SkyboxRPass : OGLRenderPass {
	public:
		SkyboxRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~SkyboxRPass();

		virtual void PreRender() override;
		virtual void Render() override;
	private:
		GameWorld& gameWorld;

		OGLMesh* quad;
		OGLShader* shader;
	};
}
