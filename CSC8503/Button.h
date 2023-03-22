/**
 * @file   Button.h
 * @brief  
 * 
 * @author Yifei Hu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "Debug.h"
#include "InputKeyMap.h"
#include "UIObject.h"
#include "OptionManager.h"

#include "Vector4.h"

#include <functional>
#include <memory>

namespace NCL {
    namespace Rendering {
        class TextureBase;
    }
}

using namespace NCL::Maths;
using namespace NCL::Rendering;

namespace NCL {
    namespace CSC8503 {
        class GameTechRenderer;

        class Button : public UIObject {
        public:
            typedef std::function<void(Button& button)> overlap_func;

            Button(float PosX, float PosY, float Width, float Height, std::shared_ptr<TextureBase> texture, overlap_func onclick, overlap_func onselect, overlap_func onpress);
            ~Button();

            void Update(float dt) override;
            Vector4 GetDimension() const override;

            void SetColour(Vector4 colour) {
                btncolour = colour;
            };

            void OnClick();
            void OnSelect();
            void OnPress();
        protected:
            void DrawExtras() override;
        private:
            void CheckMousePosition(Vector2 mousePos);

            NCL::InputKeyMap& keyMap;
            GameTechRenderer& renderer;

            float posX;
            float posY;
            float width;
            float height;

            Vector4 btncolour = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

            bool isMouseHover;

            overlap_func OnClickCallback = nullptr;
            overlap_func OnSelectCallback = nullptr;
            overlap_func OnPressCallback = nullptr;

            OptionManager& optionManager = OptionManager::instance();

            int counter = 12000;
            int t = 0;
        };
    }
}
