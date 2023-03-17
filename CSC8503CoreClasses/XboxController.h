/**
 * @file   XboxController.h
 * @brief  header file for Xbox controller
 *
 * @author Shantao Liu
 * @author Xiaoyang Liu
 * @date   February 2023
 */

#pragma once
#ifdef x64
#include<Windows.h>
#include<Xinput.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

namespace NCL
{
    class XboxController
    {
    public:
        XboxController();

        ~XboxController();

        void CheckPorts();
        int GetActiveControllerNumber();

        bool GetThumbLeft(unsigned int controllerNum, Maths::Vector2& v);
        bool GetThumbRight(unsigned int controllerNum, Maths::Vector2& v);
        bool GetRightTrigger(unsigned int controllerNum, float& n);
        bool GetLeftTrigger(unsigned int controllerNum, float& n);

    protected:

        bool UpdateConnection(unsigned int controllerNum);

        XINPUT_STATE state;
        DWORD dwResult;
        XINPUT_VIBRATION vibration;
        std::vector<int> connectedControllers;
    };
}
#endif //x64