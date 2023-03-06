/**
 * @file   AssetLibrary.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include <unordered_map>
#include <memory>
#include <string>

namespace NCL::Rendering {
	class ShaderBase;
	class TextureBase;
}

using namespace NCL::Rendering;

namespace NCL {
	class MeshGeometry;
	class MeshMaterial;
	class MeshAnimation;

	class AssetLibrary {
	public:
		static AssetLibrary& instance() {
			static AssetLibrary INSTANCE;
			return INSTANCE;
		}

		void AddMesh(const std::string& name, std::shared_ptr<MeshGeometry> mesh);
		std::shared_ptr<MeshGeometry> GetMesh(const std::string& name);
		bool HasMesh(const std::string& name);

		void AddTexture(const std::string& name, std::shared_ptr<TextureBase> texture);
		std::shared_ptr<TextureBase> GetTexture(const std::string& name);
		bool HasTexture(const std::string& name);

		void AddShader(const std::string& name, std::shared_ptr<ShaderBase> shader);
		std::shared_ptr<ShaderBase> GetShader(const std::string& name);
		bool HasShader(const std::string& name);

		void AddAnimation(const std::string& name, std::shared_ptr<MeshAnimation> anim);
		std::shared_ptr<MeshAnimation> GetAnimation(const std::string& name);
		bool HasAnimation(const std::string& name);

		void AddMaterial(const std::string& name, std::shared_ptr<MeshMaterial> material);
		std::shared_ptr<MeshMaterial> GetMaterial(const std::string& name);
		bool HasMaterial(const std::string& name);
	private:
		std::unordered_map<std::string, std::shared_ptr<MeshGeometry>>  meshes;
		std::unordered_map<std::string, std::shared_ptr<TextureBase>>   textures;
		std::unordered_map<std::string, std::shared_ptr<ShaderBase>>    shaders;
		std::unordered_map<std::string, std::shared_ptr<MeshAnimation>> animations;
		std::unordered_map<std::string, std::shared_ptr<MeshMaterial>>  materials;
	};
}
