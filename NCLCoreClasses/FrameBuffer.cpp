/**
 * @file   FrameBuffer.cpp
 * @brief  See FrameBuffer.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "FrameBuffer.h"

#include "TextureBase.h"

using namespace NCL::Rendering;

FrameBuffer::FrameBuffer() {
}

FrameBuffer::~FrameBuffer() {
}

void FrameBuffer::AddTexture(TextureBase& texture) {
	textures.emplace_back(texture);
}

void FrameBuffer::AddTexture(TextureBase& texture, unsigned int attachment) {
	if (attachment >= textures.size()) {
		textures.emplace_back(texture);
	} else {
		textures[attachment] = texture;
	}
	BindToTexture(texture, attachment);
}
