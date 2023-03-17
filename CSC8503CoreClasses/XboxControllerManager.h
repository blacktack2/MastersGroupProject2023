/**
 * @file   XboxControllerManager.h
 * @brief  header file for Xbox controller Manager
 *
 * @author Xiaoyang Liu
 * @date   March 2023
 */

#pragma once
#ifdef x64
#include "XboxController.h"

namespace NCL
{
    class XboxControllerManager
    {
    public:
        XboxControllerManager()
        {

        }

        ~XboxControllerManager()
        {

        }

    public:
        static XboxController& GetXboxController()
        {
            static XboxController controller;
            return controller;
        }


    };
}
#endif //x64