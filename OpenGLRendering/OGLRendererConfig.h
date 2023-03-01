/**
 * @file   OGLRendererConfig.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RendererConfigBase.h"

namespace NCL::Rendering {
	class OGLRendererConfig : public RendererConfigBase {
	public:
		OGLRendererConfig(RendererBase& renderer);

		void SetViewport(int x, int y, unsigned int width, unsigned int height) override;
		void SetClearColour(float r, float g, float b, float a) override;
		void SetAlphaTest(bool enabled, AlphaTestFunc func, float ref) override;
		void SetBlend(bool enabled, BlendFuncSrc srcFactor, BlendFuncDst dstFactor) override;
		void SetCullFace(bool enabled, CullFace mode) override;
		void SetDepthTest(bool enabled, DepthTestFunc func, float nearVal, float farVal) override;
		void SetDepthMask(bool enabled = false) override;
	};
}
