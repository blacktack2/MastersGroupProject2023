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
namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of the presentation render pass.
	 */
	class OGLPresentRenderPass : public OGLRenderPass, public IPresentRenderPass {
	public:
		OGLPresentRenderPass() : OGLRenderPass() {}

		void WindowResize(int width, int height) override {
			OGLRenderPass::WindowResize(width, height);
		}
	protected:
		void OnWindowResize() override {
		}
	};
}
