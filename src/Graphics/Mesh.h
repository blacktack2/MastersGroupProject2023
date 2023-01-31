/**
 * @file   Mesh.h
 * @brief  Utility class for handling mesh geometry and OpenGL draw calls.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include <glad/gl.h>

#include <glm/vec3.hpp>

#include <memory>

namespace paintHell::core::graphics {
	/**
	 * @brief Utility class for handling mesh geometry and OpenGL draw calls.
	 */
	class Mesh {
	public:
		Mesh();
		~Mesh();

		void draw() const;
		void drawInstanced(GLsizei count) const;

		static std::shared_ptr<Mesh> generateQuad();
	private:
		enum class VBOIndex {
			POSITION = 0ul,
			INDICES,
			MAX
		};

		/**
		 * @brief Upload vertex data to the GPU.
		 */
		void bufferData();
		/**
		 * @brief Upload specific vertex attribute to the GPU.
		 */
		void uploadAttribute(VBOIndex index, GLuint numElements, GLsizei dataTypeSize, GLint attribSize,
			const void* data, const char* debugName);

		GLuint mArrayObject;
		GLuint mVBOs[(size_t)VBOIndex::MAX]{};

		GLuint mType = GL_TRIANGLES;
		GLuint mNumVertices = 0u;
		GLuint mNumIndices = 0u;

		glm::vec3* mPositions = nullptr;
		GLuint* mIndices = nullptr;
	};
}
