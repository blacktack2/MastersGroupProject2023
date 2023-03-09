#include "OGLLoadingManager.h"
#include "OGLMesh.h"
#include "OGLShader.h"
#include "OGLRenderer.h"

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

	this->renderer = (OGLRenderer*)renderer;

	loadingContext = this->renderer->CreateContext();
}

OGLLoadingManager::~OGLLoadingManager() {
	delete quad;
	delete shader;

	wglDeleteContext(loadingContext);
}