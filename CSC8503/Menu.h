/**
 * @file   Menu.h
 * @brief  A class for defining a menu.
 *
 * @author Felix Chiu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "Button.h"
#include "UIObject.h"

#include "Vector2.h"

#include <memory>
#include <vector>

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
}

using namespace NCL::Rendering;
using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class Menu : public UIObject {
		public:
			Menu(Vector2 screenPos, Vector2 dimension, std::shared_ptr<TextureBase> texture);
			~Menu();

			void Update(float dt) override;
			Vector4 GetDimension() const override;

			Button& AddButton(float x, float y, float width, float height, std::shared_ptr<TextureBase> texture, Button::overlap_func onclick = nullptr);

			const std::vector<std::unique_ptr<Button>>& GetButtons() {
				return buttons;
			}
		protected:
			Vector2 screenPos;
			Vector2 dimension;

			std::vector<std::unique_ptr<Button>> buttons;

			int buttonSelect = 0;
		};
	}
}
