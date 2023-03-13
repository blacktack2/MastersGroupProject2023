/**
 * @file   RendererConfigBase.cpp
 * @brief  See RendererConfigBase.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "RendererConfigBase.h"

#include "RendererBase.h"

using namespace NCL;
using namespace Rendering;

RendererConfigBase::RendererConfigBase(RendererBase& renderer) : renderer(renderer) {
}

void RendererConfigBase::ResetAll() {
	SetDefaultViewport();
	SetClearColour();
	SetAlphaTest();
	SetBlend();
	SetCullFace();
	SetDepthTest();
	SetDepthMask();
}
