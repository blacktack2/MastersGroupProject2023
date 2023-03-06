/**
 * @file   MeshMaterial.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include <functional>
#include <unordered_map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace NCL::Rendering {
	class ShaderBase;
	class TextureBase;
}

using namespace NCL::Rendering;

namespace NCL {
	class MeshMaterialEntry {
		friend class MeshMaterial;
	public:
		inline std::optional<std::reference_wrapper<TextureBase>> GetTexture(const std::string& name) const {
			auto i = textures.find(name);
			return i == textures.end() ? std::optional<std::reference_wrapper<TextureBase>>() : *i->second;
		}
		inline std::optional<std::reference_wrapper<ShaderBase>> GetShader() const {
			return shader ? *shader : std::optional<std::reference_wrapper<ShaderBase>>();
		}
	protected:
		std::unordered_map<std::string, std::shared_ptr<TextureBase>> textures;
		std::shared_ptr<ShaderBase> shader;
	};

	class MeshMaterial {
	public:
		MeshMaterial(const std::string& filename);
		MeshMaterial(const std::vector<std::pair<std::string, std::shared_ptr<TextureBase>>>& textures, std::shared_ptr<ShaderBase> shader);
		~MeshMaterial() {}

		const std::optional<std::reference_wrapper<MeshMaterialEntry>> GetMaterialForLayer(int i) const;
	protected:
		std::vector<MeshMaterialEntry> materialLayers;
		std::vector<std::reference_wrapper<MeshMaterialEntry>> meshLayers;
	};
}
