/**
 * @author Yifei Hu
 * @date   February 2023
 */
#include "Button.h"
#include "Debug.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;


Button::Button(float PosX, float PosY, float Width, float Height): keyMap(paintHell::InputKeyMap::instance()) {
    m_fPosX = PosX;
    m_fPosY = PosY;
    m_fWidth = Width;
    m_fHeight = Height;
}
Button::~Button() {
}

void Button::Update(float dt){
    UIObject::Update(dt);
    isMouseHover = false;
    if (keyMap.HasMouse()) {
        Vector2 mousePos = keyMap.GetMousePosition();
        CheckMousePosition(mousePos);

        if (isMouseHover && keyMap.GetButton(InputType::MouseLeftClick)) {
            this->OnClickCallback();
        }
    }
    
} 

void Button::Render(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 20, 0, 20, 0, 100);
    glColor3f(0, 0, 0);
}

void Button::Draw(Vector4 colour) {
    Debug::Print("[]", Vector2(m_fPosX, m_fPosY), colour, 1.0f);
    Debug::Print("[]", Vector2(m_fPosX + m_fWidth, m_fPosY + m_fHeight), colour, 1.0f);
}

Button* Button::CheckMousePosition(Vector2 mousePos)
{
    mousePos.x = mousePos.x * 2 / (float)renderer.GetWidth() - 1;
    mousePos.y = -(mousePos.y * 2 / (float)renderer.GetHeight() - 1);
    if (mousePos.x < (m_fPosX - m_fWidth)) {
        return nullptr;
    }
    if (mousePos.x > (m_fPosX + m_fWidth)) {
        return nullptr;
    }
    if (mousePos.y < (m_fPosY - m_fHeight)) {
        return nullptr;
    }
    if (mousePos.y > (m_fPosY + m_fHeight)) {
        return nullptr;
    }
    isMouseHover = true;
    return this;
}

Vector4 Button::GetDimension(){
    if (isMouseHover) {
        float movement = 1.01f;

        return Vector4(
            m_fPosX * movement,
            m_fPosY * movement,
            m_fWidth,
            m_fHeight
        );
    }
    return Vector4(
        m_fPosX,
        m_fPosY,
        m_fWidth,
        m_fHeight
    );
}
