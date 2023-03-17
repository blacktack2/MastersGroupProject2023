#pragma once
#ifdef _ORBIS
#include "PS4MemoryAware.h"
#include "Camera.h"
#include "PS4Input.h"

namespace NCL {
	namespace PS4 {
		
		class PS4Camera :
			public Camera, public PS4MemoryAware
		{
		public :
			PS4Camera();
			PS4Camera(PS4Input* input);
			void SetInput(PS4Input* input) { mInput = input; }
			void UpdateCamera(float dt);
		protected:
			PS4Input* mInput;
			
		};

	}
}
#endif
