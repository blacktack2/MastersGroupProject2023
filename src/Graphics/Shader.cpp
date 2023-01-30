/**
 * @file   Shader.cpp
 * @brief  See Shader.h.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Shader.h"

#include "../Debugging/Logger.h"

using namespace Graphics;

Shader::Shader(const GLchar* vert, const GLchar* frag) : mProgramID(glCreateProgram()) {
	loadShader(vert, GL_VERTEX_SHADER);
	loadShader(frag, GL_FRAGMENT_SHADER);

	linkShader();
}

Shader::Shader(const GLchar* vert, const GLchar* frag, const GLchar* geom) :
mProgramID(glCreateProgram()) {
	loadShader(vert, GL_VERTEX_SHADER);
	loadShader(frag, GL_FRAGMENT_SHADER);
	loadShader(geom, GL_GEOMETRY_SHADER);

	linkShader();
}

Shader::Shader(const GLchar* vert, const GLchar* frag, const GLchar* tesc, const GLchar* tese) :
mProgramID(glCreateProgram()) {
	loadShader(vert, GL_VERTEX_SHADER);
	loadShader(frag, GL_FRAGMENT_SHADER);
	loadShader(tesc, GL_TESS_CONTROL_SHADER);
	loadShader(tese, GL_TESS_EVALUATION_SHADER);

	linkShader();
}

Shader::Shader(const GLchar* vert, const GLchar* frag, const GLchar* tesc, const GLchar* tese, const GLchar* geom) :
mProgramID(glCreateProgram()) {
	loadShader(vert, GL_VERTEX_SHADER);
	loadShader(frag, GL_FRAGMENT_SHADER);
	loadShader(tesc, GL_TESS_CONTROL_SHADER);
	loadShader(tese, GL_TESS_EVALUATION_SHADER);
	loadShader(geom, GL_GEOMETRY_SHADER);

	linkShader();
}

Shader::~Shader() {
	glDeleteProgram(mProgramID);
}

bool Shader::loadShader(const GLchar* code, GLenum type) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		Debug::Logger::getLogger().error(std::string("Failed to compile shader.\n").append(infoLog), __FILE__, __LINE__);
		return false;
	}

	glAttachShader(mProgramID, shader);
	glDeleteShader(shader);
	return true;
}

bool Shader::linkShader() {
	glLinkProgram(mProgramID);

	GLint success;
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
	if (success != GL_TRUE) {
		GLchar infoLog[512];
		glGetProgramInfoLog(mProgramID, 512, nullptr, infoLog);
		Debug::Logger::getLogger().error(std::string("Failed to link shader program.\n").append(infoLog), __FILE__, __LINE__);
		return false;
	}
	return true;
}
