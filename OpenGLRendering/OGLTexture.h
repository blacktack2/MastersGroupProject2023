/**
 * @file   OGLTexture.h
 * @brief  OpenGL implementation of a texture.
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
	/**
	 * @brief OpenGL implementation of a texture.
	 */
	class OGLTexture : public TextureBase {
	public:
		/**
		 * @brief Construct and upload a new texture of a pre-defined type.
		 * 
		 * @param width  Width (in pixels) of the texture.
		 * @param height Height (in pixels) of the texture.
		 * @param texType Type of texture to initialize (see TexType for more
		 * info).
		 */
		OGLTexture(GLsizei width, GLsizei height, TexType texType);
		/**
		 * @brief Construct and upload a new texture of a specified format.
		 * 
		 * @param width  Width (in pixels) of the texture.
		 * @param height Height (in pixels) of the texture.
		 * @param internalFormat Base or sized texture format. Default GL_RGBA8.
		 * @param type           Data type of the pixel data. Default GL_RGBA.
		 * @param format         Format of the pixel data. Default GL_UNSIGNED_BYTE.
		 * @param data Pixel data to send to the GPU. Set to, or leave as,
		 * nullptr if you wish to create an empty texture.
		 */
		OGLTexture(GLsizei width, GLsizei height, GLint internalFormat = GL_RGBA8, GLenum type = GL_RGBA, GLenum format = GL_UNSIGNED_BYTE, void* data = nullptr);
		~OGLTexture();

		/**
		 * @brief Upload the texture the the GPU with the specified bounds.
		 * 
		 * @param data Pixel data to send to the GPU. Set to, or leave as,
		 * nullptr if you wish to create an empty texture.
		 */
		virtual void Resize(GLsizei width, GLsizei height, void* data = nullptr) override;
		/**
		 * @brief Bind to texture buffer.
		 */
		virtual void Bind() override;
		/**
		 * @brief Set the active texture and bind to texture buffer.
		 * 
		 * @param slot Slot (offset from GL_TEXTURE0) to bind to.
		 */
		virtual void Bind(GLint slot) override;
		/**
		 * @brief Set the active texture, bind to texture buffer, and set the
		 * specified sampler uniform to use this slot.
		 * 
		 * @param slot    Slot (offset from GL_TEXTURE0) to bind to.
		 * @param uniform Sampler uniform location to set the slot of.
		 */
		virtual void Bind(GLint slot, GLint uniform) override;
		/**
		 * @brief Unbind the texture buffer.
		 */
		virtual void Unbind() override;

		/**
		 * @brief Set the edge parameters to clamp mode.
		 */
		void SetEdgeClamp();
		/**
		 * @brief Set the edge parameters to repeat the texture.
		 */
		void SetEdgeRepeat();

		/**
		 * @brief Set the texture magnification and minification filter modes.
		 */
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

