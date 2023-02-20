/**
 * @file   BufferObjectBase.h
 * @brief  Utility class for encapsulating Shader Storage Buffer Object (SSBO)
 * functionality.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL::Rendering {
	/**
	 * @brief Utility class for encapsulating Shader Storage Buffer Object
	 * (SSBO) functionality.
	 */
	class BufferObjectBase {
	public:
		virtual ~BufferObjectBase();

		/**
		 * @brief Read data from this buffer, or a subset of it, into dataOut.
		 * 
		 * @param dataOut Output buffer this buffer's data will be written to.
		 * @param offset Offset (in bytes) of the first element to read from.
		 * @param amount Number of bytes to read.
		 */
		virtual void Read(void* dataOut, int offset = 0, int amount = 0) = 0;
		/**
		 * @brief Fill the buffer, or a subset of it, with the contents of
		 * dataIn.
		 * 
		 * @param dataIn Input data to be written to this buffer.
		 * @param offset Offset (in bytes) of the first element to write to.
		 * @param amount Number of bytes to write.
		 */
		virtual void Write(const void* dataIn, int offset = 0, int amount = 0) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		/**
		 * @brief Create a new buffer of the specified size, and delete the old
		 * buffer.
		 * @brief After this operation the buffer will be empty.
		 * 
		 * @param to New size (in bytes) of the buffer.
		 */
		void Resize(size_t to) {
			size = to;
			OnResize();
		}
	protected:
		/**
		 * @brief Create a new BufferObject.
		 * 
		 * @param size Size (in bytes) of the buffer.
		 * @param binding Binding position for use in shaders.
		 */
		BufferObjectBase(size_t size, unsigned int binding);

		virtual void OnResize() = 0;

		size_t size;
		unsigned int binding;
	};
}
