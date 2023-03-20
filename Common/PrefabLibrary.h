/**
 * @file   PrefabLibrary.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include<string>
#include <memory>
#include<unordered_map>

namespace NCL::CSC8503 {
	class GameObject;
}
using namespace NCL::CSC8503;

namespace NCL {
	class PrefabLibrary {
	public:
		static void AddPrefab(std::string name, std::unique_ptr<GameObject> prefab);
		static GameObject& GetPrefab(std::string name);
		static bool HasPrefab(std::string name);
	private:
		static std::unordered_map<std::string, std::unique_ptr<GameObject>> prefabs;
	};
}
