/**
 * @file   Assets.h
 * @brief  Utility file for general asset related constants and functionality.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#include <string>

namespace NCL{
	namespace Assets {
		const std::string ASSETROOT(ASSETROOTLOCATION);
		const std::string SHADERDIR(ASSETROOT + "Shaders/");
		const std::string MESHDIR(ASSETROOT + "Meshes/");
		const std::string TEXTUREDIR(ASSETROOT + "Textures/");
		const std::string SOUNDSDIR(ASSETROOT + "Sounds/");
		const std::string FONTSSDIR(ASSETROOT + "Fonts/");
		const std::string DATADIR(ASSETROOT + "Data/");

		extern bool ReadTextFile(const std::string& filepath, std::string& result);
		extern bool ReadBinaryFile(const std::string& filepath, char** into, size_t& size);
	}
}
