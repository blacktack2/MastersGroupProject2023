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
	/**
	 * @brief Wrapper class for handling OpenGL shaders.
	 */
	class Shader {
	public:
		/**
		 * @brief Create and compile a new shader from source.
		 * @param vert Code for the vertex shader.
		 * @param frag Code for the fragment shader.
		 */
		Shader(const GLchar* vert, const GLchar* frag);
		/**
		 * @brief Create and compile a new shader from source.
		 * @param vert Code for the vertex shader.
		 * @param frag Code for the fragment shader.
		 * @param geom Code for the geometry shader.
		 */
		Shader(const GLchar* vert, const GLchar* frag, const GLchar* geom);
		/**
		 * @brief Create and compile a new shader from source.
		 * @param vert Code for the vertex shader.
		 * @param frag Code for the fragment shader.
		 * @param tesc Code for the tesselation control shader.
		 * @param tese Code for the tesselation evaulation shader.
		 */
		Shader(const GLchar* vert, const GLchar* frag, const GLchar* tesc, const GLchar* tese);
		/**
		 * @brief Create and compile a new shader from source.
		 * @param vert Code for the vertex shader.
		 * @param frag Code for the fragment shader.
		 * @param tesc Code for the tesselation control shader.
		 * @param tese Code for the tesselation evaulation shader.
		 * @param geom Code for the geometry shader.
		 */
		Shader(const GLchar* vert, const GLchar* frag, const GLchar* tesc, const GLchar* tese, const GLchar* geom);
		~Shader();

		inline void bind() {
			glUseProgram(mProgramID);
		}
		inline void unbind() {
			glUseProgram(0);
		}
	private:
		/**
		 * @brief Create and compile shader from source.
		 * @param code Shader code.
		 * @param type OpenGL shader type.
		 * @returns true if successfully compiled, otherwise false.
		 */
		bool loadShader(const GLchar* code, GLenum type);
		/**
		 * @brief Link the shader program.
		 * @returns true if successfully linked, otherwise false.
		 */
		bool linkShader();

		GLuint mProgramID;

		bool mInitSuccess = false;
	};
}
