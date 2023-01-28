/**
 * @file   Renderer.h
 * @brief  Main Renderer implementation for the game.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once;
#include "../Window/RendererBase.h"
#include "../Window/Window.h"

using namespace Graphics;

class Renderer : public RendererBase {
public:
	Renderer();
	~Renderer();

	virtual void update();
	virtual void render();
private:
};
