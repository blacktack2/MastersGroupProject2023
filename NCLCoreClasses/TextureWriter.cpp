/**
 * @file   TextureWriter.cpp
 * @brief  See TextureWriter.h
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#include "TextureWriter.h"

#include "Assets.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb/stb_image_write.h"

using namespace NCL;

void TextureWriter::WritePNG(const std::string& filename, char* data, int width, int height, int channels) {
	stbi_write_png(filename.c_str(), width, height, channels, data, width * channels);
}
