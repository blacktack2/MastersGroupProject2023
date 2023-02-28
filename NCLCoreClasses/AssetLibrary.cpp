/**
 * @file   AssetLibrary.cpp
 * @brief  See AssetLibrary.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "AssetLibrary.h"

#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "ShaderBase.h"
#include "TextureBase.h"
#include "MeshAnimation.h"

using namespace NCL;
using namespace Rendering;

std::map<std::string, MeshGeometry*>  AssetLibrary::meshes{};
std::map<std::string, TextureBase*>   AssetLibrary::textures{};
std::map<std::string, ShaderBase*>    AssetLibrary::shaders{};
std::map<std::string, MeshAnimation*> AssetLibrary::animations{};
std::map<std::string, MeshMaterial*>  AssetLibrary::materials{};

void AssetLibrary::AddMesh(const std::string& name, MeshGeometry* mesh) {
	meshes.emplace(name, mesh);
}

MeshGeometry* AssetLibrary::GetMesh(const std::string& name) {
	return meshes.find(name)->second;
}

bool AssetLibrary::HasMesh(const std::string& name) {
	return meshes.find(name) != meshes.end();
}

void AssetLibrary::AddTexture(const std::string& name, TextureBase* texture) {
	textures.emplace(name, texture);
}

TextureBase* AssetLibrary::GetTexture(const std::string& name) {
	return textures.find(name)->second;
}

bool AssetLibrary::HasTexture(const std::string& name) {
	return textures.find(name) != textures.end();
}

void AssetLibrary::AddShader(const std::string& name, ShaderBase* shader) {
	shaders.emplace(name, shader);
}

ShaderBase* AssetLibrary::GetShader(const std::string& name) {
	return shaders.find(name)->second;
}

bool AssetLibrary::HasShader(const std::string& name) {
	return shaders.find(name) != shaders.end();
}

void AssetLibrary::AddAnimation(const std::string& name, MeshAnimation* anim) {
	animations.emplace(name, anim);
}

MeshAnimation* AssetLibrary::GetAnimation(const std::string& name) {
	return animations.find(name)->second;
}

bool AssetLibrary::HasAnimation(const std::string& name) {
	return animations.find(name) != animations.end();
}

void AssetLibrary::AddMaterial(const std::string& name, MeshMaterial* material) {
	materials.emplace(name, material);
}

MeshMaterial* AssetLibrary::GetMaterial(const std::string& name) {
	return materials.find(name)->second;
}

bool AssetLibrary::HasMaterial(const std::string& name) {
	return materials.find(name) != materials.end();
}
