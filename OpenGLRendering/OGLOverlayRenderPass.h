/**
 * @file   OGLOverlayRenderPass.h
 * @brief  Base OpenGL implementation of an overlay stage render pass.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "IOverlayRenderPass.h"

#include "OGLRenderer.h"

namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of an overlay stage render pass.
	 */
	class OGLOverlayRenderPass : public OGLRenderPass, public IOverlayRenderPass {
	public:
		OGLOverlayRenderPass(OGLRenderer& renderer) : OGLRenderPass(renderer) {}

		virtual void OnWindowResize(int width, int height) override {
			OGLRenderPass::OnWindowResize(width, height);
		}
	};
}
