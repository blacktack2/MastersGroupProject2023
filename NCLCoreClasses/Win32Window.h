/**
 * @file   Win32Window.h
 * @brief  Win32 implementation of the window handler.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#include "Window.h"
#ifdef _WIN32

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT) 0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT) 0x02)
#endif

#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD ((USHORT) 0x06)
#endif

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "Win32Mouse.h"
#include "Win32Keyboard.h"

#include <Windows.h>

namespace NCL{
	namespace Win32Code {
		class Win32Mouse;
		class Win32Keyboard;

		/**
		 * @brief Win32 implementation of the window handler.
		 */
		class Win32Window : public Window {
		public:
			friend class Window;
			void LockMouseToWindow(bool lock) override;
			void ShowOSPointer(bool show) override;
			void SetConsolePosition(int x, int y) override;
			void ShowConsole(bool state) override;
			void SetFullScreen(bool state) override;
			void SetWindowPosition(int x, int y) override;

			HWND GetHandle() const {
				return windowHandle;
			}
			HINSTANCE GetInstance() const {
				return windowInstance;
			}
		protected:
			Win32Window(const std::string& title, int sizeX, int sizeY, bool fullScreen, int offsetX, int offsetY);
			virtual ~Win32Window(void);

			bool InternalUpdate() override;
			void UpdateTitle() override;

			void CheckMessages(MSG& msg);
			static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			HWND windowHandle;
			HINSTANCE windowInstance;

			bool forceQuit = false;

			bool active = true;
			bool fullScreen = false;
			bool lockMouse = false;
			bool showMouse = true;
			bool mouseLeftWindow = false;
			bool maximised = true;

			Win32Mouse* winMouse = nullptr;
			Win32Keyboard* winKeyboard = nullptr;
		};
	}
}
#endif //_WIN32

