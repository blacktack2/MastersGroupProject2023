/**
  * @author Yifei Hu
  * @date   February 2023
  */
#pragma once
#include "OGLOverlayRenderPass.h"

#include "GameWorld.h"

#include "Debug.h"
#include "UIObject.h"
#include "InputKeyMap.h"
#include <functional>

namespace NCL::Rendering {
	class OGLShader;
	class OGLTexture;
	class OGLFrameBuffer;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
#pragma once
    class Button : public UIObject
    {

    public:
        typedef std::function<void()> overlap_func;

        Button(float PosX, float PosY, float Width, float Height);
        ~Button();

        void Update(float dt) override;

        void Render();

        void Draw(Vector4 colour);

        Button* CheckMousePosition(Vector2 mousePos);

        Vector4 GetDimension();

        virtual void OnClick() {
            if (OnClickCallback) OnClickCallback();
        }

        float m_fPosX;
        float m_fPosY;
        float m_fWidth;
        float m_fHeight;
        float g_fWidth;
        float g_fHeight;

        bool isMouseHover;

        overlap_func OnClickCallback = nullptr;
    private:

        paintHell::InputKeyMap& keyMap;

    };
}
