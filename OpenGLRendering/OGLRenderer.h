/**
 * @file   OGLRenderer.h
 * @brief  Base OpenGL implementation of the main renderer.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RendererBase.h"

#include "OGLRendererConfig.h"

#ifdef _WIN32
#include "Windows.h"
#endif

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif

namespace NCL {
	class OGLLoadingManager;

	namespace Rendering {
		class OGLMesh;
		class OGLRenderPass;
		class IPostRenderPass;
		class OGLShader;
		class OGLTexture;

		class SimpleFont;

		/**
		 * @brief Base OpenGL implementation of the main renderer.
		 */
		class OGLRenderer : public RendererBase {
		public:
			friend class OGLLoadingManager;
			OGLRenderer(Window& w);
			~OGLRenderer();

			void OnWindowResize(int width, int height) override;
			bool HasInitialised() const override {
				return initState;
			}

			void ForceValidDebugState(bool newState) {
				forceValidDebugState = newState;
			}

			virtual bool SetVerticalSync(VerticalSyncState s);

			RendererConfigBase& GetConfig() override {
				return config;
			}

			void ClearBuffers(ClearBit mask) override;
		protected:
			void BeginFrame() override;
			void EndFrame() override;

			void SwapBuffers() override;
			void ClearBackbuffer() override;

#ifdef _WIN32
			void InitWithWin32(Window& w);

		public:
			HGLRC CreateContext();
			void MakeCurrent(HGLRC context) {
				wglMakeCurrent(deviceContext, context);
			}
		protected:
			void DestroyWithWin32();

			HDC deviceContext   = nullptr;
			HGLRC renderContext = nullptr;
#endif
		private:
			OGLRendererConfig config;

			bool initState = false;
			bool forceValidDebugState = false;
		};
	}
}
