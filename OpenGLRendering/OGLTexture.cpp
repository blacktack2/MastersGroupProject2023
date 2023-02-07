/**
 * @file   OGLTexture.cpp
 * @brief  See OGLTexture.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLTexture.h"
#include "OGLRenderer.h"

#include "TextureLoader.h"

using namespace NCL::Rendering;

OGLTexture::OGLTexture(GLsizei width, GLsizei height, TexType texType) : texType(texType) {
	glGenTextures(1, &texID);
	Bind();
	SetEdgeClamp();
	switch (texType) {
		default:
		case TexType::Colour8  : InitColour8() ; break;
		case TexType::Colour32 : InitColour32(); break;
		case TexType::Depth    : InitDepth()   ; break;
		case TexType::Stencil  : InitStencil() ; break;
		case TexType::Shadow   : InitShadow()  ; break;
	}
	Resize(width, height);
	Unbind();
}

OGLTexture::~OGLTexture() {
	glDeleteTextures(1, &texID);
}

void OGLTexture::Resize(GLsizei width, GLsizei height) {
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, type, format, nullptr);
}

void OGLTexture::Bind() {
	glBindTexture(GL_TEXTURE_2D, texID);
}

void OGLTexture::Bind(GLint slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	Bind();
}

void OGLTexture::Bind(GLint slot, GLint uniform) {
	Bind(slot);
	glUniform1i(uniform, slot);
}

void OGLTexture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OGLTexture::SetEdgeClamp() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void OGLTexture::SetEdgeRepeat() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

TextureBase* OGLTexture::RGBATextureFromData(char* data, int width, int height, int channels) {
	OGLTexture* tex = new OGLTexture(width, height);

	int dataSize = width * height * channels; //This always assumes data is 1 byte per channel

	int sourceType = GL_RGB;

	switch (channels) {
		case 1: sourceType = GL_RED ; break;
		case 2: sourceType = GL_RG  ; break;
		case 3: sourceType = GL_RGB ; break;
		default:
		case 4: sourceType = GL_RGBA; break;
	}

	tex->Bind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, sourceType, GL_UNSIGNED_BYTE, data);

	tex->Unbind();

	return tex;
}

TextureBase* OGLTexture::RGBATextureFromFilename(const std::string& filename) {
	char* texData = nullptr;
	int width     = 0;
	int height    = 0;
	int channels  = 0;
	int flags     = 0;
	TextureLoader::LoadTexture(filename, texData, width, height, channels, flags);

	TextureBase* glTex = RGBATextureFromData(texData, width, height, channels);

	delete[] texData;

	return glTex;
}

void OGLTexture::InitColour8() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	internalFormat = GL_RGBA8;
	format = GL_UNSIGNED_BYTE;
	type = GL_RGBA;
}

void OGLTexture::InitColour32() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	internalFormat = GL_RGBA32F;
	format = GL_UNSIGNED_BYTE;
	type = GL_RGBA;

	glGenerateMipmap(GL_TEXTURE_2D);
}

void OGLTexture::InitDepth() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	internalFormat = GL_DEPTH_COMPONENT24;
	format = GL_UNSIGNED_BYTE;
	type = GL_DEPTH_COMPONENT;
}

void OGLTexture::InitStencil() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	internalFormat = GL_DEPTH_STENCIL;
	format = GL_DEPTH_STENCIL;
	type = GL_RGBA;
}

void OGLTexture::InitShadow() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	internalFormat = GL_DEPTH_COMPONENT;
	format = GL_FLOAT;
	type = GL_DEPTH_COMPONENT;
}
