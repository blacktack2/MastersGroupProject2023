#ifdef _ORBIS
#include "PS4InputManager.h"
#include "../../Common/Vector2.h"
#include "pad.h"
#include <sys\_defines\_sce_ok.h>
#include <iostream>
#include <user_service\user_service_api.h>

using namespace NCL;
using namespace PS4;


PS4Input PS4InputManager::devices[SCE_USER_SERVICE_MAX_LOGIN_USERS];
SceUserServiceUserId PS4InputManager::mPlayer1Id =SCE_DEVICE_SERVICE_ERROR_INVALID_USER;

void PS4InputManager::Initialize() {
	SceUserServiceInitializeParams params;
	memset((void*)&params, 0, sizeof(params));
	params.priority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
	if (sceUserServiceInitialize(&params) != SCE_OK) {
		std::cout << "PS4Input: Failed to initialise User Service!" << std::endl;
	};

	SceUserServiceLoginUserIdList usersList;
	sceUserServiceGetLoginUserIdList(&usersList);

	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);
	int index = 0;
	scePadInit();
	if (ret == SCE_OK) {
		mPlayer1Id = userId;
		devices[index++] = PS4Input(userId);
		
	}

	for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
		if (usersList.userId[i] != SCE_USER_SERVICE_USER_ID_INVALID && usersList.userId[i] != mPlayer1Id) {
			devices[index++] = PS4Input(usersList.userId[i]);
		}
	}
	/*while (index < SCE_USER_SERVICE_MAX_LOGIN_USERS) {
		devices[index++] = nullptr;
	}*/
}

void NCL::PS4::PS4InputManager::Destroy()
{
	for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
		if (devices[i].IsInitialized())
			devices[i].DestroyController();
	}

	sceUserServiceTerminate();
}

void NCL::PS4::PS4InputManager::Update()
{
	for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
		if(devices[i].IsInitialized())
			devices[i].Poll();
	}
}

Maths::Vector2 NCL::PS4::PS4InputManager::GetAxis(unsigned int playerId, unsigned int reqAxis)
{
	if (playerId < SCE_USER_SERVICE_MAX_LOGIN_USERS && reqAxis < 3 && devices[playerId].IsInitialized())
		return Maths::Vector2(devices[playerId].GetAxis(reqAxis).x, devices[playerId].GetAxis(reqAxis).y);
	else return Maths::Vector2(-2, -2);
}

bool NCL::PS4::PS4InputManager::GetButtons(unsigned int playerId, unsigned int button)
{
	return (playerId < SCE_USER_SERVICE_MAX_LOGIN_USERS && devices[playerId].IsInitialized() && devices[playerId].GetButton(button) != 0.0f);
	
	//return false;
}

#endif //_ORBIS