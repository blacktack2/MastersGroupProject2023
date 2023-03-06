/**
 * @file   SimpleFont.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <string>
#include <vector>

using namespace NCL::Maths;

namespace NCL::Rendering {
	class TextureBase;

	class SimpleFont {
	public:
		SimpleFont(const std::string& fontName, const std::string& texName);
		~SimpleFont();

		struct InterleavedTextVertex {
			Vector2 pos;
			Vector2 texCoord;
			Vector4 colour;
		};

		int GetVertexCountForString(const std::string& text);
		void BuildVerticesForString(const std::string& text, const Vector2& startPos, const Vector4& colour, float size, std::vector<Vector3>&positions, std::vector<Vector2>&texCoords, std::vector<Vector4>&colours);
		void BuildInterleavedVerticesForString(const std::string& text, const Vector2& startPos, const Vector4& colour, float size, std::vector<InterleavedTextVertex>&vertices);

		inline const TextureBase& GetTexture() const {
			return texture;
		}
	protected:
		//matches stbtt_bakedchar
		struct FontChar {
			unsigned short x0;
			unsigned short y0;
			unsigned short x1;
			unsigned short y1;
			float xOff;
			float yOff;
			float xAdvance;
		};

		FontChar*    allCharData;
		TextureBase& texture;

		int startChar;
		int numChars;

		float texWidth;
		float texHeight;
		float texWidthRecip;
		float texHeightRecip;
	};
}
