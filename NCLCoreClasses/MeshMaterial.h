/**
 * @file   MeshMaterial.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include <string>
#include <vector>
#include <map>

namespace NCL::Rendering {
	class ShaderBase;
	class TextureBase;
}
using namespace NCL::Rendering;
namespace NCL {
	class MeshMaterialEntry {
		friend class MeshMaterial;
	public:
		inline TextureBase* GetTexture(const std::string& name) const {
			auto i = textures.find(name);
			return i == textures.end() ? nullptr : i->second;
		}
		inline ShaderBase* GetShader() const {
			return shader;
		}
	protected:
		std::map<std::string, TextureBase*> textures;
		ShaderBase* shader;
	};

	class MeshMaterial {
	public:
		MeshMaterial(const std::string& filename);
		MeshMaterial(const std::vector<std::pair<std::string, TextureBase*>>& textures, ShaderBase* shader);
		~MeshMaterial() {}

		const MeshMaterialEntry* GetMaterialForLayer(int i) const;
	protected:
		std::vector<MeshMaterialEntry>  materialLayers;
		std::vector<MeshMaterialEntry*> meshLayers;
	};
}
