/**
 * @file   OGLComputeShader.h
 * @brief  OpenGL implementation of a compute shader.
 * 
 * @author Rich Davidson
 * @date   February 2023
 */
#pragma once
#include "glad\gl.h"

#include <string>

namespace NCL {
	/**
	 * @brief OpenGL implementation of a compute shader.
	 */
	class OGLComputeShader {
	public:
		OGLComputeShader(const std::string& s);
		~OGLComputeShader();

		int GetProgramID() const {
			return programID;
		}

		void Bind() const;

		void Execute(int x, int y = 1, int z = 1) const;

		void GetThreadsInGroup(int&x, int& y, int&z) const;

		int GetThreadXCount() const;
		int GetThreadYCount() const;
		int GetThreadZCount() const;

		void Unbind();
	protected:
		GLuint shaderID;
		GLuint programID;
		int    programValid;
		GLint  threadsInGroup[3];
	};
}
