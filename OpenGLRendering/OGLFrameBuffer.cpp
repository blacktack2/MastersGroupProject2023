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

void OGLFrameBuffer::DrawBuffers() {
	DrawBuffers(numColourTexs);
}

bool OGLFrameBuffer::InitSuccess() {
	return false;
}

std::unique_ptr<FrameBuffer> OGLFrameBuffer::CreateFrameBuffer() {
	return std::make_unique<OGLFrameBuffer>();
}

void OGLFrameBuffer::BindToTexture(TextureBase& texture, unsigned int attachment) {
	GLenum attach;
	switch (texture.GetBufferType()) {
		default:
		case BufferType::ColourAttachment   : attach = GL_COLOR_ATTACHMENT0 + numColourTexs++; break;
		case BufferType::DepthAttachment   : attach = GL_DEPTH_ATTACHMENT                   ; break;
		case BufferType::StencilAttachment : attach = GL_STENCIL_ATTACHMENT                 ; break;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, static_cast<OGLTexture&>(texture).GetObjectID(), 0);
}

void OGLFrameBuffer::DrawBuffers(unsigned int numBuffers) {
	if (numBuffers == 0) {
		glDrawBuffer(GL_NONE);
	} else {
		glDrawBuffers(numBuffers, colourBuffers);
	}
}
