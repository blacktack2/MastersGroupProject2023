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

#include "GameTechRenderer.h"

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

			void Draw(OGLShader* menuShader, OGLTexture* menuTexture, OGLMesh* quad);

			void Update(float dt);

			Vector4 GetMenuDimension();

			void AddMenu(string name, Menu* menu);

			Menu* GetCurrentMenu() {
				return menus[currentMenu];
			}
			void SetCurrentMenu(string name) {
				if (menus[name] != nullptr) {
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

			map<string, Menu*> menus;
			string currentMenu = "main";
			GameTechRenderer* renderer;
			MeshGeometry* quad;
		};
	}
}
