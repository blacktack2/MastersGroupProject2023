/**
 * @file   HDRRPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class HDRRPass : public OGLRenderPass {
	public:
		HDRRPass(OGLRenderer& renderer, OGLTexture* sceneIn);
		~HDRRPass();

		virtual void Render() override;

		void SetExposure(float exposure);

		inline OGLTexture* GetOutTex() const {
			return sceneOutTex;
		}
	private:
		OGLFrameBuffer* frameBuffer;
		OGLTexture* sceneOutTex;

		OGLTexture* sceneTexIn;

		OGLMesh* quad;

		OGLShader* shader;

		GLint exposureUniform;
	};
}
