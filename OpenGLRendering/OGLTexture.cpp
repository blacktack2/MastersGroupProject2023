/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "OGLTexture.h"
#include "OGLRenderer.h"

#include "TextureLoader.h"

using namespace NCL::Rendering;

OGLTexture::OGLTexture(TexType type) : type(type) {
	glGenTextures(1, &texID);
	Bind();
	SetEdgeClamp();
	switch (type) {
		default:
		case TexType::Colour  : InitColour()  ; break;
		case TexType::Depth   : InitDepth()   ; break;
		case TexType::Stencil : InitStencil() ; break;
		case TexType::Shadow  : InitShadow()  ; break;
	}
	Unbind();
}

OGLTexture::~OGLTexture() {
	glDeleteTextures(1, &texID);
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
	OGLTexture* tex = new OGLTexture();

	int dataSize = width * height * channels; //This always assumes data is 1 byte per channel

	int sourceType = GL_RGB;

	switch (channels) {
		case 1: sourceType = GL_RED ; break;

		case 2: sourceType = GL_RG  ; break;
		case 3: sourceType = GL_RGB ; break;
		case 4: sourceType = GL_RGBA; break;
		//default:
	}

	glBindTexture(GL_TEXTURE_2D, tex->texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, sourceType, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

TextureBase* OGLTexture::RGBATextureFromFilename(const std::string&name) {
	char* texData = nullptr;
	int width     = 0;
	int height    = 0;
	int channels  = 0;
	int flags     = 0;
	TextureLoader::LoadTexture(name, texData, width, height, channels, flags);  

	TextureBase* glTex = RGBATextureFromData(texData, width, height, channels);

	free(texData);

	return glTex;
}

void OGLTexture::InitColour() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void OGLTexture::InitDepth() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void OGLTexture::InitStencil() {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void OGLTexture::InitShadow() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
}
