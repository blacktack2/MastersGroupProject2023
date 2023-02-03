/**
 * @file   OGLRenderPass.h
 * @brief  
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
	class OGLRenderPass : RenderPassBase {
	public:
		virtual ~OGLRenderPass();
	protected:
		OGLRenderPass(OGLRenderer& renderer);

		OGLRenderer& renderer;
	};
}
