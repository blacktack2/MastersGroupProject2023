/**
 * @file   CombineRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLCombineRenderPass.h"

#include "OGLTexture.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
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
		AmbientOcclusion,
	};
	class CombineRPass : public OGLCombineRenderPass {
	public:
		CombineRPass(OGLRenderer& renderer,
			OGLTexture* skyboxTexIn, OGLTexture* diffuseTexIn,
			OGLTexture* diffuseLightTexIn, OGLTexture* specularLightTexIn, OGLTexture* ssaoTexIn,
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
		OGLTexture* ssaoTexIn;
		OGLTexture* normalTexIn;
		OGLTexture* depthTexIn;

		OGLTexture* sceneOutTex;

		OGLMesh* quad;

		OGLShader* shader;
	};
}
