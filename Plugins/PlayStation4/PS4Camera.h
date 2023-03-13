#pragma once
#ifdef _ORBIS
#include "PS4MemoryAware.h"
#include "..\..\Common\Camera.h"
#include "PS4Input.h"

namespace NCL {
	class Camera;
	namespace PS4 {
		
		class PS4Camera :
			public Camera, public PS4MemoryAware
		{
		public :
			PS4Camera();
			PS4Camera(PS4Input* input);
			void SetInput(PS4Input* input) { mInput = input; }
			void UpdateCamera(float dt) override;
		protected:
			PS4Input* mInput;
			
		};

	}
}
#endif
