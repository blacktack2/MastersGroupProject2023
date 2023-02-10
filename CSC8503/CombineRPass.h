/**
 * @file   CombineRPass.h
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
	enum class RenderMode {
		Default = 0,
		Normals,
		Depth,
		Diffuse,
		DiffuseLight,
		SpecularLight,
	};
	class CombineRPass : public OGLRenderPass {
	public:
		CombineRPass(OGLRenderer& renderer,
			OGLTexture* skyboxTexIn, OGLTexture* diffuseTexIn,
			OGLTexture* diffuseLightTexIn, OGLTexture* specularLightTexIn,
			OGLTexture* normalTexIn, OGLTexture* depthTexIn);
		~CombineRPass();

		virtual void Render() override;

		void SetRenderMode(RenderMode mode);

		inline OGLTexture* GetOutTex() const {
			return sceneOutTex;
		}
	private:
		OGLFrameBuffer* frameBuffer;

		OGLTexture* skyboxTexIn;
		OGLTexture* diffuseTexIn;
		OGLTexture* diffuseLightTexIn;
		OGLTexture* specularLightTexIn;
		OGLTexture* normalTexIn;
		OGLTexture* depthTexIn;

		OGLTexture* sceneOutTex;

		OGLMesh* quad;

		OGLShader* shader;

		GLint modeUniform;
		GLint ambienceColourUniform;
	};
}
