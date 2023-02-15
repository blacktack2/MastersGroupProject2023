/**
 * @file   IPresentRenderPass.h
 * @brief  Interface class for then intermediary render pass which draws a full
 * scene to the backbuffer, to then be followed by the overlay stage.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	class TextureBase;

	class IPresentRenderPass : public IRenderPass {
	public:
		virtual void SetSceneTexIn(TextureBase* sceneTex) = 0;
	};
}
