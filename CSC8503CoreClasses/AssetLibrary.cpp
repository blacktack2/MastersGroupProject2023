#include "AssetLibrary.h"

#include "MeshGeometry.h"
#include "PhysicsObject.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace CSC8503;
using namespace Rendering;

std::map<std::string, GameObject*> AssetLibrary::prefabs{};
std::map<std::string, MeshGeometry*> AssetLibrary::meshes{};
std::map<std::string, TextureBase*> AssetLibrary::textures{};
std::map<std::string, ShaderBase*> AssetLibrary::shaders{};

void AssetLibrary::AddPrefab(std::string name, GameObject* prefab) {
	prefabs.emplace(name, prefab);
}

GameObject* AssetLibrary::GetPrefab(std::string name) {
	return prefabs.find(name)->second;
}

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