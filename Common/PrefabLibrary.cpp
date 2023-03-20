/**
 * @file   PrefabLibrary.cpp
 * @brief  See PrefabLibrary.h.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "PrefabLibrary.h"

#include "GameObject.h"

using namespace NCL;

std::unordered_map<std::string, std::unique_ptr<GameObject>> PrefabLibrary::prefabs{};

void PrefabLibrary::AddPrefab(std::string name, std::unique_ptr<GameObject> prefab) {
	prefabs.emplace(name, std::move(prefab));
}

GameObject& PrefabLibrary::GetPrefab(std::string name) {
	return *prefabs.find(name)->second;
}

bool PrefabLibrary::HasPrefab(std::string name) {
	return prefabs.find(name) != prefabs.end();
}
