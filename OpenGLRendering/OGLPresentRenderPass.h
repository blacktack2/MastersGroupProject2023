/**
 * @file   OGLPresentRenderPass.h
 * @brief  Base OpenGL implementation of the presentation render pass.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "IPresentRenderPass.h"

#include "OGLRenderer.h"

namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of the presentation render pass.
	 */
	class OGLPresentRenderPass : public OGLRenderPass, public IPresentRenderPass {
	public:
		OGLPresentRenderPass(OGLRenderer& renderer) : OGLRenderPass(renderer) {}

		virtual void OnWindowResize(int width, int height) override {
			OGLRenderPass::OnWindowResize(width, height);
		}
	};
}
