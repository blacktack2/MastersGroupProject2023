/**
 * @file   OGLShader.cpp
 * @brief  See OGLShader.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLShader.h"
#include "Assets.h"
#include <iostream>

using namespace NCL;
using namespace NCL::Rendering;

GLuint shaderTypes[(size_t)ShaderStage::Max] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER
};

std::string ShaderNames[(size_t)ShaderStage::Max] = {
	"Vertex",
	"Fragment",
	"Geometry",
	"Tess. Control",
	"Tess. Eval"
};

OGLShader::OGLShader(const std::string& vert, const std::string& frag, const std::string& tesc, const std::string& tese, const std::string& geom) :
ShaderBase(vert, frag, tesc, tese, geom) {
	Clear();
	ReloadShader();
}

OGLShader::~OGLShader() {
	DeleteIDs();
}

void OGLShader::Bind() {
	glUseProgram(programID);
}

void OGLShader::Unbind() {
	glUseProgram(0);
}

void OGLShader::ReloadShader() {
	DeleteIDs();
	programID = glCreateProgram();
	std::string fileContents = "";
	for (size_t i = 0; i < (size_t)ShaderStage::Max; i++) {
		if (shaderFiles[i].empty() || !Assets::ReadTextFile(Assets::SHADERDIR + shaderFiles[i], fileContents))
			continue;
		LoadPass((GLchar*)fileContents.c_str(), (ShaderStage)i);
	}
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &programValid);

	PrintLinkLog(programID);

	if (programValid != GL_TRUE) {
		std::cout << "This shader has failed!" << std::endl;
	} else {
		std::cout << "Shader loaded!" << std::endl;
	}
}

void OGLShader::LoadPass(const GLchar* code, ShaderStage type) {
	size_t index = (size_t)type;
	GLuint shaderID = glCreateShader(shaderTypes[index]);

	std::cout << "Reading " << ShaderNames[index] << " shader " << shaderFiles[index] << "\n";

	glShaderSource(shaderID, 1, &code, nullptr);
	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderValid[index]);
	if (shaderValid[index] != GL_TRUE) {
		std::cout << ShaderNames[index] << " shader " << " has failed!" << "\n";
		return;
	}
	glAttachShader(programID, shaderID);

	PrintCompileLog(shaderID);

	glDeleteShader(shaderID);
}

void OGLShader::Clear() {
	programID = 0;
	programValid = GL_FALSE;
	for (size_t i = 0; i < (size_t)ShaderStage::Max; i++) {
		shaderValid[i] = GL_FALSE;
	}
}

void OGLShader::DeleteIDs() {
	if (!programID) {
		return;
	}
	glDeleteProgram(programID);
	Clear();
}

void OGLShader::PrintCompileLog(GLuint object) {
	GLint logLength = 0;
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength == 0) {
		return;
	}

	std::string infoLog((size_t)logLength, 0);
	glGetShaderInfoLog(object, logLength, NULL, infoLog.data());
	std::cout << "Compile Log:\n" << infoLog << "\n";
}

void OGLShader::PrintLinkLog(GLuint program) {
	GLint logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength == 0) {
		return;
	}

	std::string infoLog((size_t)logLength, 0);
	glGetProgramInfoLog(program, logLength, NULL, infoLog.data());
	std::cout << "Link Log:\n" << infoLog << "\n";
}
