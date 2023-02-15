/**
 * @file   OGLPostRenderPass.h
 * @brief  Base OpenGL implementation of a post-process stage render pass.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "IPostRenderPass.h"

#include "OGLRenderer.h"

namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of a post-process stage render pass.
	 */
	class OGLPostRenderPass : public OGLRenderPass, public IPostRenderPass {
	public:
		OGLPostRenderPass(OGLRenderer& renderer) : OGLRenderPass(renderer) {}

		virtual void OnWindowResize(int width, int height) override {
			OGLRenderPass::OnWindowResize(width, height);
		}
	};
}
