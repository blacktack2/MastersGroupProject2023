/**
 * @author Yifei Hu
 * @date   February 2023
 */
#include "Button.h"

#include "Debug.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;


Button::Button(float PosX, float PosY, float Width, float Height) {
    m_bPressed = false;
    m_fPosX = PosX;
    m_fPosY = PosY;
    m_fWidth = Width;
    m_fHeight = Height;
    r = 0xff;
    g = 0x00;
    b = 0x00;
}

void Button::Render()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 20, 0, 20, 0, 100);
    glColor3f(0, 0, 0);
}

void Button::Draw(Vector4 colour) {
    Debug::Print("[]", Vector2(m_fPosX, m_fPosY), colour, 1.0f);
    Debug::Print("[]", Vector2(m_fPosX + m_fWidth, m_fPosY + m_fHeight), colour, 1.0f);
}