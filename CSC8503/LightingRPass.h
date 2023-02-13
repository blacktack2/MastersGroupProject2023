/**
 * @file   LightingRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"

#include "GameWorld.h"
#include "Light.h"

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class LightingRPass : public OGLRenderPass {
	public:
		LightingRPass(OGLRenderer& renderer, GameWorld& gameWorld, OGLTexture* depthTexIn, OGLTexture* normalTexIn);
		~LightingRPass();

		virtual void OnWindowResize(int width, int height) override;

		virtual void Render() override;

		inline OGLTexture* GetDiffuseOutTex() const {
			return lightDiffuseOutTex;
		}
		inline OGLTexture* GetSpecularOutTex() const {
			return lightSpecularOutTex;
		}
	private:
		void DrawLight(const Light& light);

		GameWorld& gameWorld;

		OGLFrameBuffer* frameBuffer;
		OGLTexture* lightDiffuseOutTex;
		OGLTexture* lightSpecularOutTex;

		OGLTexture* depthTexIn;
		OGLTexture* normalTexIn;

		OGLMesh* sphere;
		OGLMesh* quad;

		OGLShader* shader;

		GLint cameraPosUniform;
		GLint pixelSizeUniform;
		GLint inverseProjViewUniform;

		GLint lightPositionUniform;
		GLint lightColourUniform;
		GLint lightRadiusUniform;
		GLint lightDirectionUniform;
		GLint lightAngleUniform;

		GLint modelMatrixUniform;
		GLint viewMatrixUniform;
		GLint projMatrixUniform;
	};
}