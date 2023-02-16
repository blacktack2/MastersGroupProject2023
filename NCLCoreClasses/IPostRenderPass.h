/**
 * @file   IPostRenderPass.h
 * @brief  Interface class for post-processing stage render passes.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	class TextureBase;

	/**
	 * @brief Interface class for post-processing stage render passes.
	 * @brief This pass must take a pre-rendered scene in from the SetSceneTexIn
	 * method, and output a new texture with a post-processing effect applied to
	 * the GetOutTex() method.
	 */
	class IPostRenderPass : public IRenderPass {
	public:
		virtual void SetSceneTexIn(TextureBase* sceneTex) = 0;
		virtual TextureBase* GetOutTex() const = 0;
	};
}
