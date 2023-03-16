/**
 * @file   OGLRenderPass.h
 * @brief  Base OpenGL implementation of a render pass.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RenderPassBase.h"

namespace NCL {
	namespace Rendering {
		/**
		 * @brief Base OpenGL implementation of a render pass.
		 */
		class OGLRenderPass : public RenderPassBase {
		public:
			virtual ~OGLRenderPass() {}
		protected:
			OGLRenderPass() {}
		};
	}
}
