/**
 * @file   OGLBufferObject.cpp
 * @brief  See OGLBufferObject.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLBufferObject.h"

using namespace NCL::Rendering;

OGLBufferObject::OGLBufferObject(size_t size, GLuint binding) : BufferObjectBase(size, binding) {
	glGenBuffers(1, &id);

	Bind();

	OnResize();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);

	Unbind();
}

OGLBufferObject::~OGLBufferObject() {
	glDeleteBuffers(1, &id);
}

void OGLBufferObject::Read(void* data, int offset, int amount) const {
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, amount == 0 ? size - offset : amount, data);
}

void OGLBufferObject::Write(const void* data, int offset, int amount) {
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, amount == 0 ? size - offset : amount, data);
}

void OGLBufferObject::Bind() const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
}

void OGLBufferObject::Unbind() const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

std::unique_ptr<BufferObjectBase> OGLBufferObject::CreateBufferObject(size_t size, unsigned int binding) {
	return std::make_unique<OGLBufferObject>(size, binding);
}

void OGLBufferObject::OnResize() {
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_READ);
}
