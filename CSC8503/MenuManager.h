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
#include <memory>
#include <string>

namespace NCL {
	namespace CSC8503 {
		using namespace Maths;
		class MenuManager {
		public:
			static MenuManager& instance() {
				static MenuManager INSTANCE;
				return INSTANCE;
			}

			void Update(float dt);

			Vector4 GetMenuDimension();

			Menu& AddMenu(const std::string& name, const Vector2& position, const Vector2& size, std::shared_ptr<TextureBase> texture);

			Menu& GetCurrentMenu() {
				return *menus.find(currentMenu)->second;
			}

			const std::string& GetCurrentMenuName() {
				return currentMenu;
			}

			void RemoveAndEraseMenu(const std::string& name);


			void SetCurrentMenu(std::string name) {
				if (menus.contains(name)) {
					currentMenu = name;
				}
			}
			/**
			* @brief function class for Converting pixel coordinates to -1 to 1 range for quad
			*
			* @param screenWidth Current screen Width.
			* @param screenHeight Current screen Height.
			* @param componentDimension The dimension (x, y, width, height) of the quad/component in terms of pixels. (x and y are center of component)
			* @return Vector4 containing (vector2 topLeft, vector2 bottomRight) in screen space
			*/
			Vector4 PixelToScreenSpace(float screenWidth, float screenHeight, Vector4 componentDimension);
		protected:
			std::map<std::string, std::unique_ptr<Menu>> menus;
			std::string currentMenu = "main";
			GameTechRenderer& renderer;
		private:
			MenuManager();
			~MenuManager();
		};
	}
}
