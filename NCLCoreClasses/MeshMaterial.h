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

namespace NCL {
	namespace Rendering {
		class ShaderBase;
		class TextureBase;
	}
}

using namespace NCL::Rendering;

namespace NCL {
	class MeshMaterialEntry {
	public:
		MeshMaterialEntry() {}

		inline void AddTexture(const std::string& name, std::shared_ptr<TextureBase> texture) {
			textures.emplace(std::make_pair(name, texture));
		}
		inline TextureBase* GetTexture(const std::string& name) const {
			auto i = textures.find(name);
			return i == textures.end() ? nullptr : i->second.get();
		}
		inline void SetShader(std::shared_ptr<ShaderBase> shader) {
			this->shader = shader;
		}
		inline ShaderBase* GetShader() const {
			return shader.get();
		}
		inline void SetShadowShader(std::shared_ptr<ShaderBase> shader) {
			shadowShader = shader;
		}
		inline ShaderBase* GetShadowShader() const {
			return shadowShader.get();
		}
	protected:
		std::unordered_map<std::string, std::shared_ptr<TextureBase>> textures{};
		std::shared_ptr<ShaderBase> shader = nullptr;
		std::shared_ptr<ShaderBase> shadowShader = nullptr;
	};

	class MeshMaterial {
	public:
		MeshMaterial(const std::string& filename);
		MeshMaterial(const std::vector<std::pair<std::string, std::shared_ptr<TextureBase>>>& textures, std::shared_ptr<ShaderBase> shader);
		~MeshMaterial() {}

		const MeshMaterialEntry* GetMaterialForLayer(int i) const;
	protected:
		std::vector<MeshMaterialEntry> materialLayers;
		std::vector<std::reference_wrapper<MeshMaterialEntry>> meshLayers;
	};
}
