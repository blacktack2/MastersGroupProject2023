/**
 * @file   Button.cpp
 * @brief  See Button.h.
 * 
 * @author Yifei Hu
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "Button.h"

#include "GameTechRenderer.h"

#include "Debug.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;


Button::Button(float PosX, float PosY, float Width, float Height, std::shared_ptr<TextureBase> texture, overlap_func onclick, overlap_func onselect, overlap_func onpress):
    UIObject(texture), keyMap(NCL::InputKeyMap::instance()), renderer(GameTechRenderer::instance()), OnClickCallback(onclick), OnSelectCallback(onselect), OnPressCallback(onpress) {
    posX = PosX;
    posY = PosY;
    width = Width;
    height = Height;
    isMouseHover = false;
}

Button::~Button() {
}

void Button::Update(float dt){
    isMouseHover = false;
    if (!keyMap.HasMouse()) {
        return;
    }
    Vector2 mousePos = keyMap.GetMousePosition();
    CheckMousePosition(mousePos);

    if (isMouseHover && keyMap.GetButton(InputType::Confirm)) {
        OnClick();
    }

    if (keyMap.GetButtonDown(InputType::DOWN)) {
        if (counter == 0) {
            counter++;
        }
        counter++;
        t++;
        optionManager.SetCounter(counter);
        OnSelect();
    }
    if (keyMap.GetButtonUp(InputType::UP)) {
        if (t == 0) {
            counter++;
        }
        t++;
        counter--;
        optionManager.SetCounter(counter);
        OnSelect();
    }

    if (keyMap.GetButtonDown(InputType::Confirm_K)) {
        OnPress();
    }
}

Vector4 Button::GetDimension() const {
    constexpr float movement = 0.003f;
    return isMouseHover ? Vector4(posX + movement, posY - movement, width, height) : Vector4(posX, posY, width, height);
}

void Button::OnClick() {
    if (!OnClickCallback) {
        //std::cout << "Button has no OnClick callback!\n";
        return;
    }
    OnClickCallback(*this);
}

void Button::OnSelect() {
    if (!OnSelectCallback) {
        //std::cout << "Button has no OnSelect callback!\n";
        return;
    }
    OnSelectCallback(*this);
}

void Button::OnPress() {
    if (!OnPressCallback) {
        //std::cout << "Button has no OnPress callback!\n";
        return;
    }
    OnPressCallback(*this);
}

void Button::DrawExtras() {
    
}

void Button::CheckMousePosition(Vector2 mousePos) {
    mousePos.x = (mousePos.x * 2 / (float)renderer.GetWidth() - 1);
    mousePos.y = -(mousePos.y * 2 / (float)renderer.GetHeight() - 1);
    isMouseHover = !(mousePos.x < (posX - width ) || mousePos.x > (posX + width) ||
                     mousePos.y < (posY - height) || mousePos.y > (posY + height));
}