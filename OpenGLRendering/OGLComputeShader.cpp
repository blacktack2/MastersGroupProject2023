/**
 * @file   OGLComputeShader.cpp
 * @brief  See OGLComputeShader.h.
 * 
 * @author Rich Davidson
 * @date   February 2023
 */
#include "OGLComputeShader.h"
#include "OGLShader.h"
#include "Assets.h"
#include <iostream>

using namespace NCL;
using namespace Rendering;

OGLComputeShader::OGLComputeShader(const std::string& s) {
	std::string fileContents = "";
	Assets::ReadTextFile(Assets::SHADERDIR + s, fileContents);

	programID = glCreateProgram();
	shaderID  = glCreateShader(GL_COMPUTE_SHADER);

	const char* stringData = fileContents.c_str();
	int stringLength = (int)fileContents.length();
	glShaderSource(shaderID, 1, &stringData, &stringLength);
	glCompileShader(shaderID);
	glAttachShader(programID, shaderID);

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &programValid);

	//std::cout << "Loading compute shader " << s << std::endl;
	if (programValid != GL_TRUE) {
		//std::cout << "Compute shader has failed!" << std::endl;
		threadsInGroup[0] = 0;
		threadsInGroup[1] = 0;
		threadsInGroup[2] = 0;
	}
	else {
		//std::cout << "Compute shader loaded!" << std::endl;
		
		glGetProgramiv(programID, GL_COMPUTE_WORK_GROUP_SIZE, threadsInGroup);
	}
	OGLShader::PrintCompileLog(shaderID);
};

OGLComputeShader::~OGLComputeShader() {
	glDetachShader(programID, shaderID);
	glDeleteShader(shaderID);
	glDeleteProgram(programID);
}

void OGLComputeShader::Execute(int x, int y, int z) const {
	glDispatchCompute(x, y, z);
}

void OGLComputeShader::Bind() const {
	glUseProgram(programID);
}

void OGLComputeShader::Unbind() {
	glUseProgram(0);
}

void OGLComputeShader::GetThreadsInGroup(int&x, int& y, int&z)const {
	x = threadsInGroup[0];
	y = threadsInGroup[1];
	z = threadsInGroup[2];
}

int OGLComputeShader::GetThreadXCount() const {
	return threadsInGroup[0];
}

int OGLComputeShader::GetThreadYCount() const {
	return threadsInGroup[1];
}

int OGLComputeShader::GetThreadZCount() const {
	return threadsInGroup[2];
}
