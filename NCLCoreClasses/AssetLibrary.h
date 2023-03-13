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
#include <functional>

namespace NCL {
	template<typename T>
	class AssetLibrary {
	public:
		static void AddAsset(const std::string& name, std::shared_ptr<T> asset);
		static std::shared_ptr<T> GetAsset(const std::string& name);
		static bool HasAsset(const std::string& name);
		static void RunOnAssets(std::function<void(T&)> func);
	private:
		static std::unordered_map<std::string, std::shared_ptr<T>> assets;
	};

	template<typename T>
	inline void AssetLibrary<T>::AddAsset(const std::string& name, std::shared_ptr<T> asset) {
		assets.emplace(name, std::move(asset));
	}

	template<typename T>
	inline std::shared_ptr<T> AssetLibrary<T>::GetAsset(const std::string& name) {
		return assets.find(name)->second;
	}

	template<typename T>
	inline bool AssetLibrary<T>::HasAsset(const std::string& name) {
		return assets.contains(name);
	}

	template<typename T>
	inline void AssetLibrary<T>::RunOnAssets(std::function<void(T&)> func) {
		for (std::pair<const std::string, std::shared_ptr<T>>& it : assets) {
			func(*it.second);
		}
	}
}
