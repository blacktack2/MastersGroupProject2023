/**
 * @file   IOverlayRenderPass.h
 * @brief  Interface class for overlay stage render passes.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	/**
	 * @brief Interface class for overlay stage render passes.
	 * @brief This pass must draw directly to the backbuffer.
	 */
	class IOverlayRenderPass : public IRenderPass {
	};
}
