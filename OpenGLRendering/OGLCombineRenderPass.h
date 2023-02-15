/**
 * @file   OGLCombineRenderPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "ICombineRenderPass.h"

#include "OGLRenderer.h"

namespace NCL::Rendering {
	class OGLCombineRenderPass : public OGLRenderPass, public ICombineRenderPass {
	public:
		OGLCombineRenderPass(OGLRenderer& renderer) : OGLRenderPass(renderer) {}

		virtual void OnWindowResize(int width, int height) override {
			OGLRenderPass::OnWindowResize(width, height);
		}
	};
}
