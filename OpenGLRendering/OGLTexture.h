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
		Colour,
		Depth,
		Stencil,
		Shadow,
	};
	class OGLTexture : public TextureBase {
	public:
		OGLTexture(GLsizei width, GLsizei height, TexType texType);
		OGLTexture(GLsizei width, GLsizei height, GLint internalFormat = GL_RGBA8, GLenum type = GL_RGBA, GLenum format = GL_UNSIGNED_BYTE, void* data = nullptr);
		~OGLTexture();

		virtual void Resize(GLsizei width, GLsizei height, void* data = nullptr) override;
		virtual void Bind() override;
		virtual void Bind(GLint slot) override;
		virtual void Bind(GLint slot, GLint uniform) override;
		virtual void Unbind() override;

		void SetEdgeClamp();
		void SetEdgeRepeat();

		void SetFilters(GLfloat minParam, GLfloat magParam);

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
		void InitColour(GLint internalFormat, GLenum type, GLenum format);
		void InitDepth();
		void InitStencil();
		void InitShadow();

		GLint internalFormat;
		GLenum format;
		GLenum type;
	};
}

