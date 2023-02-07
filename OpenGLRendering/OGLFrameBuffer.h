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
			return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		}
	private:
		GLuint fboID;

		GLsizei numColourTexs = 0ull;

		std::vector<OGLTexture*> textures{};
	};
}
