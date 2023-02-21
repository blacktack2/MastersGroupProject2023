/**
 * @file   Menu.h
 * @brief  A class for defining a menu.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "Vector2.h"
#include "Button.h"
#include <vector>
#include <RenderObject.h>

namespace NCL {
	namespace CSC8503 {
		using namespace Maths;
		using namespace std;
		class Menu{
		public:
			Menu(){}
			Menu(Vector2 screenPos, Vector2 dimension);
			~Menu();

			void Draw(OGLShader* menuShader, OGLTexture* menuTexture, OGLMesh* quad);

			void Update(float dt);

			void AddButton(Button* btn) {
				buttons.push_back(btn);
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}
			vector<Button*>* GetButtons() {
				return &buttons;
			}

			Vector4 GetDimension();

		protected:
			Vector2 screenPos;
			Vector2 dimension;

			RenderObject* renderObject = nullptr;

			vector<Button*> buttons;

			int buttonSelect = 0;
		};
	}
}
