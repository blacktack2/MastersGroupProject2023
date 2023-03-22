/**
 * @file   RendererConfigBase.cpp
 * @brief  See RendererConfigBase.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "RendererConfigBase.h"

using namespace NCL::Rendering;

RendererConfigBase::RendererConfigBase(const RendererBase& renderer) : renderer(renderer) {
}

void RendererConfigBase::ResetAll() {
	SetDefaultViewport();
	SetClearColour();

	SetAlphaTest();
	SetBlend();
	SetColourMask();
	SetCullFace();
	SetDepthTest();
	SetDepthMask();

	SetPatchVertices();
}
