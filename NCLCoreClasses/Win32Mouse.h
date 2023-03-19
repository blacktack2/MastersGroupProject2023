/**
 * @file   Win32Mouse.h
 * @brief  Win32 implementation of the mouse handler.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#ifdef _WIN32
#include "Mouse.h"

#include "Win32Window.h"

#include <Windows.h>

namespace NCL{
	namespace Win32Code {
		/**
		 * @brief Win32 implementation of the mouse handler.
		 */
		class Win32Mouse : public NCL::Mouse {
		public:
			friend class Win32Window;
		protected:
			Win32Mouse(HWND& hwnd);
			virtual ~Win32Mouse(void) {}

			void UpdateWindowPosition(const Vector2& newPos) {
				windowPosition = newPos;
			}

			virtual void UpdateRAW(RAWINPUT* raw);
			RAWINPUTDEVICE rid;

			bool setAbsolute;
		};
	}
}
#endif //_WIN32
