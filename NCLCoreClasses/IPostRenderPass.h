/**
 * @file   IPostRenderPass.h
 * @brief  Interface class for post-processing render passes.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	class TextureBase;

	class IPostRenderPass : public IRenderPass {
	public:
		virtual void SetSceneTexIn(TextureBase* sceneTex) = 0;
		virtual TextureBase* GetOutTex() const = 0;
	};
}
