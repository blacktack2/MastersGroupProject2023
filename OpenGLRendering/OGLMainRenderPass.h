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

namespace NCL {
	namespace Rendering {
		/**
		 * @brief Base OpenGL implementation of a main stage render pass.
		 */
		class OGLMainRenderPass : public OGLRenderPass, public IMainRenderPass {
		public:
			OGLMainRenderPass() : OGLRenderPass() {}

			virtual void OnWindowResize(int width, int height) override {
				OGLRenderPass::OnWindowResize(width, height);
			}
		};
	}
}
