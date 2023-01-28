/**
 * @file   Renderer.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once;
#include "../Window/RendererBase.h"
#include "../Window/Window.h"

class Renderer : public Graphics::RendererBase {
public:
	Renderer(Graphics::Application::Window& window);
	~Renderer();

	virtual void update();
	virtual void render();
private:
};
