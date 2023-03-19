/**
 * @file   IMainRenderPass.h
 * @brief  Interface class for main stage render passes.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "IRenderPass.h"

namespace NCL {
	namespace Rendering {
		/**
		 * @brief Interface class for main stage render passes.
		 * @brief This pass must create elements of the GBuffer as textures which
		 * should be bound to the combine pass.
		 */
		class IMainRenderPass : public IRenderPass {
		};
	}
}
