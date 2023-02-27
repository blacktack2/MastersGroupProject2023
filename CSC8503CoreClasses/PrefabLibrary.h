/**
 * @file   PrefabLibrary.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL::CSC8503 {
	class GameObject;
}
using namespace NCL::CSC8503;

namespace NCL {
	class PrefabLibrary {
	public:
		static void AddPrefab(std::string name, GameObject* prefab);
		static GameObject* GetPrefab(std::string name);
	private:
		static std::map<std::string, GameObject*>    prefabs;
	};
}
