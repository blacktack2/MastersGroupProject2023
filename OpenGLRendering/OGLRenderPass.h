/**
 * @file   OGLRenderPass.h
 * @brief  Base OpenGL implementation of a render pass.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RenderPassBase.h"

#include "OGLMesh.h"
#include "OGLRenderer.h"

#include "glad/gl.h"


namespace NCL::Rendering {
	/**
	 * @brief Base OpenGL implementation of a render pass.
	 */
	class OGLRenderPass : public RenderPassBase {
	public:
		virtual ~OGLRenderPass();
	protected:
		OGLRenderPass(OGLRenderer& renderer);

		OGLRenderer& renderer;
	};
}
