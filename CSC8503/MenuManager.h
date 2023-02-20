/**
 * @file   MenuManager.h
 * @brief  A class for managing different menus.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "menu.h"
#include "Vector2.h"
#include <map>
#include <string>

namespace NCL {
	namespace CSC8503 {
		using namespace Maths;
		using namespace std;
		class MenuManager {
		private:
			MenuManager();
			~MenuManager();
		public:
			static MenuManager& instance() {
				static MenuManager INSTANCE;
				return INSTANCE;
			}

			void Draw();

			void Update(float dt);

		protected:
			map<string, Menu> menus;
		};
	}
}
