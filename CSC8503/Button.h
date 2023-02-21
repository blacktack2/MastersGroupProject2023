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
#include <RenderObject.h>

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

    public:

        Button(float PosX, float PosY, float Width, float Height);

        void Update(float dt);

        RenderObject* GetRenderObject() const {
            return renderObject;
        }

        void SetRenderObject(RenderObject* newObject) {
            renderObject = newObject;
        }

        void Render();

        void Draw(Vector4 colour);

        Button* CheckMousePosition(Vector2 mousePos);

        Vector4 GetDimension();

        float m_fPosX;
        float m_fPosY;
        float m_fWidth;
        float m_fHeight;
        float g_fWidth;
        float g_fHeight;

        bool m_bPressed;

        bool isMouseHover;
    private:
        UCHAR r, g, b;

        RenderObject* renderObject = nullptr;

    };
}
