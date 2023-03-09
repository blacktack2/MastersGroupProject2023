#include "OGLRenderer.h"
#include "Window.h"
#include "GameTechRenderer.h"
#include <functional>
#include <thread>
#include <chrono>
#include <iostream>

namespace paintHell::core {
	class LoadingScreen{
	private:

		void DisplayLoadingScreen() {
			
			using namespace std::chrono_literals;
			while (window->UpdateWindow())
			{
				float dt = window->GetTimer()->GetTimeDeltaSeconds();
				//std::cout << "*";

				renderer.LoadingRender(dt, shader, simpleMesh);

				if (loadingComplete) break;
			}
			std::cout << "\n";
		}

		void RunFunc(std::function<void()> func) {
			func();
			loadingComplete = true;
		}


		bool loadingComplete;
		ShaderBase* shader;
		MeshGeometry* simpleMesh;
		Window* window;
		GameTechRenderer& renderer = GameTechRenderer::instance();

	public:

		LoadingScreen(Window* w) {
			loadingComplete = false;
			shader = new OGLShader("loading.vert", "loading.frag");
			simpleMesh = new OGLMesh();
			simpleMesh->SetVertexPositions({
				Vector3(-1,  1, -1),
				Vector3(-1, -1, -1),
				Vector3(1, -1, -1),
				Vector3(1,  1, -1),
				});
			simpleMesh->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
			simpleMesh->UploadToGPU();
			window = w;
		}
		~LoadingScreen() {
		}

		void Load(std::function<void()> func) {
			loadingComplete = false;
			std::thread t(&LoadingScreen::RunFunc, this, func);
			DisplayLoadingScreen();
			t.join();
		}
	};
}