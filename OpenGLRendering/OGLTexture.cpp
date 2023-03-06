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

#include "AssetLoader.h"

using namespace NCL::Rendering;

OGLTexture::OGLTexture(TextureType type, unsigned int width, unsigned int height) : TextureBase(type) {
	glGenTextures(1, &texID);
	switch (type) {
		case TextureType::ColourR8      : pixComponents = GL_R8               ; break;
		case TextureType::ColourR16     : pixComponents = GL_R16              ; break;
		case TextureType::ColourRGB8    : pixComponents = GL_RGB8             ; break;
		case TextureType::ColourRGBA8   : pixComponents = GL_RGBA8            ; break;
		case TextureType::ColourRGBF    : pixComponents = GL_R11F_G11F_B10F   ; break;
		case TextureType::ColourRGB16   : pixComponents = GL_RGB16            ; break;
		case TextureType::ColourRGB16F  : pixComponents = GL_RGB16F           ; break;
		case TextureType::ColourRGBA16F : pixComponents = GL_RGBA16F          ; break;
		case TextureType::ColourRGB32F  : pixComponents = GL_RGB32F           ; break;
		case TextureType::ColourRGBA32F : pixComponents = GL_RGBA32F          ; break;
		case TextureType::Depth        : pixComponents = GL_DEPTH_COMPONENT24; break;
		case TextureType::Stencil      : pixComponents = GL_RGBA             ; break;
		case TextureType::Shadow       : pixComponents = GL_DEPTH_COMPONENT  ; break;
	}
	Bind();
	if (type == TextureType::Shadow) {
		SetFilters(MinFilter::Linear, MagFilter::Linear);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	} else {
		SetFilters(MinFilter::Nearest, MagFilter::Nearest);
	}
	SetEdgeWrap(EdgeWrap::ClampToEdge);
	Resize(width, height);
	Unbind();
}

OGLTexture::~OGLTexture() {
	glDeleteTextures(1, &texID);
}

void OGLTexture::Resize(unsigned int width, unsigned int height) {
	this->width  = width;
	this->height = height;
	glTexImage2D(GL_TEXTURE_2D, 0, pixComponents, width, height, 0, 0, 0, nullptr);
}

void OGLTexture::Upload(void* data, PixelDataFormat format, PixelDataType type) {
	GLenum glformat, gltype;
	switch (format) {
		case PixelDataFormat::Red: glformat = GL_RED; break;
		case PixelDataFormat::RG: glformat = GL_RG; break;
		case PixelDataFormat::RGB: glformat = GL_RGB; break;
		case PixelDataFormat::RGBA: glformat = GL_RGBA; break;
		case PixelDataFormat::DepthComponent: glformat = GL_DEPTH_COMPONENT; break;
		case PixelDataFormat::DepthStencil: glformat = GL_DEPTH_STENCIL; break;
	}
	switch (type) {
		case PixelDataType::UnsignedByte: gltype = GL_UNSIGNED_BYTE; break;
		case PixelDataType::Byte: gltype = GL_BYTE; break;
		case PixelDataType::UnsignedShort: gltype = GL_UNSIGNED_SHORT; break;
		case PixelDataType::Short: gltype = GL_SHORT; break;
		case PixelDataType::UnsignedInt: gltype = GL_UNSIGNED_INT; break;
		case PixelDataType::Int: gltype = GL_INT; break;
		case PixelDataType::HalfFloat: gltype = GL_HALF_FLOAT; break;
		case PixelDataType::Float: gltype = GL_FLOAT; break;
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, glformat, gltype, data);
}

void OGLTexture::Bind() const {
	glBindTexture(GL_TEXTURE_2D, texID);
}

void OGLTexture::Bind(int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	Bind();
}

void OGLTexture::Bind(int slot, int uniform) const {
	Bind(slot);
	glUniform1i(uniform, slot);
}

void OGLTexture::Unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OGLTexture::SetFilters(MinFilter minFilter, MagFilter magFilter) {
	GLfloat glminFilter, glmagFilter;
	switch (minFilter) {
		default:
		case MinFilter::Nearest             : glminFilter = GL_NEAREST               ; break;
		case MinFilter::Linear              : glminFilter = GL_LINEAR                ; break;
		case MinFilter::NearestMipmapNearest: glminFilter = GL_NEAREST_MIPMAP_NEAREST; break;
		case MinFilter::LinearMipmapNearest : glminFilter = GL_LINEAR_MIPMAP_NEAREST ; break;
		case MinFilter::NearestMipmapLinear : glminFilter = GL_NEAREST_MIPMAP_LINEAR ; break;
		case MinFilter::LinearMipmapLinear  : glminFilter = GL_LINEAR_MIPMAP_LINEAR  ; break;
	}
	switch (magFilter) {
		default:
		case MagFilter::Nearest : glmagFilter = GL_NEAREST; break;
		case MagFilter::Linear  : glmagFilter = GL_LINEAR; break;
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glmagFilter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glminFilter);
}

void OGLTexture::SetEdgeWrap(EdgeWrap edgeWrap) {
	GLfloat gledgeWrap;
	switch (edgeWrap) {
		default:
		case EdgeWrap::Repeat         : gledgeWrap = GL_REPEAT         ; break;
		case EdgeWrap::MirroredRepeat : gledgeWrap = GL_MIRRORED_REPEAT; break;
		case EdgeWrap::ClampToEdge    : gledgeWrap = GL_CLAMP_TO_EDGE  ; break;
		case EdgeWrap::ClampToBorder  : gledgeWrap = GL_CLAMP_TO_BORDER; break;
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gledgeWrap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gledgeWrap);
}

std::unique_ptr<TextureBase> OGLTexture::LoadTexture(const std::string& filename) {
	char* texData = nullptr;

	unsigned int width = 0;
	unsigned int height = 0;

	int channels = 0;
	int flags = 0;

	if (!AssetLoader::LoadTextureData(filename, texData, width, height, channels, flags)) {
		return nullptr;
	}

	std::unique_ptr<TextureBase> glTex = CreateTextureFromData(texData, width, height, channels);

	delete[] texData;

	return glTex;
}

std::unique_ptr<TextureBase> OGLTexture::CreateTexture(TextureType type, unsigned int width, unsigned int height) {
	return std::make_unique<OGLTexture>(type, width, height);
}

std::unique_ptr<TextureBase> OGLTexture::CreateTextureFromData(char* data, unsigned int width, unsigned int height, int channels) {
	PixelDataFormat format;
	switch (channels) {
		case 1: format = PixelDataFormat::Red ; break;
		case 2: format = PixelDataFormat::RG  ; break;
		case 3: format = PixelDataFormat::RGB ; break;
		default:
		case 4: format = PixelDataFormat::RGBA; break;
	}

	std::unique_ptr<TextureBase> tex = std::make_unique<OGLTexture>(TextureType::ColourRGBA32F, width, height);
	tex->Bind();

	tex->Upload(data, format, PixelDataType::UnsignedByte);

	tex->Unbind();

	return tex;
}
