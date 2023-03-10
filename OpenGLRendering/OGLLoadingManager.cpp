#include "OGLLoadingManager.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLRenderer.h"
#include "AssetLibrary.h"

using namespace NCL;

OGLLoadingManager::OGLLoadingManager(Window* w, RendererBase* renderer) : LoadingMangerBase(w) {
	quad = new OGLMesh();
	quad->SetVertexPositions({
			Vector3(-1, 1, -1),
			Vector3(-1, -1, -1),
			Vector3(1, -1, -1),
			Vector3(1, 1, -1),
	});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->UploadToGPU();
	shader = new OGLShader("loading.vert", "loading.frag");
	shader->Initilize();

	this->renderer = (OGLRenderer*)renderer;

	loadingContext = this->renderer->CreateContext();
}

OGLLoadingManager::~OGLLoadingManager() {
	delete quad;
	delete shader;

	wglDeleteContext(loadingContext);
}

void OGLLoadingManager::Load(std::function<void()> func) {
	loadingComplete = false;
	std::thread t(&OGLLoadingManager::RunFunc, this, func);
	DisplayLoadingScreen();
	t.join();
	AssetLibrary<MeshGeometry>::RunOnAssets([](std::shared_ptr<MeshGeometry> item)->void {((OGLMesh*)item.get())->UploadToGPU(); });
	AssetLibrary<ShaderBase>::RunOnAssets([](std::shared_ptr<ShaderBase> item)->void {((OGLShader*)item.get())->Initilize(); });
}

void OGLLoadingManager::RunFunc(std::function<void()> func) {
	renderer->MakeCurrent(loadingContext);
	func();
	loadingComplete = true;
}

void OGLLoadingManager::DisplayLoadingScreen() {
	float time = 0;
	//renderer->MakeCurrent(renderer->renderContext);
	while (window->UpdateWindow())
	{
		time += window->GetTimer()->GetTimeDeltaSeconds();
		while (time > 1)
		{
			time -= 1;
		}
		renderer->BeginFrame();
		renderer->ClearBuffers(ClearBit::ColorDepth);
		shader->Bind();
		shader->SetUniformFloat("screenSize", window->GetScreenSize());
		shader->SetUniformFloat("time", time);
		quad->Draw();

		renderer->EndFrame();
		renderer->SwapBuffers();

		if (loadingComplete) break;
	}
	
}