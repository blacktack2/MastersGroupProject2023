/**
 * @file   OGLShader.h
 * @brief  OpenGL implementation of a shader.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "ShaderBase.h"
#include "glad\gl.h"

#include <string>

namespace NCL::Rendering {
	/**
	 * @brief OpenGL implementation of a shader.
	 */
	class OGLShader : public ShaderBase {
	public:
		OGLShader(const std::string& vert, const std::string& frag, const std::string& tesc = "", const std::string& tese = "", const std::string& geom = "");
		~OGLShader();

		virtual void Bind() override;
		virtual void Unbind() override;
		void ReloadShader() override;

		bool LoadSuccess() const {
			return programValid == GL_TRUE;
		}

		int GetProgramID() const {
			return programID;
		}

		static void PrintCompileLog(GLuint object);
		static void PrintLinkLog(GLuint program);
	protected:
		void LoadPass(const GLchar* code, ShaderStage type);
		void Clear();
		void DeleteIDs();

		GLuint programID;
		GLint shaderValid[(size_t)ShaderStage::Max];
		GLint programValid;
	};
}
