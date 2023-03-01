#pragma once
#ifdef _ORBIS
#include "PS4Input.h"

namespace NCL{
	namespace Maths { class Vector2; }
	namespace PS4 {
		enum Players {
			Player1=0,
			Player2,
			Player3,
			Player4
		};

		class PS4InputManager {
		private:
			PS4InputManager() {};
			~PS4InputManager();

		public:
			//create the input manager and collect controller data
			static void Initialize();
			
			static void Destroy() ;
			/*static PS4InputManager* GetInstance() { return instance; }*/
			
			//Update the controller states. Needs to be called every frame
			static void Update();

			//Provides axis data for Left stick, right stick and direction keys
			static Maths::Vector2 GetAxis(unsigned int playerId, unsigned int reqAxis);

			//Provides button pressed information
			static bool GetButtons(unsigned int playerId, unsigned int button);
			
		private:
			/*static PS4InputManager* instance;*/

			static PS4Input devices[SCE_USER_SERVICE_MAX_LOGIN_USERS];
			static SceUserServiceUserId mPlayer1Id;
		};
	}
}
#endif // _ORBIS
