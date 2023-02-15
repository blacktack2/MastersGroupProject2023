/**
 * @file   ICombineRenderPass.h
 * @brief  Interface class for the intermediary render pass which combines
 * the elements of the GBuffer into a single image, to then be sent to the
 * post-processing stage.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	class TextureBase;

	class ICombineRenderPass : public IRenderPass {
	public:
		virtual TextureBase* GetOutTex() const = 0;
	};
}
