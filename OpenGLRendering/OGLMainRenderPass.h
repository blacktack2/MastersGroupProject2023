/**
 * @file   OGLMainRenderPass.h
 * @brief  Base OpenGL implementation of a main stage render pass.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "IMainRenderPass.h"

namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of a main stage render pass.
	 */
	class OGLMainRenderPass : public OGLRenderPass, public IMainRenderPass {
	public:
		OGLMainRenderPass() : OGLRenderPass() {}

		void WindowResize(int width, int height) override {
			OGLRenderPass::WindowResize(width, height);
		}
	protected:
		void OnWindowResize() override {
		}
	};
}
