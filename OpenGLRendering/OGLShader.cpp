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

#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <iostream>

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
		if (!(shaderFiles[i].empty() || !Assets::ReadTextFile(Assets::SHADERDIR + shaderFiles[i], fileContents))) {
			LoadPass((GLchar*)fileContents.c_str(), (ShaderStage)i);
		}
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

void OGLShader::SetUniformFloat(const std::string& uniform, float v1) {
	glUniform1f(GetUniformLocation(uniform), v1);
}

void OGLShader::SetUniformFloat(const std::string& uniform, float v1, float v2) {
	glUniform2f(GetUniformLocation(uniform), v1, v2);
}

void OGLShader::SetUniformFloat(const std::string& uniform, float v1, float v2, float v3) {
	glUniform3f(GetUniformLocation(uniform), v1, v2, v3);
}

void OGLShader::SetUniformFloat(const std::string& uniform, float v1, float v2, float v3, float v4) {
	glUniform4f(GetUniformLocation(uniform), v1, v2, v3, v4);
}

void OGLShader::SetUniformFloat(const std::string& uniform, const Vector2& v) {
	glUniform2fv(GetUniformLocation(uniform), 1, (GLfloat*)v.array);
}

void OGLShader::SetUniformFloat(const std::string& uniform, const Vector3& v) {
	glUniform3fv(GetUniformLocation(uniform), 1, (GLfloat*)v.array);
}

void OGLShader::SetUniformFloat(const std::string& uniform, const Vector4& v) {
	glUniform4fv(GetUniformLocation(uniform), 1, (GLfloat*)v.array);
}

void OGLShader::SetUniformFloat(const std::string& uniform, const std::vector<Vector2>& v) {
	glUniform2fv(GetUniformLocation(uniform), v.size(), (GLfloat*)v[0].array);
}

void OGLShader::SetUniformFloat(const std::string& uniform, const std::vector<Vector3>& v) {
	glUniform3fv(GetUniformLocation(uniform), v.size(), (GLfloat*)v[0].array);
}

void OGLShader::SetUniformFloat(const std::string& uniform, const std::vector<Vector4>& v) {
	glUniform4fv(GetUniformLocation(uniform), v.size(), (GLfloat*)v[0].array);
}

void OGLShader::SetUniformInt(const std::string& uniform, int v1) {
	glUniform1i(GetUniformLocation(uniform), v1);
}

void OGLShader::SetUniformInt(const std::string& uniform, int v1, int v2) {
	glUniform2i(GetUniformLocation(uniform), v1, v2);
}

void OGLShader::SetUniformInt(const std::string& uniform, int v1, int v2, int v3) {
	glUniform3i(GetUniformLocation(uniform), v1, v2, v3);
}

void OGLShader::SetUniformInt(const std::string& uniform, int v1, int v2, int v3, int v4) {
	glUniform4i(GetUniformLocation(uniform), v1, v2, v3, v4);
}

void OGLShader::SetUniformMatrix(const std::string& uniform, const Matrix2& m) {
	glUniformMatrix2fv(GetUniformLocation(uniform), 1, GL_FALSE, (GLfloat*)m.array);
}

void OGLShader::SetUniformMatrix(const std::string& uniform, const Matrix3& m) {
	glUniformMatrix3fv(GetUniformLocation(uniform), 1, GL_FALSE, (GLfloat*)m.array);
}

void OGLShader::SetUniformMatrix(const std::string& uniform, const Matrix4& m) {
	glUniformMatrix4fv(GetUniformLocation(uniform), 1, GL_FALSE, (GLfloat*)m.array);
}

void OGLShader::SetUniformMatrix(const std::string& uniform, const std::vector<Matrix2>& m) {
	glUniformMatrix2fv(GetUniformLocation(uniform), m.size(), GL_FALSE, (GLfloat*)m[0].array);
}

void OGLShader::SetUniformMatrix(const std::string& uniform, const std::vector<Matrix3>& m) {
	glUniformMatrix3fv(GetUniformLocation(uniform), m.size(), GL_FALSE, (GLfloat*)m[0].array);
}

void OGLShader::SetUniformMatrix(const std::string& uniform, const std::vector<Matrix4>& m) {
	glUniformMatrix4fv(GetUniformLocation(uniform), m.size(), GL_FALSE, (GLfloat*)m[0].array);
}

int OGLShader::GetUniformLocation(const std::string& uniform) {
	auto u = uniformMap.find(uniform);
	GLint location = u == uniformMap.end() ?
		uniformMap.emplace(uniform, glGetUniformLocation(GetProgramID(), uniform.c_str())).first->second :
		u->second;
	return location;
}

void OGLShader::LoadPass(const GLchar* code, ShaderStage type) {
	size_t index = (size_t)type;
	GLuint shaderID = glCreateShader(shaderTypes[index]);

	std::cout << "Reading " << ShaderNames[index] << " shader " << shaderFiles[index] << "\n";

	glShaderSource(shaderID, 1, &code, nullptr);
	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderValid[index]);
	PrintCompileLog(shaderID);
	if (shaderValid[index] != GL_TRUE) {
		std::cout << ShaderNames[index] << " shader " << " has failed!" << "\n";
		return;
	}
	glAttachShader(programID, shaderID);

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

std::unique_ptr<ShaderBase> OGLShader::CreateShader(const std::string& vertex, const std::string& fragment) {
	return std::make_unique<OGLShader>(vertex, fragment);
}

std::unique_ptr<ShaderBase> OGLShader::CreateShaderAndInit(const std::string& vertex, const std::string& fragment) {
	std::unique_ptr<ShaderBase> shader = std::make_unique<OGLShader>(vertex, fragment);
	(static_cast<OGLShader*>(shader.get()))->Initialize();
	return shader;
}

void OGLShader::Initialize() {
	Clear();
	ReloadShader();
}

void OGLShader::PrintCompileLog(GLuint object) {
	GLint logLength = 0;
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength == 0) {
		return;
	}

	std::string infoLog((size_t)logLength, 0);
	glGetShaderInfoLog(object, logLength, nullptr, infoLog.data());
	std::cout << "Compile Log:\n" << infoLog << "\n";
}

void OGLShader::PrintLinkLog(GLuint program) {
	GLint logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength == 0) {
		return;
	}

	std::string infoLog((size_t)logLength, 0);
	glGetProgramInfoLog(program, logLength, nullptr, infoLog.data());
	std::cout << "Link Log:\n" << infoLog << "\n";
}
