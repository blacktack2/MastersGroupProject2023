/**
 * @author Yifei Hu
 * @date   February 2023
 */
#pragma once
#include "OGLOverlayRenderPass.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "Debug.h"

namespace NCL::Rendering {
	class OGLShader;
	class OGLTexture;
	class OGLFrameBuffer;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
#pragma once
    class Button
    {
    private:
        UCHAR r, g, b;
    public:
        float m_fPosX;
        float m_fPosY;
        float m_fWidth;
        float m_fHeight;
        float g_fWidth;
        float g_fHeight;

        bool m_bPressed;

        Button(float PosX, float PosY, float Width, float Height)
        {
            m_bPressed = false;
            m_fPosX = PosX;
            m_fPosY = PosY;
            m_fWidth = Width;
            m_fHeight = Height;
            r = 0xff;
            g = 0x00;
            b = 0x00;
        }
        void Render()
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glOrtho(0, 20, 0, 20, 0, 100);
            glColor3f(0, 0, 0);
        }
        void Draw() {
            Debug::Print("[]", Vector2(m_fPosX, m_fPosY), Vector4(0, 1, 0, 1), 30.0f);
        }

    };
}
