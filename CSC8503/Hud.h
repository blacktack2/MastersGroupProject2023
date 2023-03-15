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
#include "HudBase.h"

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
					delete hud.second;
				}
				huds.clear();
			}
			void loadHuds(int BossHP, int PlayerHP);

			void Draw();
			void AddHealthBar(int playerID, Health* hp, Vector2 anchor, Vector2 dimension);
			const std::map<int, HealthBar*>& getHuds() {
				return huds;
			};
			void EnableHealthBar(int playerID, bool isEnable) {
				//DisableAllHealthBar();
				if(huds.count(playerID * 2) > 0)
					huds[playerID * 2]->IsEnabled = isEnable;
				if (huds.count(playerID * 2 + 1) > 0)
					huds[playerID * 2 + 1]->IsEnabled = isEnable;
			}
			void DisableAllHealthBar() {
				for (auto i : huds) {
					i.second->IsEnabled = false;
				}
			}

		protected:
			std::map<int, HealthBar*> huds;
		};
	}
}
