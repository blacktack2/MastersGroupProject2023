/**
 * @file   ModelRPass.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"

#include "GameWorld.h"

#include <vector>

namespace NCL::Rendering {
	class OGLFrameBuffer;
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class ModelRPass : public OGLRenderPass {
	public:
		ModelRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~ModelRPass();

		virtual void Render() override;

		void AddModelShader(OGLShader* shader);

		inline void SetGamma(float g) {
			gamma = g;
		}

		inline OGLTexture* GetDiffuseOutTex() const {
			return diffuseOutTex;
		}
		inline OGLTexture* GetNormalOutTex() const {
			return normalOutTex;
		}
		inline OGLTexture* GetDepthOutTex() const {
			return depthOutTex;
		}
	private:
		GameWorld& gameWorld;

		OGLFrameBuffer* frameBuffer;
		OGLTexture* diffuseOutTex;
		OGLTexture* normalOutTex;
		OGLTexture* depthOutTex;

		OGLTexture* defaultDiffuse;
		OGLTexture* defaultBump;

		OGLShader* defaultShader;

		float gamma = 2.2f;

		std::vector<OGLShader*> modelShaders{};
	};
}
