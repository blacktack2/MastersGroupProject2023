/**
 * @file   XboxController.cpp
 * @brief  source file for Xbox controller
 *
 * @author Shantao Liu
 * @author Xiaoyang Liu
 * @date   February 2023
 */
#ifdef x64
#include "XboxController.h"
#include <cmath>

using namespace NCL;

XboxController::XboxController()
{

}

XboxController::~XboxController()
{

}

void XboxController::CheckPorts()
{
    connectedControllers.clear();
    for (int i = 0; i < 4; i++)
    {
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
        dwResult = XInputGetState(i, &state);
        if (dwResult == ERROR_SUCCESS)
        {
            connectedControllers.push_back(i+1);
        }
    }
}

int XboxController::GetActiveControllerNumber()
{
    return connectedControllers.size();
}

void XboxController::UpdateALastState(unsigned int controllerNum) {
    UpdateConnection(controllerNum);
    buttonALastState[controllerNum] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
}
bool XboxController::UpdateConnection(unsigned int controllerNum)   // controllerNum == 1,2,3,4
{
    controllerNum--;

    if (controllerNum >= XUSER_MAX_COUNT)
    {
        return false;
    }
    
    ZeroMemory(&state, sizeof(XINPUT_STATE));
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

    dwResult = XInputGetState(controllerNum, &state);

    if (dwResult != ERROR_SUCCESS)      // Controller is not connected
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool XboxController::GetThumbLeft(unsigned int controllerNum, Maths::Vector2& v)    // v normalized to -1 ~ 1
{
    if (!UpdateConnection(controllerNum))   // Controller is not connected OR controllerNum is out of range
    {
        return false;   // v is unchanged if connection fails
    }
    SHORT deadZone = 10000;
    SHORT x = state.Gamepad.sThumbLX;
    SHORT y = state.Gamepad.sThumbLY;
    float displacement = std::sqrt((x * x) + (y * y));
    if (displacement < deadZone)
    {
        v = Maths::Vector2{ 0,0 };
    }
    else
    {
        float xNorm = (1.0f / 32767.0f) * std::abs(x);
        if (x < 0)
        {
            xNorm *= -1.0f;
        }
        float yNorm = (1.0f / 32767.0f) * std::abs(y);
        if (y < 0)
        {
            yNorm *= -1.0f;
        }
        v = Maths::Vector2{ xNorm,yNorm };
    }
    return true;
}

bool XboxController::GetThumbRight(unsigned int controllerNum, Maths::Vector2& v)    // v normalized to -1 ~ 1
{
    if (!UpdateConnection(controllerNum))   // Controller is not connected OR controllerNum is out of range
    {
        return false;   // v is unchanged if connection fails
    }
    SHORT deadZone = 10000;
    SHORT x = state.Gamepad.sThumbRX;
    SHORT y = state.Gamepad.sThumbRY;
    float displacement = std::sqrt((x * x) + (y * y));
    if (displacement < deadZone)
    {
        v = Maths::Vector2{ 0,0 };
    }
    else
    {
        float xNorm = (1.0f / 32767.0f) * std::abs(x);
        if (x < 0)
        {
            xNorm *= -1.0f;
        }
        float yNorm = (1.0f / 32767.0f) * std::abs(y);
        if (y < 0)
        {
            yNorm *= -1.0f;
        }
        v = Maths::Vector2{ xNorm,yNorm };
    }
    return true;
}

bool XboxController::GetRightTrigger(unsigned int controllerNum, float& n)      // n normalized to 0 ~ 1
{
    if (!UpdateConnection(controllerNum))   // Controller is not connected OR controllerNum is out of range
    {
        return false;   // n is unchanged if connection fails
    }
    BYTE deadZone = 10;
    BYTE keyDown = state.Gamepad.bRightTrigger;
    if (keyDown < deadZone)
    {
        n = 0;
    }
    else
    {
        n = (1.0f / 255) * keyDown;
    }
    return true;
}

bool XboxController::GetLeftTrigger(unsigned int controllerNum, float& n)      // n normalized to 0 ~ 1
{
    if (!UpdateConnection(controllerNum))   // Controller is not connected OR controllerNum is out of range
    {
        return false;   // n is unchanged if connection fails
    }
    BYTE deadZone = 10;
    BYTE keyDown = state.Gamepad.bLeftTrigger;
    if (keyDown < deadZone)
    {
        n = 0;
    }
    else
    {
        n = (1.0f / 255) * keyDown;
    }
    return true;
}



bool XboxController::GetButton(unsigned int controllerNum, short int button)      // n normalized to 0 ~ 1
{
    if (!UpdateConnection(controllerNum))   // Controller is not connected OR controllerNum is out of range
    {
        return false;   // n is unchanged if connection fails
    }

    WORD   keyDown = state.Gamepad.wButtons;
    
    return keyDown & button;
}

bool XboxController::GetAButtonClick(unsigned int controllerNum)      // n normalized to 0 ~ 1
{
    if (!UpdateConnection(controllerNum))   // Controller is not connected OR controllerNum is out of range
    {
        return false;   // n is unchanged if connection fails
    }

    WORD   keyDown = state.Gamepad.wButtons;

    return (keyDown & XINPUT_GAMEPAD_A) && buttonALastState[controllerNum] == false;
}
#endif //x64