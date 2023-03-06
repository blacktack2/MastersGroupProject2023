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

void AssetLibrary::AddMesh(const std::string& name, std::shared_ptr<MeshGeometry> mesh) {
	meshes.emplace(name, std::move(mesh));
}

std::shared_ptr<MeshGeometry> AssetLibrary::GetMesh(const std::string& name) {
	return meshes.find(name)->second;
}

bool AssetLibrary::HasMesh(const std::string& name) {
	return meshes.find(name) != meshes.end();
}

void AssetLibrary::AddTexture(const std::string& name, std::shared_ptr<TextureBase> texture) {
	textures.emplace(name, std::move(texture));
}

std::shared_ptr<TextureBase> AssetLibrary::GetTexture(const std::string& name) {
	return textures.find(name)->second;
}

bool AssetLibrary::HasTexture(const std::string& name) {
	return textures.find(name) != textures.end();
}

void AssetLibrary::AddShader(const std::string& name, std::shared_ptr<ShaderBase> shader) {
	shaders.emplace(name, std::move(shader));
}

std::shared_ptr<ShaderBase> AssetLibrary::GetShader(const std::string& name) {
	return shaders.find(name)->second;
}

bool AssetLibrary::HasShader(const std::string& name) {
	return shaders.find(name) != shaders.end();
}

void AssetLibrary::AddAnimation(const std::string& name, std::shared_ptr<MeshAnimation> anim) {
	animations.emplace(name, std::move(anim));
}

std::shared_ptr<MeshAnimation> AssetLibrary::GetAnimation(const std::string& name) {
	return animations.find(name)->second;
}

bool AssetLibrary::HasAnimation(const std::string& name) {
	return animations.find(name) != animations.end();
}

void AssetLibrary::AddMaterial(const std::string& name, std::shared_ptr<MeshMaterial> material) {
	materials.emplace(name, std::move(material));
}

std::shared_ptr<MeshMaterial> AssetLibrary::GetMaterial(const std::string& name) {
	return materials.find(name)->second;
}

bool AssetLibrary::HasMaterial(const std::string& name) {
	return materials.find(name) != materials.end();
}
