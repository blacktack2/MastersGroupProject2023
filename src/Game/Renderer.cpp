/**
 * @file   Renderer.cpp
 * @brief  See Renderer.h.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Renderer.h"

#include <string>
#include <fstream>

using namespace paintHell;
using namespace core;
using namespace graphics;

Renderer::Renderer() : RendererBase() {
	std::string vert, frag;

	std::ifstream vStream("Shaders/test.vert");
	vStream.seekg(0, std::ios::end);
	vert.reserve(vStream.tellg());
	vStream.seekg(0, std::ios::beg);

	vert.assign(std::istreambuf_iterator<char>(vStream), std::istreambuf_iterator<char>());

	std::ifstream fStream("Shaders/test.frag");
	fStream.seekg(0, std::ios::end);
	frag.reserve(fStream.tellg());
	fStream.seekg(0, std::ios::beg);

	frag.assign(std::istreambuf_iterator<char>(fStream), std::istreambuf_iterator<char>());

	quad = Mesh::generateQuad();
	shader = std::make_shared<Shader>(vert.c_str(), frag.c_str());
}

Renderer::~Renderer() {
}

void Renderer::update() {
	glClear(GL_COLOR_BUFFER_BIT);
	shader->bind();
	quad->draw();
	shader->unbind();
}

void Renderer::render() {
}
