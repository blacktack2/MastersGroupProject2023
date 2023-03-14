/**
 * @file   FrameBuffer.h
 * @brief  Utility class for encapsulating FrameBuffer functionality.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include <functional>
#include <vector>

namespace NCL {
	namespace Rendering {
		class TextureBase;

		/**
		 * @brief Utility class for encapsulating FrameBuffer functionality.
		 */
		class FrameBuffer {
		public:
			FrameBuffer();
			virtual ~FrameBuffer();

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			/**
			 * @brief Define the buffers to be drawn to this framebuffer.
			 * @brief Number of colour buffers is assumed to be equal to the number
			 * of added textures. If you are using non-colour textures then use the
			 * other overload of this function.
			 */
			virtual void DrawBuffers() = 0;
			/**
			 * @brief Define the buffers to be drawn to this framebuffer.
			 *
			 * @param numBuffers Number of colour buffers to be drawn to.
			 */
			virtual void DrawBuffers(unsigned int numBuffers) = 0;

			virtual bool InitSuccess() = 0;

			/**
			 * @brief Add and bind a texture to this framebuffer.
			 * @brief Textures are attached in the same order this function is
			 * called.
			 * @brief Any non-colour textures (depth/stencil) should be appended
			 * last.
			 */
			void AddTexture(TextureBase& texture);
			/**
			 * @brief Add and bind a texture to a specific position on this
			 * framebuffer.
			 * @brief If a texture already exists at attachment it will be replaced
			 * with the new one.
			 * @brief If no texture already exists at attachment the new texture
			 * will be appended (not neccessarily at the position specified).
			 */
			void AddTexture(TextureBase& texture, unsigned int attachment);
		protected:
			virtual void BindToTexture(TextureBase& texture, unsigned int attachment) = 0;
		private:
			std::vector<std::reference_wrapper<TextureBase>> textures{};
		};
	}
}
