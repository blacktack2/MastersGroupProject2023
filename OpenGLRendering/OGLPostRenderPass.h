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

namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of a post-process stage render pass.
	 */
	class OGLPostRenderPass : public OGLRenderPass, public IPostRenderPass {
	public:
		OGLPostRenderPass() : OGLRenderPass() {}

		void WindowResize(int width, int height) override {
			OGLRenderPass::WindowResize(width, height);
		}
	protected:
		void OnWindowResize() override {
		}
	};
}
