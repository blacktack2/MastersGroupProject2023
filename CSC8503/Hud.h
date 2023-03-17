/**
 * @file   Hud.h
 * @brief  A class defining a hud.
 *
 * @author Shantao Liu
 * @author Felix Chiu
 * @author Xiaoyang Liu
 * @date   March 2023
 */
#pragma once

#include "HealthBar.h"
#include "Assetlibrary.h"

namespace NCL {
	namespace CSC8503 {
		using namespace Maths;
		class Hud
		{
		public:
			Hud() {
			}
			~Hud() {
				ClearAndErase();
			}
			void ClearAndErase() {
				for (auto hud : huds) {
					delete hud;
				}
				huds.clear();
			}
			void loadHuds(int BossHP, int PlayerHP);

			void Draw();
			void AddHealthBar(Health* hp, Vector2 anchor, Vector2 dimension);
			const std::vector<HealthBar*>& getHuds() {
				return huds;
			};

		protected:
			std::vector<HealthBar*> huds;
		};
	}
}
