/**
 * @file   IPresentRenderPass.h
 * @brief  Interface class for the presentation pass of the rendering pipeline.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	class TextureBase;

	/**
	 * @brief Interface class for the presentation pass of the rendering
	 * pipeline.
	 * @brief This pass must take a texture from the post-processing stage,
	 * through the SetSceneTexIn() method, and draw it to the backbuffer with
	 * any gamma correction applied.
	 * @brief There can only be one of these at a given time.
	 */
	class IPresentRenderPass : public IRenderPass {
	public:
		virtual void SetSceneTexIn(TextureBase* sceneTex) = 0;
	};
}
