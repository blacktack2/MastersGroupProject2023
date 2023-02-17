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

void OGLBufferObject::Read(void* data, int offset, int amount) {
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, amount == 0 ? size - offset : amount, data);
}

void OGLBufferObject::Write(const void* data, int offset, int amount) {
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, amount == 0 ? size - offset : amount, data);
}

void OGLBufferObject::Bind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
}

void OGLBufferObject::Unbind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void OGLBufferObject::OnResize() {
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_READ);
}
