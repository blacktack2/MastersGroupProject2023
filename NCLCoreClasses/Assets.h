/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#ifdef x64
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
#endif
#ifdef _ORBIS

#include <string>
namespace NCL {
	namespace Assets {
		const std::string SHADERDIR("/app0/Assets/Shaders/");
		const std::string MESHDIR("/app0/Assets/Meshes/");
		const std::string TEXTUREDIR("/app0/Assets/Textures/");
		const std::string SOUNDSDIR("/app0/Assets/Sounds/");
		const std::string FONTSSDIR("/app0/Assets/Fonts/");
		const std::string DATADIR("/app0/Assets/Data/");
		extern bool ReadTextFile(const std::string& filepath, std::string& result);
		extern bool ReadBinaryFile(const std::string& filepath, char** into, size_t& size);
	}
}
#endif