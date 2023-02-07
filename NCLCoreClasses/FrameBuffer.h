/**
 * @file   FrameBuffer.h
 * @brief  Utility class for encapsulating FrameBuffer functionality.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

namespace NCL::Rendering {
	class FrameBuffer {
	public:
		FrameBuffer();
		virtual ~FrameBuffer();

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
	};
}
