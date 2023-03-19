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

#include <memory>
#include <string>
#include <vector>

namespace NCL {
	namespace Rendering {
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
			OGLTexture(TextureType type, unsigned int width, unsigned int height);
			~OGLTexture() override;

			void Resize(unsigned int width, unsigned int height) override;
			void Upload(void* data, PixelDataFormat format = PixelDataFormat::RGBA, PixelDataType type = PixelDataType::UnsignedByte) override;
			void SoftUpload(void* data, size_t amount, PixelDataFormat format = PixelDataFormat::RGBA, PixelDataType type = PixelDataType::UnsignedByte) override;

			virtual void Initialize() override;

			void Bind() const override;
			void Bind(int slot) const override;
			void Bind(int slot, int uniform) const override;
			void Unbind() const override;

			void SetFilters(MinFilter minFilter, MagFilter magFilter) override;
			void SetEdgeWrap(EdgeWrap edgeWrap) override;

			inline GLuint GetObjectID() const {
				return texID;
			}

			static std::unique_ptr<TextureBase> LoadTexture(const std::string& filename);
			static std::unique_ptr<TextureBase> CreateTexture(TextureType type, unsigned int width, unsigned int height);
			static std::unique_ptr<TextureBase> CreateTextureFromData(char* data, unsigned int width, unsigned int height, int channels);
		private:
			GLuint texID        = 0;
			GLint pixComponents = 0;
			GLenum dummyFormat  = 0;
			GLenum dummyType    = 0;

			unsigned int width, height;

			std::vector<char> data{};
		};
	}
}

