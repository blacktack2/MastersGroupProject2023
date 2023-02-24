#pragma once
#include <map>
#include <string>

namespace NCL {
	class MeshGeometry;
	class MeshAnimation;
	class MeshMaterial;
	namespace Rendering {
		class ShaderBase;
		class TextureBase;
	}
	using namespace Rendering;
	namespace CSC8503 {
		class GameObject;
		class AssetLibrary {
		public:
			static void AddPrefab(std::string name, GameObject* prefab);
			static GameObject* GetPrefab(std::string name);

			static void AddMesh(std::string name, MeshGeometry* mesh);
			static MeshGeometry* GetMesh(std::string name);

			static void AddTexture(std::string name, TextureBase* texture);
			static TextureBase* GetTexture(std::string name);

			static void AddShader(std::string name, ShaderBase* shader);
			static ShaderBase* GetShader(std::string name);

			static void AddAnimation(std::string name, MeshAnimation* anim);
			static MeshAnimation* GetAnimation(std::string name);
			///*
			static void AddMeshMaterial(std::string name, NCL::MeshMaterial* anim);
			static NCL::MeshMaterial* GetMeshMaterial(std::string name);
			//*/
		private:
			static std::map<std::string, GameObject*> prefabs;
			static std::map<std::string, MeshGeometry*> meshes;
			static std::map<std::string, TextureBase*> textures;
			static std::map<std::string, ShaderBase*> shaders;
			static std::map<std::string, MeshAnimation*> animations;
			static std::map<std::string, NCL::MeshMaterial*> meshMaterials;
		};
	}
}
