/**
 * @file   OGLFrameBuffer.cpp
 * @brief  See OGLFrameBuffer.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLFrameBuffer.h"

#include "OGLTexture.h"

using namespace NCL::Rendering;

// Can be safely expanded to include additional buffers if necessary
static const GLenum colourBuffers[]{
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
};

OGLFrameBuffer::OGLFrameBuffer() {
	glGenFramebuffers(1, &fboID);
}

OGLFrameBuffer::~OGLFrameBuffer() {
	glDeleteFramebuffers(1, &fboID);
}

void OGLFrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void OGLFrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLFrameBuffer::AddTexture(OGLTexture* texture) {
	textures.emplace_back(texture);
	GLenum attachment;
	switch (texture->GetType()) {
		default:
		case TexType::Colour  : attachment = GL_COLOR_ATTACHMENT0 + numColourTexs++; break;
		case TexType::Depth   :
		case TexType::Shadow  : attachment = GL_DEPTH_ATTACHMENT  ; break;
		case TexType::Stencil : attachment = GL_STENCIL_ATTACHMENT; break;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->GetObjectID(), 0);
}

void OGLFrameBuffer::DrawBuffers() {
	if (numColourTexs == 0) {
		glDrawBuffer(GL_NONE);
	} else {
		glDrawBuffers(numColourTexs, colourBuffers);
	}
}