/**
 * @file   MenuManager.h
 * @brief  A class for managing different menus.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "GameTechRenderer.h"
#include "menu.h"
#include "Vector2.h"

#include <map>
#include <string>

namespace NCL {
	namespace CSC8503 {
		using namespace Maths;
		class MenuManager {
		private:
			MenuManager();
			~MenuManager();
		public:
			static MenuManager& instance() {
				static MenuManager INSTANCE;
				return INSTANCE;
			}

			void Update(float dt);

			Vector4 GetMenuDimension();

			void AddMenu(std::string name, Menu* menu);

			void RemoveMenu(std::string name);

			Menu* GetCurrentMenu() {
				return menus[currentMenu];
			}

			std::string GetCurrentMenuName() {
				return currentMenu;
			}

			void SetCurrentMenu(std::string name) {
				if (menus.contains(name)) {
					currentMenu = name;
				}
			}
			/**
			* @brief function class for Converting pixel coordinates to -1 to 1 range for quad
			* @param screenWidth Current screen Width.
			* @param screenHeight Current screen Height.
			* @param componentDimension The dimension (x, y, width, height) of the quad/component in terms of pixels. (x and y are center of component)
			* @return Vector4 containing (vector2 topLeft, vector2 bottomRight) in screen space
			*/
			Vector4 PixelToScreenSpace(float screenWidth, float screenHeight, Vector4 componentDimension);

		protected:

			std::map<std::string, Menu*> menus;
			std::string currentMenu = "main";
			GameTechRenderer* renderer;
		};
	}
}
