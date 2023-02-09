/**
 * @file   GBufferRPass.h
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
	class GBufferRPass : public OGLRenderPass {
	public:
		GBufferRPass(OGLRenderer& renderer,
			OGLTexture* skyboxTexIn, OGLTexture* diffuseTexIn,
			OGLTexture* diffuseLightTexIn, OGLTexture* specularLightTexIn,
			OGLTexture* normalTexIn, OGLTexture* depthTexIn);
		~GBufferRPass();

		virtual void Render() override;

		void SetRenderMode(RenderMode mode);

		inline OGLTexture* GetSceneOutTex() const {
			return sceneOutTex;
		}
		inline OGLTexture* GetBloomOutTex() const {
			return bloomOutTex;
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
		OGLTexture* bloomOutTex;

		OGLMesh* quad;

		OGLShader* shader;

		GLint modeUniform;
		GLint ambienceColourUniform;
	};
}
