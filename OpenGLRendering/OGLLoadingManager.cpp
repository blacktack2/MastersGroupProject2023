#include "OGLLoadingManager.h"

#include "OGLMesh.h"
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"

#include "AssetLibrary.h"

using namespace NCL;

OGLLoadingManager::OGLLoadingManager(Window* w, RendererBase& renderer) : LoadingMangerBase(w),
renderer(static_cast<OGLRenderer&>(renderer)) {
	quad = std::make_unique<OGLMesh>();
	quad->SetVertexPositions({
			Vector3(-1,  1, -1),
			Vector3(-1, -1, -1),
			Vector3( 1, -1, -1),
			Vector3( 1,  1, -1),
	});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->UploadToGPU();
	shader = std::make_unique<OGLShader>("loading.vert", "loading.frag");
	shader->Initialize();

	loadingContext = this->renderer.CreateContext();
}

OGLLoadingManager::~OGLLoadingManager() {
	wglDeleteContext(loadingContext);
}

void OGLLoadingManager::Load(std::function<void()> func) {
	loadingComplete = false;
	std::thread t(&OGLLoadingManager::RunFunc, this, func);
	DisplayLoadingScreen();
	t.join();
	float time = 0;
	AssetLibrary<MeshGeometry>::RunOnAssets([&](MeshGeometry& asset)->void {
		time += window->GetTimer()->GetTimeDeltaSeconds(); 
		if (time >= 0.25f)
		{
			shaderTime += time;
			while (shaderTime > 1) {
				shaderTime -= 1;
			}
			renderer.BeginFrame();
			renderer.ClearBuffers(ClearBit::ColorDepth);
			shader->Bind();
			shader->SetUniformFloat("screenSize", window->GetScreenSize());
			shader->SetUniformFloat("time", shaderTime);
			quad->Draw();

			renderer.EndFrame();
			renderer.SwapBuffers();
			time = 0;
		}
		asset.UploadToGPU();
		});
	AssetLibrary<ShaderBase>  ::RunOnAssets([&](ShaderBase&   asset)->void { 
		time += window->GetTimer()->GetTimeDeltaSeconds();
		if (time >= 0.25f)
		{
			shaderTime += time;
			while (shaderTime > 1) {
				shaderTime -= 1;
			}
			renderer.BeginFrame();
			renderer.ClearBuffers(ClearBit::ColorDepth);
			shader->Bind();
			shader->SetUniformFloat("screenSize", window->GetScreenSize());
			shader->SetUniformFloat("time", time);
			quad->Draw();

			renderer.EndFrame();
			renderer.SwapBuffers();
			time = 0;
		}
		asset.Initialize() ; });
	/*AssetLibrary<TextureBase> ::RunOnAssets([&](TextureBase& asset)->void {
		time += window->GetTimer()->GetTimeDeltaSeconds();
		if (time >= 0.25f)
		{
			shaderTime += time;
			while (shaderTime > 1) {
				shaderTime -= 1;
			}
			renderer.BeginFrame();
			renderer.ClearBuffers(ClearBit::ColorDepth);
			shader->Bind();
			shader->SetUniformFloat("screenSize", window->GetScreenSize());
			shader->SetUniformFloat("time", shaderTime);
			quad->Draw();

			renderer.EndFrame();
			renderer.SwapBuffers();
			time = 0;
		}
		asset.Initialize(); 
		});*/

	shader->Unbind();
}

void OGLLoadingManager::RunFunc(std::function<void()> func) {
	renderer.MakeCurrent(loadingContext);
	func();
	loadingComplete = true;
}

void OGLLoadingManager::DisplayLoadingScreen() {
	shaderTime = 0;
	//renderer->MakeCurrent(renderer->renderContext);
	while (window->UpdateWindow()) {
		shaderTime += window->GetTimer()->GetTimeDeltaSeconds();
		while (shaderTime > 1) {
			shaderTime -= 1;
		}
		renderer.BeginFrame();
		renderer.ClearBuffers(ClearBit::ColorDepth);
		shader->Bind();
		shader->SetUniformFloat("screenSize", window->GetScreenSize());
		shader->SetUniformFloat("time", shaderTime);
		quad->Draw();

		renderer.EndFrame();
		renderer.SwapBuffers();

		if (loadingComplete) break;
	}
}
