/**
 * @file   OGLMainRenderPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"
#include "IMainRenderPass.h"

#include "OGLRenderer.h"

namespace NCL::Rendering {
	class OGLMainRenderPass : public OGLRenderPass, public IMainRenderPass {
	public:
		OGLMainRenderPass(OGLRenderer& renderer) : OGLRenderPass(renderer) {}

		virtual void OnWindowResize(int width, int height) override {
			OGLRenderPass::OnWindowResize(width, height);
		}
	};
}
