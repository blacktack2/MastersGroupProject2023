/**
 * @file   Window.h
 * @brief  Handler class for base window functionality.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"

#include "Vector2.h"

#include <string>

namespace NCL {
	namespace Rendering {
		class RendererBase;
	};
	using namespace NCL::Rendering;
	
	/**
	 * @brief Handler class for base window functionality.
	 */
	class Window {
	public:
		static Window* CreateGameWindow(std::string title = "NCLGL!", int sizeX = 800, int sizeY = 600, bool fullScreen = false, int offsetX = 100, int offsetY = 100);

		static inline void DestroyGameWindow() {
			delete window;
			window = nullptr;
		}

		inline bool IsMinimised() const {
			return minimised;
		}

		bool UpdateWindow();
		void SetRenderer(RendererBase* r);

		inline bool HasInitialised() const {
			return init;
		}

		inline float GetScreenAspect() const {
			return size.x / size.y;
		}

		inline Vector2 GetScreenSize() const {
			return size;
		}
		inline Vector2 GetScreenPosition() const {
			return position;
		}

		inline const std::string& GetTitle() const {
			return windowTitle;
		}
		inline void SetTitle(const std::string& title) {
			windowTitle = title;
			UpdateTitle();
		};

		virtual void LockMouseToWindow(bool lock) = 0;
		virtual void ShowOSPointer(bool show) = 0;

		virtual void SetWindowPosition(int x, int y) {};
		virtual void SetFullScreen(bool state) {};
		virtual void SetConsolePosition(int x, int y) {};
		virtual void ShowConsole(bool state) {};

		static inline const Keyboard* GetKeyboard() {
			return keyboard;
		}
		static inline const Mouse* GetMouse() {
			return mouse;
		}
		static inline const GameTimer* GetTimer() {
			return timer;
		}

		static inline Window* const GetWindow() {
			return window;
		}
	protected:
		Window();
		virtual ~Window();

		virtual void UpdateTitle() {}

		virtual bool InternalUpdate() = 0;

		void ResizeRenderer();

		RendererBase* renderer;

		bool minimised = false;
		bool init = false;
		Vector2 position;
		Vector2 size;
		Vector2 defaultSize;

		std::string windowTitle;

		static Window*    window;
		static Keyboard*  keyboard;
		static Mouse*     mouse;
		static GameTimer* timer;
	};
}
