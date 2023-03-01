#ifdef _ORBIS
#include "PS4Input.h"
#include "pad.h"
#include <sys\_defines\_sce_ok.h>
#include <iostream>
#include <user_service\user_service_api.h>

using namespace NCL::PS4;

PS4Input::PS4Input()	{
	/*SceUserServiceInitializeParams params;
	memset((void*)&params, 0, sizeof(params));
	params.priority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
	if (sceUserServiceInitialize(&params) != SCE_OK) {
		std::cout << "PS4Input: Failed to initialise User Service!" << std::endl;
	};

	scePadInit();
	InitController();*/
}

NCL::PS4::PS4Input::PS4Input(SceUserServiceUserId& s)
{
	padHandle = scePadOpen(s, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);
	int ret = scePadGetControllerInformation(padHandle, &padInfo);
	if (ret == SCE_OK) {
		//can get deadzone information etc through here
		initialized = true;

		deadzoneLeft = padInfo.stickInfo.deadZoneLeft;
		deadzoneRight = padInfo.stickInfo.deadZoneRight;
		std::cout << "Dead Zone Left:" << (int)padInfo.stickInfo.deadZoneLeft << std::endl;
		std::cout << "Dead Zone Right:" << (int)padInfo.stickInfo.deadZoneRight << std::endl;
	}
}

void PS4Input::DestroyController()	{
	scePadClose(padHandle);

	/*sceUserServiceTerminate();*/
}
//PS4Input::~PS4Input()	{
//	scePadClose(padHandle);
//
//	/*sceUserServiceTerminate();*/
//}

void PS4Input::InitController() {
	//Get the currently logged in player
	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);

	if (ret == SCE_OK) {
		padHandle = scePadOpen(userId, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);

		//Can get connectivity / deadzone info from this structure.
		int ret = scePadGetControllerInformation(padHandle, &padInfo);
		if (ret == SCE_OK) {
			//can get deadzone information etc through here

			
			deadzoneLeft= padInfo.stickInfo.deadZoneLeft;
			deadzoneRight=padInfo.stickInfo.deadZoneRight;
			std::cout << "Dead Zone Left:" << (int)padInfo.stickInfo.deadZoneLeft <<std::endl;
			std::cout << "Dead Zone Right:" << (int)padInfo.stickInfo.deadZoneRight <<std::endl;
		}
	}
}

void PS4Input::Poll() {
	ScePadData data;

	int ret = scePadReadState(padHandle, &data);
	
	if (ret == SCE_OK) {
		if (data.connected) {
			float lowerDeadZoneArea = 128.0f - (int)deadzoneLeft;
			float higherDeadZoneArea = 128.0f + (int)deadzoneLeft;
			axis[0].x = ((((floor(((data.leftStick.x <= lowerDeadZoneArea) || (data.leftStick.x >= higherDeadZoneArea) ?data.leftStick.x :128.0f)  / 255.0f * 100.0f) / 100.0f)) * 2 ) - 1.0f);
			axis[0].y = ((((floor(((data.leftStick.y <= lowerDeadZoneArea) || (data.leftStick.y >= higherDeadZoneArea) ?data.leftStick.y : 128.0f) / 255.0f * 100.0f) / 100.0f)) * 2 ) - 1.0f);

			lowerDeadZoneArea = 128.0f - (int)deadzoneRight;
			higherDeadZoneArea = 128.0f + (int)deadzoneRight;
			axis[1].x = ((((floor(((data.rightStick.x <= lowerDeadZoneArea) || (data.rightStick.x >= higherDeadZoneArea) ? data.rightStick.x : 128.0f) / 255.0f * 100.0f) / 100.0f)) * 2) - 1.0f);
			axis[1].y = ((((floor(((data.rightStick.y <= lowerDeadZoneArea) || (data.rightStick.y >= higherDeadZoneArea) ? data.rightStick.y : 128.0f) / 255.0f * 100.0f) / 100.0f)) * 2) - 1.0f);

			axis[2].x  = 0.0f;
			axis[2].x += ((data.buttons & SCE_PAD_BUTTON_RIGHT) ? 1.0f : 0.0f);
			axis[2].x -= ((data.buttons & SCE_PAD_BUTTON_LEFT)  ? 1.0f : 0.0f);

			axis[2].y  = 0.0f;
			axis[2].y += ((data.buttons & SCE_PAD_BUTTON_UP)    ? 1.0f : 0.0f);
			axis[2].y -= ((data.buttons & SCE_PAD_BUTTON_DOWN)  ? 1.0f : 0.0f);

			buttons[0] = ((data.buttons & SCE_PAD_BUTTON_TRIANGLE) ? 1.0f : 0.0f);
			buttons[1] = ((data.buttons & SCE_PAD_BUTTON_CIRCLE)   ? 1.0f : 0.0f);
			buttons[2] = ((data.buttons & SCE_PAD_BUTTON_CROSS)    ? 1.0f : 0.0f);
			buttons[3] = ((data.buttons & SCE_PAD_BUTTON_SQUARE)   ? 1.0f : 0.0f);

			buttons[4] = data.analogButtons.l2 / 255.0f;
			buttons[5] = data.analogButtons.r2 / 255.0f;

			buttons[6] = ((data.buttons & SCE_PAD_BUTTON_L1) ? 1.0f : 0.0f);
			buttons[7] = ((data.buttons & SCE_PAD_BUTTON_R1) ? 1.0f : 0.0f);

			buttons[8] = ((data.buttons & SCE_PAD_BUTTON_L3) ? 1.0f : 0.0f);
			buttons[9] = ((data.buttons & SCE_PAD_BUTTON_R3) ? 1.0f : 0.0f);
		}
	}
}
#endif