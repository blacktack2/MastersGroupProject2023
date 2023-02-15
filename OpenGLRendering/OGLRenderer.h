/**
 * @file   OGLRenderer.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RendererBase.h"

#include "Vector3.h"
#include "Vector4.h"

#ifdef _WIN32
#include "windows.h"
#endif

#include <string>
#include <vector>

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif

namespace NCL {
	class MeshGeometry;
}

namespace NCL::Maths {
	class Matrix4;
}

namespace NCL::Rendering {
	class OGLMesh;
	class OGLRenderPass;
	class IPostRenderPass;
	class OGLShader;
	class OGLTexture;

	class SimpleFont;
		
	class OGLRenderer : public RendererBase {
	public:
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
	protected:
		void BeginFrame() override;
		void EndFrame() override;
		void SwapBuffers() override;
		void ClearBackbuffer() override;

#ifdef _WIN32
		void InitWithWin32(Window& w);
		void DestroyWithWin32();

		HDC deviceContext;
		HGLRC renderContext;
#endif

	private:
		bool initState;
		bool forceValidDebugState;
	};
}
