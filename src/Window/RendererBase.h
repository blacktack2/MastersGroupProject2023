/**
 * @file   RendererBase.h
 * @brief  Base class for handling OpenGL operations and drawing to the window.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include <glad/gl.h>

#include <Windows.h>

namespace Graphics {
	enum class VSyncState {
		OFF      =  0,
		ON       =  1,
		ADAPTIVE = -1,
	};

	/**
	 * @brief Base class for handling OpenGL operations and drawing to the window.
	 */
	class RendererBase {
	public:
		/**
		 * @brief Construct new RendererBase and initialize OpenGL with the Window.
		 */
		RendererBase();
		~RendererBase();

		/**
		 * @param state Mode to set OpenGL VSync. Use VSyncState::OFF to
		 * disable.
		 */
		bool setVSync(VSyncState state);

		/**
		 * @brief Swap the application's double buffers, and wait until next
		 * frame if VSync is enabled.
		 */
		void swapBuffers();

		/**
		 * @brief Update the viewport to match the window size.
		 */
		void onWindowResize();

		/**
		 * @brief Event method to be called when the window detaches from this
		 * renderer.
		 */
		virtual void onWindowDetach() {}

		/**
		 * @brief Update method for handling changes to objects every frame. Should
		 * be called once every frame.
		 */
		virtual void update() = 0;
		/**
		 * @brief Update method for drawing the current world state to the window.
		 * Should be called once every frame, after calling RendererBase::update.
		 */
		virtual void render() = 0;

		/**
		 * @returns false if initialization has failed, otherwise true.
		 */
		inline bool initSuccess() const {
			return mInitSuccess;
		}
	protected:
		HDC   mDeviceContext;
		HGLRC mRenderContext;
	private:
		VSyncState mCurrentVSync = VSyncState::OFF;
		bool mInitSuccess = false;
	};
}
