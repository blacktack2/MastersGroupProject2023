/**
 * @file   MeshMaterial.cpp
 * @brief  See MeshMaterial.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "MeshMaterial.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"
#include "Assets.h"

#include "ShaderBase.h"
#include "TextureBase.h"

#include <fstream>
#include <iostream>

using namespace NCL;
using namespace NCL::Rendering;

MeshMaterial::MeshMaterial(const std::string& filename) {
	std::ifstream file(Assets::MESHDIR + filename);

	std::string dataType;
	file >> dataType;

	if (dataType != "MeshMat") {
		std::cout << __FUNCTION__ << " File " << filename << " is not a MeshMaterial!\n";
		return;
	}
	int version;
	file >> version;

	if (version != 1) {
		std::cout << __FUNCTION__ << " File " << filename << " has incompatible version " << version << "!\n";
		return;
	}

	int matCount;
	int meshCount;
	file >> matCount;
	file >> meshCount;

	materialLayers.resize(matCount);

	for (int i = 0; i < matCount; ++i) {
		std::string shader;
		int count;
		file >> shader;
		file >> count;

		materialLayers[i].SetShader(AssetLibrary<ShaderBase>::HasAsset(shader) ? AssetLibrary<ShaderBase>::GetAsset(shader) : nullptr);
		for (int j = 0; j < count; ++j) {
			std::string entryData;
			file >> entryData;
			std::string channel, texture;

			size_t split = entryData.find_first_of(':');
			channel = entryData.substr(0, split);
			texture = entryData.substr(split + 1);

			if (!AssetLibrary<TextureBase>::HasAsset(texture)) {
				std::shared_ptr<TextureBase> tex = AssetLoader::LoadTexture(texture);
				if (tex) {
					AssetLibrary<TextureBase>::AddAsset(texture, std::move(tex));
					
					materialLayers[i].AddTexture(channel, AssetLibrary<TextureBase>::GetAsset(texture));
				}
			} else {
				materialLayers[i].AddTexture(channel, AssetLibrary<TextureBase>::GetAsset(texture));
			}
		}
	}

	for (int i = 0; i < meshCount; ++i) {
		int entry;
		file >> entry;
		meshLayers.emplace_back(materialLayers[entry]);
	}
}

MeshMaterial::MeshMaterial(const std::vector<std::pair<std::string, std::shared_ptr<TextureBase>>>& textures, std::shared_ptr<ShaderBase> shader) {
	materialLayers.resize(1);
	materialLayers[0].SetShader(shader);
	for (const auto& texPair : textures) {
		materialLayers[0].AddTexture(texPair.first, texPair.second);
	}
}

const MeshMaterialEntry* MeshMaterial::GetMaterialForLayer(int i) const {
	return (i < 0 || i >= meshLayers.size()) ? nullptr : &meshLayers[i].get();
}
