/**
 * @file   Window.h
 * @brief  Creates and handles the application window, as well as handling
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
		/**
		 * @brief Creates and handles the application window, as well as user input.
		 */
		class Window {
		public:
			/**
			 * @brief Update user input. Should be called once per frame.
			 * @param dt Time since last frame (milliseconds).
			 */
			bool update(float dt);

			/**
			 * @brief Set whether the mouse cursor should be locked to the window.
			 * @param lockMouse If true, prevent the mouse cursor from leaving the
			 * window. If false, return the mouse cursor to normal behaviour.
			 */
			void setMouseLock(bool lockMouse);
			/**
			 * @brief Set whether the mouse cursor should be visible.
			 * @param show If true, set the mouse cursor to be visible as normal. If
			 * false, set the mouse cursor to be invisible.
			 */
			void showOSPointer(bool show);

			/**
			 * @returns The text currently being displayed on the window header.
			 */
			inline const std::string& getTitle() const {
				return mTitle;
			}
			/**
			 * @param title The text to be displayed on the window header.
			 */
			void setTitle(const std::string& title);

			/**
			 * @returns The handle to the application window. See
			 * http://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#HWND
			 */
			inline HWND getHandle() const {
				return mWindowHandle;
			}
			/**
			 * @returns The handle to the application instance. See
			 * http://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#HINSTANCE
			 */
			inline HINSTANCE getInstance() const {
				return mWindowInstance;
			}

			/**
			 * @param Bind a renderer to this Window to allow handling of
			 * resize events. Only one can be set at a time.
			 * @param renderer Renderer to be bound to the Window.
			 */
			void setRenderer(RendererBase* renderer);
			/**
			 * @returns The renderer currently bound to this Window.
			 */
			RendererBase* getRenderer() const;

			/**
			 * @brief Signal an update for the renderer to sync its viewport
			 * with the window.
			 */
			void resizeRenderer();

			/**
			 * @returns Current width of the window.
			 */
			inline LONG getWidth() {
				return mWindowWidth;
			}
			/**
			 * @returns Current height of the window.
			 */
			inline LONG getHeight() {
				return mWindowHeight;
			}

			/**
			 * @returns false if initialization has failed, otherwise true.
			 */
			inline bool initSuccess() const {
				return mInitSuccess;
			}

			/**
			 * @brief Singleton constructor for the Window class. Must be called
			 * before Window::getWindow.
			 * @returns Reference to the newly constructed window.
			 */
			static Window& createWindow(bool fullScreen = false) {
				return *(new Window(fullScreen));
			}
			/**
			 * @brief Singleton getter for the Window class. Must only be called after
			 * Window::createWindow, and not after Window::destroyWindow.
			 */
			static Window& getWindow() {
				return *sWindow;
			}
			/**
			 * @brief Singleton destructor for the Window class.
			 */
			static void destroyWindow() {
				delete sWindow;
				sWindow = nullptr;
			}

			/**
			 * @brief Singleton getter for the Keyboard class. See Keyboard.
			 */
			static Keyboard& getKeyboard();
			/**
			 * @brief Singleton getter for the Mouse class. See Mouse.
			 */
			static Mouse& getMouse();
		private:
			Window(bool fullScreen);
			~Window();

			/**
			 * @brief Callback function for handling messages sent to the window. See
			 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
			 */
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

			/**
			 * @brief Parse user input and update the corresponding handlers.
			 */
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
