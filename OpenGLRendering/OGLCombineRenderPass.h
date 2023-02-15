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

#include "OGLRenderer.h"

namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of the combine render pass.
	 */
	class OGLCombineRenderPass : public OGLRenderPass, public ICombineRenderPass {
	public:
		OGLCombineRenderPass(OGLRenderer& renderer) : OGLRenderPass(renderer) {}

		virtual void OnWindowResize(int width, int height) override {
			OGLRenderPass::OnWindowResize(width, height);
		}
	};
}
