#include "RendererBase.h"

using namespace NCL;
using namespace Rendering;

RendererBase::RendererBase(Window& window) : hostWindow(window)	{
	windowWidth  = window.GetScreenSize().x;
	windowHeight = window.GetScreenSize().y;
}


RendererBase::~RendererBase() {
}

void RendererBase::OnWindowResize(int width, int height) {
	for (IMainRenderPass* pass : mainRenderPasses) {
		pass->OnWindowResize(width, height);
	}
	for (IPostRenderPass* pass : postRenderPasses) {
		pass->OnWindowResize(width, height);
	}
	for (IOverlayRenderPass* pass : overlayRenderPasses) {
		pass->OnWindowResize(width, height);
	}
}

void RendererBase::RenderFrame() {
	for (auto pass : mainRenderPasses) {
		pass->Render();
	}
	for (auto pass : postRenderPasses) {
		pass->Render();
	}
	for (auto pass : overlayRenderPasses) {
		pass->Render();
	}
}
