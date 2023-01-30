/**
 * @file   Shader.h
 * @brief  Wrapper class for handling OpenGL shaders.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include <glad/gl.h>

#include <string>

namespace Graphics {
	class Shader {
	public:
		Shader(const GLchar* vert, const GLchar* frag);
		Shader(const GLchar* vert, const GLchar* frag, const GLchar* geom);
		Shader(const GLchar* vert, const GLchar* frag, const GLchar* tesc, const GLchar* tese);
		Shader(const GLchar* vert, const GLchar* frag, const GLchar* tesc, const GLchar* tese, const GLchar* geom);
		~Shader();

		inline void bind() {
			glUseProgram(mProgramID);
		}
		inline void unbind() {
			glUseProgram(0);
		}
	private:
		bool loadShader(const GLchar* code, GLenum type);
		bool linkShader();

		GLuint mProgramID;

		bool mInitSuccess = false;
	};
}
