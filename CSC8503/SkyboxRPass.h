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
	class SkyboxRPass : public OGLRenderPass {
	public:
		SkyboxRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~SkyboxRPass();

		virtual void Render() override;

		inline OGLTexture* GetOutTex() const {
			return colourOutTex;
		}
	private:
		GameWorld& gameWorld;

		OGLFrameBuffer* frameBuffer;
		OGLTexture* colourOutTex;

		OGLMesh* quad;
		OGLShader* shader;

		GLint viewMatrixUniform;
	};
}
