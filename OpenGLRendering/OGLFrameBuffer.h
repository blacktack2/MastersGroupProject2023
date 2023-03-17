/**
 * @file   OGLFrameBuffer.h
 * @brief  OpenGL implementation of a framebuffer.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "FrameBuffer.h"

#include "glad/gl.h"

#include <memory>
#include <vector>

namespace NCL {
	namespace Rendering {
		class OGLTexture;

		/**
		 * @brief Base OpenGL implementation of a framebuffer.
		 */
		class OGLFrameBuffer : public FrameBuffer {
		public:
			OGLFrameBuffer();
			~OGLFrameBuffer();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			void DrawBuffers() const override;
			void DrawBuffers(unsigned int numBuffers) const override;

			bool InitSuccess() const override;

			static std::unique_ptr<FrameBuffer> CreateFrameBuffer();
		protected:
			void BindToTexture(TextureBase& texture, unsigned int attachment) override;
		private:
			GLuint fboID;

			GLsizei numColourTexs = 0;
		};
	}
}
