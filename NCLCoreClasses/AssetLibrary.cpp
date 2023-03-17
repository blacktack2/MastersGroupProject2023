/**
 * @file   AssetLibrary.cpp
 * @brief  See AssetLibrary.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "AssetLibrary.h"

#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "ShaderBase.h"
#include "TextureBase.h"
#include "MeshAnimation.h"

using namespace NCL;
using namespace Rendering;

template<> std::unordered_map<std::string, std::shared_ptr<MeshGeometry>>  AssetLibrary<MeshGeometry>::assets;
template<> std::unordered_map<std::string, std::shared_ptr<TextureBase>>   AssetLibrary<TextureBase>::assets;
template<> std::unordered_map<std::string, std::shared_ptr<ShaderBase>>    AssetLibrary<ShaderBase>::assets;
template<> std::unordered_map<std::string, std::shared_ptr<MeshAnimation>> AssetLibrary<MeshAnimation>::assets;
template<> std::unordered_map<std::string, std::shared_ptr<MeshMaterial>>  AssetLibrary<MeshMaterial>::assets;
