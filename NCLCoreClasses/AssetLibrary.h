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
		static void AddMesh(const std::string& name, std::shared_ptr<MeshGeometry> mesh);
		static std::shared_ptr<MeshGeometry> GetMesh(const std::string& name);
		static bool HasMesh(const std::string& name);

		static void AddTexture(const std::string& name, std::shared_ptr<TextureBase> texture);
		static std::shared_ptr<TextureBase> GetTexture(const std::string& name);
		static bool HasTexture(const std::string& name);

		static void AddShader(const std::string& name, std::shared_ptr<ShaderBase> shader);
		static std::shared_ptr<ShaderBase> GetShader(const std::string& name);
		static bool HasShader(const std::string& name);

		static void AddAnimation(const std::string& name, std::shared_ptr<MeshAnimation> anim);
		static std::shared_ptr<MeshAnimation> GetAnimation(const std::string& name);
		static bool HasAnimation(const std::string& name);

		static void AddMaterial(const std::string& name, std::shared_ptr<MeshMaterial> material);
		static std::shared_ptr<MeshMaterial> GetMaterial(const std::string& name);
		static bool HasMaterial(const std::string& name);
	private:
		static std::unordered_map<std::string, std::shared_ptr<MeshGeometry>>  meshes;
		static std::unordered_map<std::string, std::shared_ptr<TextureBase>>   textures;
		static std::unordered_map<std::string, std::shared_ptr<ShaderBase>>    shaders;
		static std::unordered_map<std::string, std::shared_ptr<MeshAnimation>> animations;
		static std::unordered_map<std::string, std::shared_ptr<MeshMaterial>>  materials;
	};
}
