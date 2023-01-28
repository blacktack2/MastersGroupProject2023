#include "Renderer.h"

Renderer::Renderer(Graphics::Application::Window& window) : RendererBase(window) {
}

Renderer::~Renderer() {
}

void Renderer::update() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render() {
}
