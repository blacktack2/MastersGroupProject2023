/**
 * @file   AssetLoader.cpp
 * @brief  See AssetLoader.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "AssetLoader.h"

#include "Assets.h"

#include "BufferObjectBase.h"
#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb/stb_image.h"

#include <iostream>
#include <filesystem>

using namespace NCL;
using namespace Rendering;

std::map<std::string, TextureDataLoadFunction> AssetLoader::fileHandlers{};

MeshLoadFunction    AssetLoader::meshLoad    = nullptr;
TextureLoadFunction AssetLoader::textureLoad = nullptr;

BufferObjectCreateFunction AssetLoader::bufferObjectCreate = nullptr;
FrameBufferCreateFunction  AssetLoader::frameBufferCreate  = nullptr;
MeshCreateFunction         AssetLoader::meshCreate         = nullptr;
ShaderCreateFunction       AssetLoader::shaderCreate       = nullptr;
ShaderCreateFunction       AssetLoader::shaderCreateAndInit = nullptr;
TextureCreateFunction      AssetLoader::textureCreate      = nullptr;

void AssetLoader::RegisterTextureDataLoadFunction(TextureDataLoadFunction f, const std::string& fileExtension) {
	fileHandlers.insert(std::make_pair(fileExtension, f));
}

std::string AssetLoader::GetFileExtension(const std::string& fileExtension) {
	auto p = std::filesystem::path(fileExtension);
	auto ext = p.extension();
	return ext.string();
}

void AssetLoader::RegisterMeshLoadFunction(MeshLoadFunction f) {
	if (meshLoad) {
		std::cout << __FUNCTION__ << " replacing previously defined Mesh Load Function.\n";
	}
	meshLoad = f;
}

void AssetLoader::RegisterTextureLoadFunction(TextureLoadFunction f) {
	if (textureLoad) {
		std::cout << __FUNCTION__ << " replacing previously defined Texture Load Function.\n";
	}
	textureLoad = f;
}

void AssetLoader::RegisterBufferObjectCreateFunction(BufferObjectCreateFunction f) {
	if (bufferObjectCreate) {
		std::cout << __FUNCTION__ << " replacing previously defined BufferObject Create Function.\n";
	}
	bufferObjectCreate = f;
}

void AssetLoader::RegisterFrameBufferCreateFunction(FrameBufferCreateFunction f) {
	if (frameBufferCreate) {
		std::cout << __FUNCTION__ << " replacing previously defined FrameBuffer Create Function.\n";
	}
	frameBufferCreate = f;
}

void AssetLoader::RegisterMeshCreateFunction(MeshCreateFunction f) {
	if (meshCreate) {
		std::cout << __FUNCTION__ << " replacing previously defined Mesh Create Function.\n";
	}
	meshCreate = f;
}

void AssetLoader::RegisterShaderCreateFunction(ShaderCreateFunction f) {
	if (shaderCreate) {
		std::cout << __FUNCTION__ << " replacing previously defined Shader Create Function.\n";
	}
	shaderCreate = f;
}

void AssetLoader::RegisterShaderCreateAndInitFunction(ShaderCreateFunction f) {
	if (shaderCreateAndInit) {
		std::cout << __FUNCTION__ << " replacing previously defined Shader Create and init Function.\n";
	}
	shaderCreateAndInit = f;
}

void AssetLoader::RegisterTextureCreateFunction(TextureCreateFunction f) {
	if (textureCreate) {
		std::cout << __FUNCTION__ << " replacing previously defined Texture Create Function.\n";
	}
	textureCreate = f;
}

bool AssetLoader::LoadTextureData(const std::string& filename, char*& outData, unsigned int& width, unsigned int& height, int& channels, int& flags) {
	if (filename.empty()) {
		return false;
	}

	std::filesystem::path path(filename);

	std::string extension = path.extension().string();

	bool isAbsolute = path.is_absolute();

	auto it = fileHandlers.find(extension);

	std::string realPath = isAbsolute ? filename : Assets::TEXTUREDIR + filename;

	if (it != fileHandlers.end()) {
		return it->second(realPath, outData, width, height, channels, flags);
	}
	stbi_uc* texData = stbi_load(realPath.c_str(), reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height), &channels, 4);

	channels = 4;

	if (texData) {
		outData = (char*)texData;
		return true;
	}

	return false;
}

std::unique_ptr<MeshGeometry> AssetLoader::LoadMesh(const std::string& filename) {
	if (meshLoad == nullptr) {
		std::cout << __FUNCTION__ << " no Mesh Load Function has been defined!\n";
		return nullptr;
	}
	return meshLoad(filename);
}

std::unique_ptr<TextureBase> AssetLoader::LoadTexture(const std::string& filename) {
	if (textureLoad == nullptr) {
		std::cout << __FUNCTION__ << " no Texture Load Function has been defined!\n";
		return nullptr;
	}
	return textureLoad(filename);
}

std::unique_ptr<BufferObjectBase> AssetLoader::CreateBufferObject(size_t size, unsigned int binding) {
	if (bufferObjectCreate == nullptr) {
		std::cout << __FUNCTION__ << " no BufferObject Create Function has been defined!\n";
		return nullptr;
	}
	return bufferObjectCreate(size, binding);
}

std::unique_ptr<FrameBuffer> AssetLoader::CreateFrameBuffer() {
	if (frameBufferCreate == nullptr) {
		std::cout << __FUNCTION__ << " no FrameBuffer Create Function has been defined!\n";
		return nullptr;
	}
	return frameBufferCreate();
}

std::unique_ptr<MeshGeometry> AssetLoader::CreateMesh() {
	if (meshCreate == nullptr) {
		std::cout << __FUNCTION__ << " no Mesh Create Function has been defined!\n";
		return nullptr;
	}
	return meshCreate();
}

std::unique_ptr<ShaderBase> AssetLoader::CreateShader(const std::string& vertex, const std::string& fragment, const std::string& tessCont, const std::string& tessEval, const std::string& geometry) {
	if (shaderCreate == nullptr) {
		std::cout << __FUNCTION__ << " no Shader Create Function has been defined!\n";
		return nullptr;
	}
	return shaderCreate(vertex, fragment, tessCont, tessEval, geometry);
}

std::unique_ptr<ShaderBase> AssetLoader::CreateShaderAndInit(const std::string& vertex, const std::string& fragment, const std::string& tessCont, const std::string& tessEval, const std::string& geometry) {
	if (shaderCreateAndInit == nullptr) {
		std::cout << __FUNCTION__ << " no Shader Create And Init Function has been defined!\n";
		return nullptr;
	}
	return shaderCreateAndInit(vertex, fragment, tessCont, tessEval, geometry);
}

std::unique_ptr<TextureBase> AssetLoader::CreateTexture(TextureType type, unsigned int width, unsigned int height) {
	if (textureCreate == nullptr) {
		std::cout << __FUNCTION__ << " no Create Function has been defined!\n";
		return nullptr;
	}
	return textureCreate(type, width, height);
}
