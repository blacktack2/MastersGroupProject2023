/**
 * @file   RenderPassBase.cpp
 * @brief  See RenderPassBase.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "RenderPassBase.h"

using namespace NCL::Rendering;

RenderPassBase::RenderPassBase() {
}

void RenderPassBase::AddScreenTexture(TextureBase& tex) {
	textures.push_back(tex);
}

RenderPassBase::~RenderPassBase() {
}

void RenderPassBase::OnWindowResize(int width, int height) {
	for (auto tex : textures) {
		tex.get().Bind();
		tex.get().Resize(width, height);
	}
}
