/**
 * @file   AssetLibrary.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include <map>
#include <string>

namespace NCL {
	class MeshGeometry;
	class MeshMaterial;
	class MeshAnimation;

	namespace Rendering {
		class ShaderBase;
		class TextureBase;
	}

	using namespace Rendering;

	class AssetLibrary {
	public:
		static void AddMesh(const std::string& name, MeshGeometry* mesh);
		static MeshGeometry* GetMesh(const std::string& name);
		static bool HasMesh(const std::string& name);

		static void AddTexture(const std::string& name, TextureBase* texture);
		static TextureBase* GetTexture(const std::string& name);
		static bool HasTexture(const std::string& name);

		static void AddShader(const std::string& name, ShaderBase* shader);
		static ShaderBase* GetShader(const std::string& name);
		static bool HasShader(const std::string& name);

		static void AddAnimation(const std::string& name, MeshAnimation* anim);
		static MeshAnimation* GetAnimation(const std::string& name);
		static bool HasAnimation(const std::string& name);

		static void AddMaterial(const std::string& name, MeshMaterial* material);
		static MeshMaterial* GetMaterial(const std::string& name);
		static bool HasMaterial(const std::string& name);
	private:
		static std::map<std::string, MeshGeometry*>  meshes;
		static std::map<std::string, TextureBase*>   textures;
		static std::map<std::string, ShaderBase*>    shaders;
		static std::map<std::string, MeshAnimation*> animations;
		static std::map<std::string, MeshMaterial*>  materials;
	};
}
