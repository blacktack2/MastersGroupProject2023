#pragma once
#include "LoadingManager.h"

#ifdef _WIN32
#include "windows.h"
#endif

namespace NCL {
	namespace Rendering {
		class OGLMesh;
		class OGLShader;
		class OGLRenderer;
	}

	class OGLLoadingManager : LoadingMangerBase {
	public:
		OGLLoadingManager(Window* w, RendererBase* base);
		~OGLLoadingManager();


		virtual void Load(std::function<void()> func) override {}
	protected:
		virtual void DisplayLoadingScreen() override {}
	private:
		OGLMesh* quad;
		OGLShader* shader;
		OGLRenderer* renderer;
#ifdef _WIN32
		HGLRC loadingContext;
#endif
	};
}