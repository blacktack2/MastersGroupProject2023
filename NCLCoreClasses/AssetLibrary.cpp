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

void AssetLibrary::AddMesh(std::string name, MeshGeometry* mesh) {
	meshes.emplace(name, mesh);
}

MeshGeometry* AssetLibrary::GetMesh(std::string name) {
	return meshes.find(name)->second;
}

void AssetLibrary::AddTexture(std::string name, TextureBase* texture) {
	textures.emplace(name, texture);
}

TextureBase* AssetLibrary::GetTexture(std::string name) {
	return textures.find(name)->second;
}

void AssetLibrary::AddShader(std::string name, ShaderBase* shader) {
	shaders.emplace(name, shader);
}

ShaderBase* AssetLibrary::GetShader(std::string name) {
	return shaders.find(name)->second;
}

void AssetLibrary::AddAnimation(std::string name, MeshAnimation* anim) {
	animations.emplace(name, anim);
}

MeshAnimation* AssetLibrary::GetAnimation(std::string name) {
	return animations.find(name)->second;
}

void AssetLibrary::AddMaterial(std::string name, MeshMaterial* material) {
	materials.emplace(name, material);
}

MeshMaterial* AssetLibrary::GetMaterial(std::string name) {
	return materials.find(name)->second;
}
