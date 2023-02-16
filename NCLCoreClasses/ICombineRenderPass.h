/**
 * @file   ICombineRenderPass.h
 * @brief  Interface class for the combine stage of the rendering pipeline.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	class TextureBase;

	/**
	 * @brief Interface class for the combine stage of the rendering pipeline.
	 * @brief This pass must take in textures from the GBuffer and draw them to
	 * a single texture, to be output by the GetOutTex() method.
	 * @brief There can only be one of these at a given time.
	 */
	class ICombineRenderPass : public IRenderPass {
	public:
		virtual TextureBase* GetOutTex() const = 0;
	};
}
