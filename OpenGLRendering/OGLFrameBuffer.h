/**
 * @file   OGLFrameBuffer.h
 * @brief  OpenGL based implementation of a framebuffer.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "FrameBuffer.h"

#include <glad/gl.h>

#include <vector>

namespace NCL::Rendering {
	class OGLTexture;

	class OGLFrameBuffer : public FrameBuffer {
	public:
		OGLFrameBuffer();
		~OGLFrameBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		void AddTexture(OGLTexture* texture);

		void DrawBuffers();

		bool InitSuccess() {
			GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			return status;
		}
	private:
		GLuint fboID;

		GLsizei numColourTexs = 0ull;

		std::vector<OGLTexture*> textures{};
	};
}
