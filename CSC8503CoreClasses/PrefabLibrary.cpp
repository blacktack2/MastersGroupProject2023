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

std::map<std::string, GameObject*> PrefabLibrary::prefabs{};

void PrefabLibrary::AddPrefab(std::string name, GameObject* prefab) {
	prefabs.emplace(name, prefab);
}

GameObject* PrefabLibrary::GetPrefab(std::string name) {
	return prefabs.find(name)->second;
}

bool PrefabLibrary::HasPrefab(std::string name) {
	return prefabs.find(name) != prefabs.end();
}
