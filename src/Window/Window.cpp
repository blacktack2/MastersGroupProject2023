/**
 * @file   Window.cpp
 * @brief  
 * 
 * @author Rich Davison
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Window.h"

#include "RendererBase.h"

#include <strsafe.h>
#include <windowsx.h>

using namespace Graphics;
using namespace Application;

Window* Window::sWindow = nullptr;

#define WINDOWCLASS "WindowClass"

Window::Window(bool fullScreen) :
mIsFullScreen(fullScreen), mWindowX(mIsFullScreen ? 0 : 100), mWindowY(mIsFullScreen ? 0 : 100) {
	sWindow = this;
	mWindowInstance = GetModuleHandle(nullptr);
	
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	if (!GetClassInfoEx(mWindowInstance, WINDOWCLASS, &windowClass)) {
		windowClass.cbSize        = sizeof(WNDCLASSEX);
		windowClass.style         = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc   = (WNDPROC)WindowProc;
		windowClass.hInstance     = mWindowInstance;
		windowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		windowClass.lpszClassName = WINDOWCLASS;

		if (!RegisterClassEx(&windowClass))
			return;
	}

	if (mIsFullScreen) {
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth        = mWindowWidth;
		dmScreenSettings.dmPelsHeight       = mWindowHeight;
		dmScreenSettings.dmBitsPerPel       = 32;
		dmScreenSettings.dmDisplayFrequency = 60;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			return;
	}

	mWindowHandle = CreateWindowEx(
		(mIsFullScreen ? WS_EX_TOPMOST : (DWORD)0), WINDOWCLASS, mTitle.c_str(),
		(mIsFullScreen ? WS_POPUP | WS_VISIBLE : WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
		mWindowX, mWindowY, mWindowWidth, mWindowHeight,
		nullptr, nullptr, mWindowInstance, nullptr
	);

	if (!mWindowHandle)
		return;

	getMouse().setAbsolutePositionBounds(mWindowWidth, mWindowHeight);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(mWindowHandle, &pt);

	mInitSuccess = true;
}

Window::~Window() {
	
}

bool Window::update() {
	MSG msg;

	while (PeekMessage(&msg, mWindowHandle, 0, 0, PM_REMOVE)) {
		handleInput(msg);
	}

	return !mForceQuit;
}

void Window::setMouseLock(bool lockMouse) {
	mLockMouse = lockMouse;
	if (mLockMouse) {
		RECT windowRect;
		GetWindowRect(mWindowHandle, &windowRect);

		SetCapture(mWindowHandle);
		ClipCursor(&windowRect);

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(mWindowHandle, &pt);
		getMouse().setAbsolutePosition(pt.x, pt.y);
	} else {
		ReleaseCapture();
		ClipCursor(nullptr);
	}
}

void Window::showOSPointer(bool show) {
	if (show == mShowMouse)
		return;

	mShowMouse = show;
	ShowCursor(mShowMouse ? 1 : 0);
}

void Window::setTitle(const std::string& title) {
	mTitle = title;
	SetWindowText(mWindowHandle, mTitle.c_str());
}

void Window::setRenderer(RendererBase* renderer) {
	if (mRenderer && mRenderer != renderer)
		mRenderer->onWindowDetach();

	mRenderer = renderer;

	resizeRenderer();
}

RendererBase* Window::getRenderer() const {
	return mRenderer;
}

void Window::resizeRenderer() {
	if (mRenderer)
		mRenderer->onWindowResize(mWindowWidth, mWindowHeight);
}

Keyboard& Window::getKeyboard() {
	static Keyboard keyboard = Keyboard(getWindow().mWindowHandle);
	return keyboard;
}

Mouse& Window::getMouse() {
	static Mouse mouse = Mouse(getWindow().mWindowHandle);
	return mouse;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	bool doResize = false;
	switch (message) {
		case WM_DESTROY: {
			getWindow().showOSPointer(true);
			getWindow().setMouseLock(false);

			PostQuitMessage(0);
			getWindow().mForceQuit = true;
		}break;
		case WM_ACTIVATE: {
			if (LOWORD(wParam) == WA_INACTIVE) {
				getWindow().mIsActive = false;
				ReleaseCapture();
				ClipCursor(nullptr);
				getMouse().sleep();
				getKeyboard().sleep();
			} else if (getWindow().initSuccess()) {
				getWindow().mIsActive = true;
				getMouse().wake();
				getKeyboard().wake();

				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(getWindow().mWindowHandle, &pt);
				getMouse().setAbsolutePosition(pt.x, pt.y);

				if (getWindow().mLockMouse)
					getWindow().setMouseLock(true);
			}
		}return 0;
		case WM_LBUTTONDOWN: {
			if (getWindow().mLockMouse)
				getWindow().setMouseLock(true);
		}break;
		case WM_MOUSEMOVE: {
			TRACKMOUSEEVENT tme{};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = getWindow().mWindowHandle;
			TrackMouseEvent(&tme);

			if (!getWindow().mMouseLeftWindow)
				break;
			getWindow().mMouseLeftWindow = true;
			getMouse().wake();
			getKeyboard().wake();

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(getWindow().mWindowHandle, &pt);
			getMouse().setAbsolutePosition(pt.x, pt.y);
		}break;
		case WM_MOUSELEAVE: {
			getWindow().mMouseLeftWindow = true;
			getMouse().sleep();
			getKeyboard().sleep();
		}break;
		case WM_SIZE: {
			LONG newWidth  = LOWORD(lParam);
			LONG newHeight = HIWORD(lParam);

			if (newWidth > 0 && newHeight > 0 && (newWidth != getWindow().mWindowWidth || newHeight != getWindow().mWindowHeight)) {
				getWindow().mWindowWidth = newWidth;
				getWindow().mWindowHeight = newHeight;
			}

			if (wParam == SIZE_MINIMIZED) {
				getWindow().mIsMinimized = true;
			}

			if (wParam == SIZE_MAXIMIZED) {
				getWindow().mIsMaximized = true;
				getWindow().mIsMinimized = false;
				doResize = true;
			} else if (wParam == SIZE_RESTORED && getWindow().mIsMaximized) {
				getWindow().mIsMaximized = false;
				doResize = true;
			} else if (wParam == SIZE_RESTORED && getWindow().mIsMinimized) {
				getWindow().mIsMinimized = false;
				doResize = true;
			}
		}break;
		case WM_ENTERSIZEMOVE:
			break;
		case WM_EXITSIZEMOVE: {
			doResize = true;
		}break;
	}

	if (doResize) {
		getWindow().resizeRenderer();
		if (getWindow().initSuccess()) {
			getMouse().setAbsolutePositionBounds(getWindow().mWindowWidth, getWindow().mWindowHeight);
			getWindow().setMouseLock(getWindow().mLockMouse);
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Window::handleInput(MSG& msg) {
	switch (msg.message) {
		case WM_QUIT: case WM_CLOSE:
			mForceQuit = true;
			break;
		case WM_INPUT: {
			UINT dwSize{};
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

			BYTE* lpb = new BYTE[dwSize];

			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (mIsActive && raw->header.dwType == RIM_TYPEKEYBOARD)
				getKeyboard().update(raw);
			if (mIsActive && raw->header.dwType == RIM_TYPEMOUSE)
				getMouse().update(raw);
			delete[] lpb;
		}break;
		default: {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}break;
	}
}
