/**
 * @file   OGLPresentRenderPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "IPresentRenderPass.h"

#include "OGLRenderer.h"

namespace NCL::Rendering {
	class OGLPresentRenderPass : public OGLRenderPass, public IPresentRenderPass {
	public:
		OGLPresentRenderPass(OGLRenderer& renderer) : OGLRenderPass(renderer) {}

		virtual void OnWindowResize(int width, int height) override {
			OGLRenderPass::OnWindowResize(width, height);
		}
	};
}
