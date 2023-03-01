#pragma once
#ifdef _ORBIS
#include "pad.h"
#include "InputBase.h"


namespace NCL {
	namespace PS4 {
		class PS4Input : public InputBase
		{
		public:
			PS4Input();
			PS4Input(SceUserServiceUserId &s);
			~PS4Input() {};

			void Poll();
			bool IsInitialized() { return initialized; }

			void DestroyController();

		protected:
			void InitController();

			int32_t padHandle;
			ScePadControllerInformation padInfo;
			uint8_t deadzoneLeft;
			uint8_t deadzoneRight;
			bool initialized=false;
		};
	}
}
#endif
