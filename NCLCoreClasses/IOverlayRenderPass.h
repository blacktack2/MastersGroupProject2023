/**
 * @file   IOverlayRenderPass.h
 * @brief  Interface class for overlay render passes (writing directly to the
 * backbuffer after post-processing).
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL::Rendering {
	class IOverlayRenderPass : public IRenderPass {
	};
}
