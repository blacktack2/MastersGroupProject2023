/**
 * @file   HudRPass.h
 * @brief
 *
 * @author Shantao Liu
 * @author Xiaoyang Liu
 * @date   March 2023
 */

#pragma once
#include "OGLOverlayRenderPass.h"

#include "Hud.h"

#include <memory>

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class GameTechRenderer;

		class HudRPass : public OGLOverlayRenderPass {
		public:
			HudRPass();
			~HudRPass() = default;

			void Render() override;
		private:
			GameTechRenderer& renderer;
			GameWorld& gameWorld;

			Hud hud;
		};
	}
}
