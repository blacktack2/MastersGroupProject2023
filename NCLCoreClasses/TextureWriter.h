/**
 * @file   TextureWriter.h
 * @brief  Utility class for writing texture data to a file.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#include <string>

namespace NCL {
	/**
	 * @brief Utility class for writing texture data to a file.
	 */
	class TextureWriter	{
	public:
		static void WritePNG(const std::string& filename, char* data, int width, int height, int channels);
	};
}

