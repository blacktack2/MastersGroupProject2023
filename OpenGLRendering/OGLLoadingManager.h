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
		OGLLoadingManager(Window* w, RendererBase& base);
		~OGLLoadingManager();

		void RunFunc(std::function<void()> func);


		virtual void Load(std::function<void()> func) override;
	protected:
		virtual void DisplayLoadingScreen() override;
	private:
		std::unique_ptr<OGLMesh> quad;
		std::unique_ptr<OGLShader> shader;
		OGLRenderer& renderer;
		float shaderTime = 0;

#ifdef _WIN32
		HGLRC loadingContext;
#endif
	};
}
