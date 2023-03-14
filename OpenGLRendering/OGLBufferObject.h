/**
 * @file   OGLBufferObject.h
 * @brief  OpenGL implementation of an SSBO.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "BufferObjectBase.h"

#include <glad/gl.h>

#include <memory>
#include <vector>

namespace NCL::Rendering {
	/**
	 * @brief OpenGL implementation of an SSBO.
	 */
	class OGLBufferObject : public BufferObjectBase {
	public:
		OGLBufferObject(size_t size, GLuint binding);
		~OGLBufferObject();

		virtual void Read(void* dataOut, int offset = 0, int amount = 0) override;
		virtual void Write(const void* dataIn, int offset = 0, int amount = 0) override;

		virtual void Bind() override;
		virtual void Unbind() override;

		static std::unique_ptr<BufferObjectBase> CreateBufferObject(size_t size, unsigned int binding);
	protected:
		virtual void OnResize() override;
	private:
		GLuint id;
		GLuint binding;
	};
}
