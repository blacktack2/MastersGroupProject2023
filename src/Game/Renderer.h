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

#include "../Graphics/Shader.h"
#include "../Graphics/Mesh.h"

#include <memory>

using namespace Graphics;

class Renderer : public RendererBase {
public:
	Renderer();
	~Renderer();

	virtual void update();
	virtual void render();
private:
	std::shared_ptr<Mesh> quad = nullptr;

	std::shared_ptr<Shader> shader = nullptr;
};
