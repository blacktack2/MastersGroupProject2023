/**
 * @file   TextureBase.cpp
 * @brief  See TextureBase.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "TextureBase.h"

using namespace NCL::Rendering;

TextureBase::TextureBase(TextureType type) : type(type) {
	switch (type) {
		default: bufferType = BufferType::ColourAttachment; break;
		case TextureType::Depth:
		case TextureType::Shadow : bufferType = BufferType::DepthAttachment; break;
		case TextureType::Stencil: bufferType = BufferType::StencilAttachment; break;
	}
}


TextureBase::~TextureBase() {
}
