/**
 * @file   Renderer.h
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
	class Renderer {
	public:
		Renderer(Application::Window& window);
		~Renderer();

		void swapBuffers();

		void onWindowResize(GLsizei width, GLsizei height);
		void onWindowDetach();

		inline bool initSuccess() const {
			return mInitSuccess;
		}
	protected:
		Application::Window& mWindow;

		HDC   mDeviceContext;
		HGLRC mRenderContext;
	private:
		bool mInitSuccess = false;
	};
}
