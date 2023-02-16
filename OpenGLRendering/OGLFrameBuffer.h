/**
 * @file   OGLFrameBuffer.h
 * @brief  Base OpenGL implementation of a framebuffer.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "FrameBuffer.h"

#include <glad/gl.h>

#include <vector>

namespace NCL::Rendering {
	class OGLTexture;

	/**
	 * @brief Base OpenGL implementation of a framebuffer.
	 */
	class OGLFrameBuffer : public FrameBuffer {
	public:
		OGLFrameBuffer();
		~OGLFrameBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		void AddTexture(OGLTexture* texture);
		void AddTexture(OGLTexture* texture, GLsizei attachment);

		void DrawBuffers();
		void DrawBuffers(GLsizei numBuffers);

		bool InitSuccess() {
			return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		}
	private:
		GLuint fboID;

		GLsizei numColourTexs = 0;

		std::vector<OGLTexture*> textures{};
	};
}
