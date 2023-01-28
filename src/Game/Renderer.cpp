/**
 * @file   Renderer.cpp
 * @brief  See Renderer.h.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Renderer.h"

using namespace Graphics;

Renderer::Renderer() : RendererBase() {
}

Renderer::~Renderer() {
}

void Renderer::update() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render() {
}
