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

        Button(float PosX, float PosY, float Width, float Height);

        void Render();

        void Draw(Vector4 colour);

    };
}
