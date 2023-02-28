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
		static void AddMesh(std::string name, MeshGeometry* mesh);
		static MeshGeometry* GetMesh(std::string name);

		static void AddTexture(std::string name, TextureBase* texture);
		static TextureBase* GetTexture(std::string name);

		static void AddShader(std::string name, ShaderBase* shader);
		static ShaderBase* GetShader(std::string name);

		static void AddAnimation(std::string name, MeshAnimation* anim);
		static MeshAnimation* GetAnimation(std::string name);

		static void AddMaterial(std::string name, MeshMaterial* material);
		static MeshMaterial* GetMaterial(std::string name);
	private:
		static std::map<std::string, MeshGeometry*>  meshes;
		static std::map<std::string, TextureBase*>   textures;
		static std::map<std::string, ShaderBase*>    shaders;
		static std::map<std::string, MeshAnimation*> animations;
		static std::map<std::string, MeshMaterial*>  materials;
	};
}
