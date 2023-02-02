/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "TextureBase.h"
#include "glad\gl.h"

#include <string>

constexpr GLsizei SHADOWSIZE = 4096;

namespace NCL::Rendering {
	enum class TexType {
		Colour,
		Depth,
		Stencil,
		Shadow,
	};
	class OGLTexture : public TextureBase {
	public:
		OGLTexture(TexType type = TexType::Colour);
		~OGLTexture();

		virtual void Bind() override;
		virtual void Unbind() override;

		void SetEdgeClamp();
		void SetEdgeRepeat();

		inline GLuint GetObjectID() const {
			return texID;
		}

		inline TexType GetType() const {
			return type;
		}

		static TextureBase* RGBATextureFromData(char* data, int width, int height, int channels);

		static TextureBase* RGBATextureFromFilename(const std::string&name);
	protected:
		GLuint texID;
		TexType type;
	private:
		void InitColour();
		void InitDepth();
		void InitStencil();
		void InitShadow();
	};
}

