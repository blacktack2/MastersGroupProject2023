/**
 * @file   RendererBase.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include "Window.h"

#include <glad/gl.h>

namespace Graphics {
	enum class VSyncState {
		OFF      = 0,
		ON       = 1,
		ADAPTIVE = -1,
	};

	class RendererBase {
	public:
		RendererBase(Application::Window& window);
		~RendererBase();

		bool setVSync(VSyncState state);

		void swapBuffers();

		void onWindowResize(GLsizei width, GLsizei height);
		void onWindowDetach();

		virtual void update() = 0;
		virtual void render() = 0;

		inline bool initSuccess() const {
			return mInitSuccess;
		}
	protected:
		Application::Window& mWindow;

		HDC   mDeviceContext;
		HGLRC mRenderContext;
	private:
		VSyncState mCurrentVSync = VSyncState::OFF;
		bool mInitSuccess = false;
	};
}
