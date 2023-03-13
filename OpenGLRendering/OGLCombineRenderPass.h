/**
 * @file   OGLCombineRenderPass.h
 * @brief  Base OpenGL implementation of the combine render pass.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "ICombineRenderPass.h"

namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of the combine render pass.
	 */
	class OGLCombineRenderPass : public OGLRenderPass, public ICombineRenderPass {
	public:
		OGLCombineRenderPass() : OGLRenderPass() {}

		void WindowResize(int width, int height) override {
			OGLRenderPass::WindowResize(width, height);
		}
	protected:
		void OnWindowResize() override {
		}
	};
}
