/**
 * @file   Win32Keyboard.h
 * @brief  Win32 implementation of the keyboard handler.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#ifdef _WIN32
#include "Keyboard.h"

#include "Win32Window.h"

#include <Windows.h>

namespace NCL{
	namespace Win32Code {
		/**
		 * @brief Win32 implementation of the keyboard handler.
		 */
		class Win32Keyboard : public Keyboard {
		public:
			friend class Win32Window;

		protected:
			Win32Keyboard(HWND& hwnd);
			virtual ~Win32Keyboard(void) {
			}

			virtual void UpdateRAW(RAWINPUT* raw);
			RAWINPUTDEVICE rid;
		};
	}
}
#endif //_WIN32
