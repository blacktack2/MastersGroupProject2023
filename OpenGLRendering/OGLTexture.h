/**
 * @file   OGLTexture.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "TextureBase.h"
#include "glad\gl.h"

#include <string>

constexpr GLsizei SHADOWSIZE = 4096;

namespace NCL::Rendering {
	enum class TexType {
		Colour4,
		Colour8,
		Colour32,
		Depth,
		Stencil,
		Shadow,
	};
	class OGLTexture : public TextureBase {
	public:
		OGLTexture(GLsizei width, GLsizei height, TexType texType = TexType::Colour32);
		~OGLTexture();

		virtual void Resize(GLsizei width, GLsizei height) override;
		virtual void Bind() override;
		virtual void Bind(GLint slot) override;
		virtual void Bind(GLint slot, GLint uniform) override;
		virtual void Unbind() override;

		void SetEdgeClamp();
		void SetEdgeRepeat();

		inline GLuint GetObjectID() const {
			return texID;
		}

		inline TexType GetType() const {
			return texType;
		}

		static TextureBase* RGBATextureFromData(char* data, int width, int height, int channels);

		static TextureBase* RGBATextureFromFilename(const std::string& filename);
	protected:
		GLuint texID;
		TexType texType;
	private:
		void InitColour4();
		void InitColour8();
		void InitColour32();
		void InitDepth();
		void InitStencil();
		void InitShadow();

		GLint internalFormat;
		GLenum format;
		GLenum type;
	};
}

