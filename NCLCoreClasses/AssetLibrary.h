/**
 * @file   AssetLibrary.h
 * @brief  Static utility class for global access of assets.
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
	/**
	 * @brief Static utility class for global access of assets.
	 * @brief See AssetLibrary.cpp for template declarations.
	 */
	template<typename T>
	class AssetLibrary {
	public:
		typedef std::function<void(T&)> asset_operation_t;
		/**
		 * @param name  Unique location id to store asset in.
		 * @param asset Asset to store in the library.
		 */
		static void AddAsset(const std::string& name, std::shared_ptr<T> asset);
		/**
		 * @brief Get asset stored in location name.
		 * @brief Will cause undefined behaviour/access violations if name
		 * does not exist.
		 * 
		 * @param name  Unique location id to store asset in.
		 */
		static std::shared_ptr<T> GetAsset(const std::string& name);
		/**
		 * @param name  Unique location id to store asset in.
		 * @return true if an asset exists in location name, otherwise false.
		 */
		static bool HasAsset(const std::string& name);
		/**
		 * @brief Execute func on every asset in this library.
		 */
		static void RunOnAssets(asset_operation_t func);
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
	inline void AssetLibrary<T>::RunOnAssets(asset_operation_t func) {
		for (std::pair<const std::string, std::shared_ptr<T>>& it : assets) {
			func(*it.second);
		}
	}
}
