/**
 * @file   Window.h
 * @brief  Creates and handles the application Window, as well as handling
 * user input.
 * 
 * @author Rich Davison
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include "Keyboard.h"
#include "Mouse.h"

#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include <string>

namespace Graphics {
	class RendererBase;

	namespace Application {
		class Window {
		public:
			bool update(float dt);

			void setMouseLock(bool lockMouse);
			void showOSPointer(bool show);

			inline const std::string& getTitle() const {
				return mTitle;
			}
			void setTitle(const std::string& title);

			inline HWND getHandle() const {
				return mWindowHandle;
			}
			inline HINSTANCE getInstance() const {
				return mWindowInstance;
			}

			void setRenderer(RendererBase* renderer);
			RendererBase* getRenderer() const;

			void resizeRenderer();

			inline bool initSuccess() const {
				return mInitSuccess;
			}

			static void generateWindow(bool fullScreen = false) {
				new Window(fullScreen);
			}
			static void destroyWindow() {
				delete sWindow;
			}
			static Window& getWindow() {
				return *sWindow;
			}
			static Keyboard& getKeyboard();
			static Mouse& getMouse();
		private:
			Window(bool fullScreen);
			~Window();

			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

			void handleInput(MSG& msg);

			std::string mTitle = "Default Title";

			LONG mWindowX, mWindowY, mWindowWidth = 500, mWindowHeight = 500;

			HWND mWindowHandle;
			HINSTANCE mWindowInstance;

			RendererBase* mRenderer = nullptr;

			bool mIsFullScreen;
			bool mIsMinimized = false;
			bool mIsMaximized = false;
			bool mLockMouse = false;
			bool mShowMouse = true;
			bool mMouseLeftWindow = false;

			bool mIsActive = true;
			bool mForceQuit = false;
			bool mInitSuccess = false;

			static Window* sWindow;
		};
	}
}
